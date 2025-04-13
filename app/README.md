# SAE Projet - Analyseur de Fichiers avec Protection Turnstile

Projet développé par Ana D'erfurth et Florian SILVA.

## Architecture

Ce projet est composé de trois parties principales :

1. **Frontend** (Nuxt 3 + Shadcn UI) : Interface utilisateur permettant de soumettre des fichiers et d'afficher les résultats d'analyse.
2. **Backend** (Node.js) : API qui gère l'authentification via Turnstile et exécute l'analyse des fichiers.
3. **Redis** : Stockage pour les sessions et autres données temporaires.

## Configuration

### Prérequis

- Docker et Docker Compose
- Clé de site et clé secrète Cloudflare Turnstile

### Variables d'environnement

Créez ou modifiez le fichier `.env` à la racine du projet avec les variables suivantes :

```
CLOUDFLARE_SITE_KEY=votre_cle_de_site_turnstile
CLOUDFLARE_SECRET_KEY=votre_cle_secrete_turnstile
```

## Démarrage rapide

### Avec le script de démarrage

1. Rendez le script exécutable :
   ```
   chmod +x start.sh
   ```

2. Exécutez le script :
   ```
   ./start.sh
   ```

### Manuellement

1. Construisez les images Docker :
   ```
   docker-compose build
   ```

2. Démarrez les conteneurs :
   ```
   docker-compose up -d
   ```

## Accès à l'application

- **Frontend** : http://localhost:3000
- **Backend API** : http://localhost:4000

## Fonctionnalités

- **Protection Turnstile** : Chaque utilisateur doit passer une vérification Turnstile avant d'accéder à l'application.
- **Analyse de fichiers** : Soumettez un fichier pour analyse par le programme C compilé.
- **Interface réactive** : Design moderne avec thème clair/sombre.

## Maintenance

- Pour arrêter les conteneurs :
  ```
  docker-compose down
  ```

- Pour voir les logs :
  ```
  docker-compose logs -f
  ```

## Structure du projet

- `/frontend` : Application Nuxt 3
- `/backend` : API Node.js
- `docker-compose.yml` : Configuration Docker Compose
- `.env` : Variables d'environnement 