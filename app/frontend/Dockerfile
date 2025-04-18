FROM node:20-alpine

WORKDIR /app

# Installer les dépendances système nécessaires
RUN apk add --no-cache python3 make g++ gcc sqlite-dev curl

# Configurer npm pour ignorer les modules binaires optionnels
ENV CYPRESS_INSTALL_BINARY=0
ENV PRISMA_SKIP_POSTINSTALL_GENERATE=1
ENV NEXT_TELEMETRY_DISABLED=1
ENV NODE_OPTIONS="--max-old-space-size=4096"

# Copier les fichiers de config
COPY package.json ./

# Installation propre
RUN rm -rf node_modules package-lock.json
RUN npm cache clean --force

# Installation de base des dépendances
RUN npm install

# Copier le script d'entrée et le reste des fichiers
COPY docker-entrypoint.sh ./
COPY . .

# Rendre le script d'entrée exécutable
RUN chmod +x /app/docker-entrypoint.sh

# Configurer l'accès en écriture
RUN mkdir -p /app/.nuxt /app/.output
RUN chmod -R 777 /app/.nuxt /app/.output

# Exposer le port
EXPOSE 3000

# Utiliser le script d'entrée comme ENTRYPOINT
ENTRYPOINT ["/app/docker-entrypoint.sh"]

# Commande par défaut
CMD ["npm", "run", "dev"] 