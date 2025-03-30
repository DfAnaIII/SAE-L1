# SAE-L1 Application

Cette application est construite avec Nuxt.js pour le frontend et Go pour le backend, utilisant Redis pour le stockage des sessions.

## Prérequis

- Docker et Docker Compose
- Compte Cloudflare avec Turnstile activé

## Configuration

1. Clonez le dépôt :
```bash
git clone https://github.com/DfAnaIII/SAE-L1.git
cd SAE-L1
```

2. Créez un fichier `.env` à la racine du projet avec vos clés Cloudflare :
```env
CLOUDFLARE_SITE_KEY=votre_clé_site_cloudflare
CLOUDFLARE_SECRET_KEY=votre_clé_secrète_cloudflare
```

## Démarrage

Pour lancer l'application en mode développement :

```bash
docker-compose up --build
```

L'application sera accessible à :
- Frontend : http://localhost:3000
- Backend : http://localhost:4000
- Redis : localhost:6379

## Structure du Projet

```
app/
├── frontend/          # Application Nuxt.js
├── backend/          # Serveur Go
│   └── script/      # Scripts C
├── docker-compose.yml
└── .env
```

## Fonctionnalités

- Authentification via Cloudflare Turnstile
- Gestion des sessions avec Redis
- Interface utilisateur responsive avec thème sombre/clair
- Compilation et exécution de scripts C

## Sécurité

- Protection contre les attaques DDoS via Cloudflare Turnstile
- Stockage sécurisé des sessions dans Redis
- Validation des entrées côté serveur

## Contribution

Les contributions sont les bienvenues ! N'hésitez pas à ouvrir une issue ou un pull request. 