#!/bin/bash

# Charger les variables d'environnement
echo "Chargement des variables d'environnement..."
set -a
source .env
set +a

# Arrêter les conteneurs existants s'ils existent
echo "Arrêt des conteneurs existants..."
docker-compose down

# Construire les images
echo "Construction des images Docker..."
docker-compose build

# Démarrer les conteneurs
echo "Démarrage des conteneurs..."
docker-compose up -d

echo "Application démarrée! Frontend sur http://localhost:3030, Backend sur http://localhost:4030" 