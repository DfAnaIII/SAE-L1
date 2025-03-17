const express = require('express');
const cors = require('cors');
const cookieParser = require('cookie-parser');


const { logger, safeExtractRequestInfo } = require('./utils/logger');
const securityMiddleware = require('./middlewares/security');
const { validateRequest } = require('./middlewares/validation');
const errorHandler = require('./middlewares/errorHandler');

const addContent = require('./routes/addContent.js');
const getContent = require('./routes/getContent.js');
const updateContent = require('./routes/updateContent.js');
const deleteContent = require('./routes/deleteContent.js');

// Création de l'application Express
const app = express();

// Route de healthcheck (avant tous les middlewares)
app.get('/health', (req, res) => {
    res.status(200).json({ status: 'ok' });
});

// Configuration CORS détaillée
const corsOptions = {
    origin: [
        'http://localhost:8080', 
        'http://frontend:8080', 
        'http://localhost:3000',
        'http://localhost', 
        'http://127.0.0.1:8080',
        'http://127.0.0.1',
        'http://127.0.0.1:3000',
        process.env.CORS_ORIGIN || '*'
    ],
    methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
    allowedHeaders: ['Content-Type', 'Authorization', 'X-Requested-With'],
    credentials: true,
    maxAge: 86400 // 24 heures
};

// Middlewares de base
app.use(cors(corsOptions));
app.use(express.json({ 
    limit: process.env.JSON_BODY_LIMIT || '10kb',
    verify: (req, res, buf) => { req.rawBody = buf } // Pour la validation des webhooks
}));
app.use(express.urlencoded({ extended: true, limit: process.env.URLENCODED_LIMIT || '10kb' }));
app.use(cookieParser(process.env.COOKIE_SECRET));

// Middlewares de sécurité
app.use(securityMiddleware);

// Validation des requêtes
app.use(validateRequest);

// Middleware de logging
app.use((req, res, next) => {
    try {
        const start = Date.now();
        
        // Extraction sécurisée des informations de requête
        const requestInfo = safeExtractRequestInfo(req);
        
        // Loggons immédiatement la requête entrante
        logger.http({
            message: 'Requête entrante',
            ...requestInfo
        });
        
        res.on('finish', () => {
            try {
                const duration = Date.now() - start;
                
                // Mise à jour des informations avec le status code et la durée
                logger.http({
                    message: 'Requête terminée',
                    ...requestInfo,
                    status: res.statusCode,
                    duration: `${duration}ms`
                });
            } catch (loggingError) {
                console.error('Erreur lors du logging de fin de requête:', loggingError);
            }
        });
        
        next();
    } catch (error) {
        console.error('Erreur critique dans le middleware de logging:', error);
        // On continue le traitement de la requête même si le logging échoue
        next();
    }
});


// Gestion des routes non trouvées
app.all('*', (req, res) => {
    // Extraction sécurisée des informations
    const requestInfo = safeExtractRequestInfo(req);
    
    logger.warn({
        message: 'Route non trouvée',
        ...requestInfo
    });
    
    res.status(404).json({
        status: 'error',
        message: `La route ${requestInfo.path || req.originalUrl || 'demandée'} n'existe pas sur ce serveur`
    });
});

// Middleware de gestion des erreurs
app.use(errorHandler);

// Gestion des erreurs non capturées
process.on('unhandledRejection', (err) => {
    logger.error('Erreur non gérée (Promise):', err);
    // En production, on ne quitte pas le processus mais on notifie
    if (process.env.NODE_ENV === 'production') {
        // Notification aux administrateurs
    }
});

process.on('uncaughtException', (err) => {
    logger.error('Exception non capturée:', err);
    // En production, on termine proprement le processus
    if (process.env.NODE_ENV === 'production') {
        // Notification aux administrateurs
        process.exit(1);
    }
});

module.exports = app;
