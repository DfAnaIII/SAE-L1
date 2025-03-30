FROM node:18-alpine

# Installation de gcc et des dépendances nécessaires
RUN apk add --no-cache gcc musl-dev

WORKDIR /app

# Copier les fichiers de configuration
COPY package*.json ./

# Installer les dépendances
RUN npm install

# Copier le code source
COPY . .

# Créer le dossier pour les scripts C
RUN mkdir -p src/scripts

# Compiler les scripts C
RUN gcc src/scripts/main.c -o main && \
    gcc src/scripts/verify.c -o verify

# Exposer le port
EXPOSE 4000

# Démarrer l'application
CMD ["npm", "start"]
