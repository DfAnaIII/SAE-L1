class SessionService {
    constructor() {
        // En production, on utiliserait une base de données
        // Pour le développement, on utilise un Map en mémoire
        this.sessions = new Map();
        this.SESSION_DURATION = 2 * 60 * 60 * 1000; // 2 heures en millisecondes
    }

    createSession() {
        const sessionId = require('crypto').randomUUID();
        const session = {
            id: sessionId,
            createdAt: Date.now(),
            expiresAt: Date.now() + this.SESSION_DURATION
        };
        this.sessions.set(sessionId, session);
        return session;
    }

    validateSession(sessionId) {
        const session = this.sessions.get(sessionId);
        
        if (!session) {
            return false;
        }

        // Vérifier si la session a expiré
        if (Date.now() > session.expiresAt) {
            this.sessions.delete(sessionId);
            return false;
        }

        // Prolonger la session
        session.expiresAt = Date.now() + this.SESSION_DURATION;
        return true;
    }

    invalidateSession(sessionId) {
        this.sessions.delete(sessionId);
    }
}

module.exports = new SessionService(); 