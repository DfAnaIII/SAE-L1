# Utilisation de l'image Node.js officielle
FROM node:18-alpine

# Création du répertoire de travail
WORKDIR /usr/src/app

# Installation des dépendances Node.js
COPY package*.json ./

# Installation des dépendances
RUN npm install --omit=dev

# Copie du code source
COPY . .

# Création du dossier logs
RUN mkdir -p logs

# Exposition du port
EXPOSE 3000

# Variables d'environnement par défaut
ENV NODE_ENV=production \
    PORT=3000

# Ajout du healthcheck
HEALTHCHECK --interval=30s --timeout=10s --start-period=30s --retries=3 \
    CMD curl -f http://localhost:3000/health || exit 1

# Commande de démarrage
CMD ["node", "src/server.js"]
