FROM node:20-alpine

WORKDIR /app

# Installer les dépendances système nécessaires pour better-sqlite3
RUN apk add --no-cache python3 make g++ gcc sqlite-dev

# Installer les dépendances
COPY package.json package-lock.json ./
RUN npm install
# Copier le reste des fichiers de l'application
COPY . .

# S'assurer que la configuration Tailwind est correcte
RUN rm -f postcss.config.js
# Créer un fichier postcss.config.cjs en CommonJS
RUN echo "module.exports = { plugins: { 'postcss-import': {}, tailwindcss: {}, autoprefixer: {} } }" > postcss.config.cjs
RUN npm install -D postcss-import

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 