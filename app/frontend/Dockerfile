# Stage de build
FROM node:20-alpine AS builder

# Installation des dépendances système nécessaires
RUN apk add --no-cache python3 make g++

WORKDIR /app

# Copie des fichiers de dépendances
COPY package*.json ./

# Installation des dépendances avec --legacy-peer-deps et nettoyage du cache npm
RUN npm install --legacy-peer-deps && \
    npm cache clean --force

# Copie du reste des fichiers
COPY . .

# Configuration de l'environnement
ENV NODE_ENV=production

# Build de l'application
RUN npm run build

# Stage de production
FROM node:20-alpine

WORKDIR /app

# Copie des fichiers nécessaires depuis le stage de build
COPY --from=builder /app/.output ./.output
COPY --from=builder /app/package*.json ./

# Installation des dépendances de production uniquement avec --legacy-peer-deps
RUN npm install --production --legacy-peer-deps && \
    npm cache clean --force

# Exposition du port
EXPOSE 3000

# Commande de démarrage
CMD ["node", ".output/server/index.mjs"] 