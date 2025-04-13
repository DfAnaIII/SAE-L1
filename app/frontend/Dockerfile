FROM node:20-alpine

WORKDIR /app

# Installer les dépendances système nécessaires pour better-sqlite3
RUN apk add --no-cache python3 make g++ gcc sqlite-dev

# Installer les dépendances
COPY package.json package-lock.json ./
RUN npm install
# Copier le reste des fichiers de l'application
COPY . .

# Créer un fichier postcss.config.js compatible avec Tailwind v4
RUN echo "module.exports = { plugins: { tailwindcss: {}, autoprefixer: {} } }" > postcss.config.js
RUN npm install -D tailwindcss@latest autoprefixer@latest postcss@latest

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 