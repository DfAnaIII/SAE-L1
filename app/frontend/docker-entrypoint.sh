#!/bin/sh
set -e

# Désinstaller les packages problématiques
npm uninstall tailwindcss postcss autoprefixer @tailwindcss/postcss

# Installer des versions spécifiques
npm install -D tailwindcss@2.2.19 postcss@8.2.15 autoprefixer@10.2.6

# Modifier package.json pour remplacer "type": "module" par "type": "commonjs"
sed -i 's/"type": "module"/"type": "commonjs"/g' package.json

# Créer un fichier tailwind.css basique
cat > assets/css/tailwind.css << 'EOF'
@tailwind base;
@tailwind components;
@tailwind utilities;
EOF

# Créer un fichier tailwind.config.js (peut être .js maintenant que le package.json est modifié)
cat > tailwind.config.js << 'EOF'
module.exports = {
  mode: 'jit',
  purge: [
    './components/**/*.{js,vue,ts}',
    './layouts/**/*.vue',
    './pages/**/*.vue',
    './plugins/**/*.{js,ts}',
    './app.vue'
  ],
  darkMode: 'class',
  theme: {
    extend: {}
  },
  variants: {
    extend: {}
  },
  plugins: []
}
EOF

# Supprimer les fichiers de configuration pour éviter les conflits
rm -f tailwind.config.cjs
rm -f tailwind.config.ts

# Exécuter la commande spécifiée
exec "$@" 