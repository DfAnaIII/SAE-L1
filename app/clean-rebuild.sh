#!/bin/bash

# Arrêter tous les conteneurs
echo "Arrêt des conteneurs..."
docker-compose down

# Supprimer les images
echo "Suppression des images Docker..."
docker rmi app_frontend app_backend || true

# Nettoyer le cache Docker builder
echo "Nettoyage du cache Docker..."
docker builder prune -f

# Reconstruire sans cache
echo "Reconstruction des images..."
docker-compose build --no-cache

# Démarrer les conteneurs
echo "Démarrage des conteneurs..."
docker-compose up -d

echo "Reconstruction terminée. Frontend sur http://localhost:3030, Backend sur http://localhost:4040" 