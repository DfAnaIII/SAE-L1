const axios = require('axios');

class TurnstileService {
    constructor() {
        this.secretKey = process.env.CLOUDFLARE_SECRET_KEY;
        this.verifyUrl = 'https://challenges.cloudflare.com/turnstile/v0/siteverify';
    }

    async verifyToken(token, ip) {
        try {
            const formData = new URLSearchParams();
            formData.append('secret', this.secretKey);
            formData.append('response', token);
            if (ip) {
                formData.append('remoteip', ip);
            }

            const response = await axios.post(this.verifyUrl, formData, {
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                }
            });

            return {
                success: response.data.success,
                hostname: response.data.hostname,
                challenge_ts: response.data.challenge_ts,
                errorCodes: response.data['error-codes'] || [],
                action: response.data.action,
                cdata: response.data.cdata
            };
        } catch (error) {
            console.error('Erreur lors de la vérification Turnstile:', error);
            return {
                success: false,
                errorCodes: ['internal-error']
            };
        }
    }
}

module.exports = new TurnstileService(); 