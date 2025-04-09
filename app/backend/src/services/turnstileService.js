const axios = require('axios');
const sessionService = require('./sessionService');

class TurnstileService {
    constructor() {
        this.secretKey = process.env.TURNSTILE_SECRET_KEY;
        this.verifyUrl = 'https://challenges.cloudflare.com/turnstile/v0/siteverify';
    }

    async verifyToken(token) {
        try {
            const response = await axios.post(this.verifyUrl, {
                secret: this.secretKey,
                response: token
            });

            const verificationResult = {
                success: response.data.success,
                challengeTimestamp: response.data.challenge_ts,
                hostname: response.data.hostname,
                errorCodes: response.data['error-codes']
            };

            if (verificationResult.success) {
                // Créer une nouvelle session si la vérification est réussie
                const session = sessionService.createSession();
                return {
                    ...verificationResult,
                    sessionId: session.id
                };
            }

            return verificationResult;
        } catch (error) {
            console.error('Erreur lors de la vérification du token Turnstile:', error);
            throw new Error('Erreur lors de la vérification du token');
        }
    }
}

module.exports = new TurnstileService(); 