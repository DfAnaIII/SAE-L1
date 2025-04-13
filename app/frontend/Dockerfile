FROM node:20-alpine

WORKDIR /app

# Installer les dépendances système nécessaires pour better-sqlite3
RUN apk add --no-cache python3 make g++ gcc sqlite-dev

# Copier uniquement les fichiers nécessaires pour l'installation
COPY package.json package-lock.json ./

# Nettoyer l'installation existante et installer des versions spécifiques compatibles
RUN npm uninstall tailwindcss postcss autoprefixer
RUN npm install
RUN npm install -D tailwindcss@3.3.5 postcss@8.4.31 autoprefixer@10.4.16

# Copier le reste des fichiers
COPY . .

# Configurer correctement PostCSS et Tailwind
RUN rm -f postcss.config.js tailwind.config.js
RUN echo "module.exports = { plugins: { tailwindcss: {}, autoprefixer: {} } }" > postcss.config.js
RUN echo "/** @type {import('tailwindcss').Config} */ module.exports = { content: ['./components/**/*.{js,vue,ts}', './layouts/**/*.vue', './pages/**/*.vue', './plugins/**/*.{js,ts}', './app.vue'], theme: { extend: {} }, plugins: [] }" > tailwind.config.js

# Corriger le fichier CSS Tailwind
RUN echo "@tailwind base;\n@tailwind components;\n@tailwind utilities;" > assets/css/tailwind.css

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 