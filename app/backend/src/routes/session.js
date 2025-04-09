const express = require('express');
const router = express.Router();
const sessionService = require('../services/sessionService');
const { logger } = require('../utils/logger');

router.post('/check', async (req, res) => {
    try {
        const { sessionId } = req.body;

        if (!sessionId) {
            logger.warn('Tentative de vérification de session sans ID');
            return res.status(400).json({
                success: false,
                error: 'ID de session manquant'
            });
        }

        const isValid = sessionService.validateSession(sessionId);

        if (!isValid) {
            logger.warn('Session invalide ou expirée', { sessionId });
            return res.status(401).json({
                success: false,
                error: 'Session invalide ou expirée'
            });
        }

        logger.info('Session vérifiée avec succès', { sessionId });
        res.json({
            success: true
        });

    } catch (error) {
        logger.error('Erreur lors de la vérification de la session:', error);
        res.status(500).json({
            success: false,
            error: 'Erreur interne du serveur'
        });
    }
});

module.exports = router; 