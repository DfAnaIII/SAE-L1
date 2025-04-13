FROM node:20-alpine

WORKDIR /app

# Installer les dépendances système nécessaires pour better-sqlite3
RUN apk add --no-cache python3 make g++ gcc sqlite-dev

# Installer les dépendances
COPY package.json package-lock.json ./
RUN npm install
# RUN npm install tailwindcss @tailwindcss/postcss postcss
# Copier le reste des fichiers de l'application
COPY . .

# S'assurer que nous avons les plugins PostCSS nécessaires pour Tailwind
RUN npm uninstall tailwindcss postcss autoprefixer
RUN npm install -D tailwindcss@latest @tailwindcss/postcss7-compat postcss@^8.0.0 autoprefixer@^10.0.0

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 