#!/bin/bash

# Arrêter tous les conteneurs
echo "Arrêt des conteneurs..."
docker-compose down

# Supprimer les volumes pour un nettoyage complet
echo "Suppression des volumes..."
docker volume rm app_frontend_node_modules app_nuxt_data || true

# Supprimer les images
echo "Suppression des images Docker..."
docker rmi app_frontend app_backend || true

# Nettoyer le cache Docker builder
echo "Nettoyage du cache Docker..."
docker builder prune -f

# Nettoyer les dossiers node_modules dans frontend
echo "Nettoyage des modules Node.js et des fichiers de configuration conflictuels..."
rm -rf frontend/node_modules
rm -rf frontend/.nuxt
rm -rf frontend/.output
rm -f frontend/postcss.config.js frontend/postcss.config.ts frontend/postcss.config.cjs
rm -f frontend/vite.config.js frontend/vite.config.ts frontend/vite.config.cjs

# Reconstruire sans cache
echo "Reconstruction des images..."
docker-compose build --no-cache

# Démarrer les conteneurs
echo "Démarrage des conteneurs..."
docker-compose up -d

echo "Reconstruction terminée. Frontend sur http://localhost:3030, Backend sur http://localhost:4040" 