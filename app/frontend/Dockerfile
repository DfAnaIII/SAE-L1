FROM node:18-alpine

WORKDIR /app

# Définition des variables d'environnement
ENV NODE_ENV=production

# Installer les dépendances système nécessaires
RUN apk add --no-cache python3 make g++ gcc sqlite-dev curl

# Copier les fichiers de config
COPY package.json ./

# Installation propre
RUN rm -rf node_modules package-lock.json
RUN npm cache clean --force

# Installer les dépendances, y compris Tailwind v3 (compatible avec shadcn)
RUN npm install
RUN npm uninstall -g tailwindcss postcss autoprefixer
RUN npm uninstall tailwindcss postcss autoprefixer
RUN npm install -D tailwindcss@3.4.1 postcss@8.4.35 autoprefixer@10.4.17

# Créer un fichier postcss.config.js standard
RUN echo "module.exports = { plugins: { tailwindcss: {}, autoprefixer: {} } }" > postcss.config.js

# Copier le reste des fichiers
COPY . .

# Utiliser le fichier tailwind.config.ts existant au lieu de le créer
RUN cp tailwind.config.ts tailwind.config.js

# Créer le fichier CSS Tailwind
RUN mkdir -p assets/css
RUN echo '@tailwind base;' > assets/css/tailwind.css
RUN echo '@tailwind components;' >> assets/css/tailwind.css
RUN echo '@tailwind utilities;' >> assets/css/tailwind.css
RUN echo '' >> assets/css/tailwind.css
RUN echo '@layer base {' >> assets/css/tailwind.css
RUN echo '  * { @apply border-border; }' >> assets/css/tailwind.css
RUN echo '  body { @apply bg-background text-foreground; }' >> assets/css/tailwind.css
RUN echo '}' >> assets/css/tailwind.css

# Remplacer nuxt.config.ts pour utiliser une version minimaliste
RUN cp nuxt.config.ts nuxt.config.ts.bak

# Créer un fichier temporaire pour nuxt.config.ts
RUN echo "module.exports = { compatibilityDate: '2024-11-01', ssr: false, devtools: { enabled: false }, modules: ['@nuxt/fonts', '@nuxt/icon', '@nuxt/scripts', 'shadcn-nuxt', '@nuxtjs/turnstile'], css: ['~/assets/css/tailwind.css'], postcss: { plugins: { tailwindcss: {}, autoprefixer: {} } }, shadcn: { prefix: '', componentDir: './components/ui' }, turnstile: { siteKey: process.env.CLOUDFLARE_SITE_KEY }, runtimeConfig: { turnstileSecretKey: process.env.CLOUDFLARE_SECRET_KEY, public: { apiUrl: process.env.API_URL || 'http://localhost:4000', turnstileSiteKey: process.env.CLOUDFLARE_SITE_KEY } } }" > nuxt.config.js
RUN sed -i 's/module.exports/export default defineNuxtConfig/' nuxt.config.js
RUN mv nuxt.config.js nuxt.config.ts

# Configurer l'accès en écriture
RUN mkdir -p /app/.nuxt /app/.output
RUN chmod -R 777 /app/.nuxt /app/.output

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 