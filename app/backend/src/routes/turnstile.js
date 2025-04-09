const express = require('express');
const router = express.Router();
const turnstileService = require('../services/turnstileService');
const { logger } = require('../utils/logger');

router.post('/validate', async (req, res) => {
    try {
        const { token } = req.body;

        if (!token) {
            logger.warn('Tentative de validation sans token');
            return res.status(400).json({
                success: false,
                error: 'Token manquant'
            });
        }

        const verificationResult = await turnstileService.verifyToken(token);

        if (!verificationResult.success) {
            logger.warn('Validation Turnstile échouée', { errorCodes: verificationResult.errorCodes });
            return res.status(403).json({
                success: false,
                error: 'Validation échouée',
                errorCodes: verificationResult.errorCodes
            });
        }

        logger.info('Validation Turnstile réussie', {
            hostname: verificationResult.hostname,
            timestamp: verificationResult.challengeTimestamp
        });

        // Générer un ID de session unique
        const sessionId = require('crypto').randomUUID();

        res.json({
            success: true,
            sessionId
        });

    } catch (error) {
        logger.error('Erreur lors de la validation Turnstile:', error);
        res.status(500).json({
            success: false,
            error: 'Erreur interne du serveur'
        });
    }
});

module.exports = router; 