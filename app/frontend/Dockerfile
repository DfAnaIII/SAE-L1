FROM node:18-alpine

WORKDIR /app

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
RUN npm install -D tailwindcss@3.3.5 postcss@8.4.31 autoprefixer@10.4.16

# Créer un fichier postcss.config.js standard
RUN echo "module.exports = { plugins: { tailwindcss: {}, autoprefixer: {} } }" > postcss.config.js

# Créer un fichier tailwind.config.js pour shadcn
RUN echo "/** @type {import('tailwindcss').Config} */ module.exports = { darkMode: 'class', content: ['./components/**/*.{js,vue,ts}', './layouts/**/*.vue', './pages/**/*.vue', './plugins/**/*.{js,ts}', './app.vue'], theme: { extend: { colors: { border: 'var(--border)', input: 'var(--input)', ring: 'var(--ring)', background: 'var(--background)', foreground: 'var(--foreground)', primary: { DEFAULT: 'var(--primary)', foreground: 'var(--primary-foreground)' }, secondary: { DEFAULT: 'var(--secondary)', foreground: 'var(--secondary-foreground)' }, destructive: { DEFAULT: 'var(--destructive)', foreground: 'var(--destructive-foreground)' }, muted: { DEFAULT: 'var(--muted)', foreground: 'var(--muted-foreground)' }, accent: { DEFAULT: 'var(--accent)', foreground: 'var(--accent-foreground)' }, popover: { DEFAULT: 'var(--popover)', foreground: 'var(--popover-foreground)' }, card: { DEFAULT: 'var(--card)', foreground: 'var(--card-foreground)' } }, borderRadius: { lg: 'var(--radius)', md: 'calc(var(--radius) - 2px)', sm: 'calc(var(--radius) - 4px)' } } }, plugins: [] }" > tailwind.config.js

# Copier le reste des fichiers
COPY . .

# Remplacer le fichier CSS Tailwind avec la syntaxe v3 correcte
RUN echo "@tailwind base;\n@tailwind components;\n@tailwind utilities;" > assets/css/tailwind.css

# Remplacer nuxt.config.ts pour utiliser une version minimaliste
RUN cp nuxt.config.ts nuxt.config.ts.bak
RUN echo "export default defineNuxtConfig({ ssr: false, devtools: { enabled: false }, modules: ['@nuxt/fonts', '@nuxt/icon', '@nuxt/scripts', 'shadcn-nuxt', '@nuxtjs/turnstile'], css: ['~/assets/css/tailwind.css'], postcss: { plugins: { tailwindcss: {}, autoprefixer: {} } }, shadcn: { prefix: '', componentDir: './components/ui' }, turnstile: { siteKey: process.env.CLOUDFLARE_SITE_KEY }, runtimeConfig: { turnstileSecretKey: process.env.CLOUDFLARE_SECRET_KEY, public: { apiUrl: process.env.API_URL || 'http://localhost:4000', turnstileSiteKey: process.env.CLOUDFLARE_SITE_KEY } } })" > nuxt.config.ts

# Configurer l'accès en écriture
RUN mkdir -p /app/.nuxt /app/.output
RUN chmod -R 777 /app/.nuxt /app/.output

# Construire l'application
RUN npm run build

# Exposer le port
EXPOSE 3000

# Commande de démarrage
CMD ["npm", "run", "start"] 