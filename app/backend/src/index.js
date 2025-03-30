const express = require('express');
const cors = require('cors');
const multer = require('multer');
const { createClient } = require('redis');
const { exec } = require('child_process');
const path = require('path');
const fs = require('fs').promises;
const winston = require('winston');
const rateLimit = require('express-rate-limit');
const helmet = require('helmet');
const compression = require('compression');
const morgan = require('morgan');
const { v4: uuidv4 } = require('uuid');
const chalk = require('chalk');
const turnstileService = require('./services/turnstile');
require('dotenv').config();

// Configuration du logger Winston
const logger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.colorize(),
        winston.format.printf(({ timestamp, level, message }) => {
            return `${chalk.gray(timestamp)} ${level} ${message}`;
        })
    ),
    transports: [
        new winston.transports.Console(),
        new winston.transports.File({ filename: 'error.log', level: 'error' }),
        new winston.transports.File({ filename: 'combined.log' })
    ]
});

const app = express();
const upload = multer({ dest: 'uploads/' });

// Configuration Redis
const redisClient = createClient({
    url: process.env.REDIS_URL
});

redisClient.on('error', err => logger.error('Redis Client Error:', err));
redisClient.connect();

// Middleware de sécurité
app.use(helmet());
app.use(compression());
app.use(cors());
app.use(express.json());

// Configuration du rate limiting
const limiter = rateLimit({
    windowMs: 15 * 60 * 1000, // 15 minutes
    max: 100, // limite chaque IP à 100 requêtes par fenêtre
    message: { error: 'Trop de requêtes, veuillez réessayer plus tard.' }
});

// Middleware de logging personnalisé
app.use((req, res, next) => {
    const requestId = uuidv4();
    req.requestId = requestId;
    
    logger.info(`${chalk.blue(req.method)} ${chalk.green(req.url)} - ${chalk.yellow(requestId)}`);
    
    res.on('finish', () => {
        const status = res.statusCode;
        const color = status < 400 ? chalk.green : status < 500 ? chalk.yellow : chalk.red;
        logger.info(`${color(status)} ${chalk.blue(req.method)} ${chalk.green(req.url)} - ${chalk.yellow(requestId)}`);
    });
    
    next();
});

// Middleware de rate limiting
app.use(limiter);

// Middleware de gestion des erreurs
app.use((err, req, res, next) => {
    logger.error(`${chalk.red('Error:')} ${err.message}`);
    res.status(500).json({ error: 'Erreur serveur', requestId: req.requestId });
});

// Routes
app.post('/content', upload.single('file'), async (req, res) => {
    const startTime = Date.now();
    try {
        if (!req.file) {
            logger.warn(`${chalk.yellow('Warning:')} Aucun fichier fourni pour /content`);
            return res.status(400).json({ error: 'Aucun fichier fourni' });
        }

        logger.info(`${chalk.blue('Processing file:')} ${req.file.originalname}`);
        const fileContent = await fs.readFile(req.file.path, 'utf8');
        const scriptPath = path.join(__dirname, '../src/scripts/main.c');
        
        exec(`gcc ${scriptPath} -o main && ./main "${fileContent}"`, async (error, stdout, stderr) => {
            const duration = Date.now() - startTime;
            
            if (error) {
                logger.error(`${chalk.red('Error:')} Erreur lors de l'exécution du script: ${error.message}`);
                return res.status(500).json({ error: 'Erreur lors de l\'exécution du script' });
            }

            await fs.unlink(req.file.path);
            logger.info(`${chalk.green('Success:')} Fichier traité en ${duration}ms`);

            const actions = stdout.trim().split('\n').reduce((acc, line, index) => {
                acc[index + 1] = line;
                return acc;
            }, {});

            res.json(actions);
        });
    } catch (error) {
        logger.error(`${chalk.red('Error:')} ${error.message}`);
        res.status(500).json({ error: 'Erreur serveur' });
    }
});

app.post('/verify', upload.single('file'), async (req, res) => {
    const startTime = Date.now();
    try {
        if (!req.file) {
            logger.warn(`${chalk.yellow('Warning:')} Aucun fichier fourni pour /verify`);
            return res.status(400).json({ error: 'Aucun fichier fourni' });
        }

        logger.info(`${chalk.blue('Verifying file:')} ${req.file.originalname}`);
        const fileContent = await fs.readFile(req.file.path, 'utf8');
        const verifyScriptPath = path.join(__dirname, '../src/scripts/verify.c');
        
        exec(`gcc ${verifyScriptPath} -o verify && ./verify "${fileContent}"`, async (error, stdout, stderr) => {
            const duration = Date.now() - startTime;
            
            if (error) {
                logger.error(`${chalk.red('Error:')} Erreur lors de la vérification: ${error.message}`);
                return res.status(500).json({ error: 'Something went wrong' });
            }

            await fs.unlink(req.file.path);
            const result = stdout.trim();
            
            if (result === 'Correct') {
                logger.info(`${chalk.green('Success:')} Vérification réussie en ${duration}ms`);
                res.status(200).json({ status: 'Correct' });
            } else {
                logger.warn(`${chalk.yellow('Warning:')} Vérification échouée en ${duration}ms`);
                res.status(401).json({ status: 'Incorrect' });
            }
        });
    } catch (error) {
        logger.error(`${chalk.red('Error:')} ${error.message}`);
        res.status(500).json({ error: 'Something went wrong' });
    }
});

app.post('/verify-challenge', async (req, res) => {
    const startTime = Date.now();
    try {
        const { token } = req.body;
        const ip = req.ip;

        if (!token) {
            logger.warn(`${chalk.yellow('Warning:')} Token Turnstile manquant`);
            return res.status(400).json({ error: 'Token manquant' });
        }

        logger.info(`${chalk.blue('Verifying Turnstile token')}`);
        const result = await turnstileService.verifyToken(token, ip);
        const duration = Date.now() - startTime;

        if (result.success) {
            // Générer un sessionId et le stocker dans Redis
            const sessionId = uuidv4();
            await redisClient.set(sessionId, 'valid', { EX: 3600 }); // Expire après 1 heure

            logger.info(`${chalk.green('Success:')} Token Turnstile validé en ${duration}ms`);
            res.json({ 
                success: true, 
                sessionId,
                hostname: result.hostname,
                challenge_ts: result.challenge_ts
            });
        } else {
            logger.warn(`${chalk.yellow('Warning:')} Token Turnstile invalide en ${duration}ms`);
            res.status(401).json({ 
                success: false, 
                errorCodes: result.errorCodes 
            });
        }
    } catch (error) {
        logger.error(`${chalk.red('Error:')} ${error.message}`);
        res.status(500).json({ error: 'Erreur serveur' });
    }
});

app.post('/check', async (req, res) => {
    const startTime = Date.now();
    try {
        const { sessionId } = req.body;
        
        if (!sessionId) {
            logger.warn(`${chalk.yellow('Warning:')} Session ID manquant pour /check`);
            return res.status(400).json({ error: 'Session ID manquant' });
        }

        const exists = await redisClient.exists(sessionId);
        const duration = Date.now() - startTime;
        
        if (exists) {
            logger.info(`${chalk.green('Success:')} Session valide vérifiée en ${duration}ms`);
            res.json({ valid: true });
        } else {
            logger.warn(`${chalk.yellow('Warning:')} Session invalide détectée en ${duration}ms`);
            res.status(401).json({ valid: false });
        }
    } catch (error) {
        logger.error(`${chalk.red('Error:')} ${error.message}`);
        res.status(500).json({ error: 'Erreur serveur' });
    }
});

// Démarrage du serveur
const PORT = process.env.PORT || 4000;
app.listen(PORT, () => {
    logger.info(`${chalk.green('Server started on port')} ${chalk.blue(PORT)}`);
}); 