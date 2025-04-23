#!/bin/sh
set -e

# Désinstaller les packages problématiques
npm uninstall -g tailwindcss postcss autoprefixer
npm uninstall tailwindcss postcss autoprefixer

# Installer les dépendances nécessaires
npm install -D @nuxtjs/tailwindcss@6.8.0 @nuxtjs/color-mode@3.3.0

# Assurer que le type est commonjs
sed -i 's/"type": "module"/"type": "commonjs"/g' package.json

# Créer un fichier tailwind.css basique
cat > assets/css/tailwind.css << 'EOF'
@tailwind base;
@tailwind components;
@tailwind utilities;

@layer base {
  :root {
    --background: #ffffff;
    --foreground: #1f2937;
    --card: #ffffff;
    --card-foreground: #1f2937;
    --popover: #ffffff;
    --popover-foreground: #1f2937;
    --primary: #4b5563;
    --primary-foreground: #f9fafb;
    --secondary: #f3f4f6;
    --secondary-foreground: #4b5563;
    --muted: #f3f4f6;
    --muted-foreground: #6b7280;
    --accent: #f3f4f6;
    --accent-foreground: #4b5563;
    --destructive: #ef4444;
    --destructive-foreground: #ffffff;
    --border: #e5e7eb;
    --input: #e5e7eb;
    --ring: #9ca3af;
    --radius: 0.625rem;
  }

  .dark {
    --background: #1f2937;
    --foreground: #f9fafb;
    --card: #1f2937;
    --card-foreground: #f9fafb;
    --popover: #1f2937;
    --popover-foreground: #f9fafb;
    --primary: #f9fafb;
    --primary-foreground: #4b5563;
    --secondary: #374151;
    --secondary-foreground: #f9fafb;
    --muted: #374151;
    --muted-foreground: #9ca3af;
    --accent: #374151;
    --accent-foreground: #f9fafb;
    --destructive: #7f1d1d;
    --destructive-foreground: #f9fafb;
    --border: #374151;
    --input: #374151;
    --ring: #6b7280;
  }
}

@layer base {
  * {
    @apply border-border;
  }
  body {
    @apply bg-background text-foreground;
  }
}
EOF

# Créer un fichier tailwind.config.js compatible avec v3
cat > tailwind.config.js << 'EOF'
/** @type {import('tailwindcss').Config} */
module.exports = {
  darkMode: 'class',
  content: [
    './components/**/*.{js,vue,ts}',
    './layouts/**/*.vue',
    './pages/**/*.vue',
    './plugins/**/*.{js,ts}',
    './app.vue'
  ],
  theme: {
    extend: {
      colors: {
        border: 'var(--border)',
        input: 'var(--input)',
        ring: 'var(--ring)',
        background: 'var(--background)',
        foreground: 'var(--foreground)',
        primary: {
          DEFAULT: 'var(--primary)',
          foreground: 'var(--primary-foreground)'
        },
        secondary: {
          DEFAULT: 'var(--secondary)',
          foreground: 'var(--secondary-foreground)'
        },
        destructive: {
          DEFAULT: 'var(--destructive)',
          foreground: 'var(--destructive-foreground)'
        },
        muted: {
          DEFAULT: 'var(--muted)',
          foreground: 'var(--muted-foreground)'
        },
        accent: {
          DEFAULT: 'var(--accent)',
          foreground: 'var(--accent-foreground)'
        },
        popover: {
          DEFAULT: 'var(--popover)',
          foreground: 'var(--popover-foreground)'
        },
        card: {
          DEFAULT: 'var(--card)',
          foreground: 'var(--card-foreground)'
        }
      },
      borderRadius: {
        lg: 'var(--radius)',
        md: 'calc(var(--radius) - 2px)',
        sm: 'calc(var(--radius) - 4px)'
      }
    }
  },
  plugins: []
}
EOF

# Créer le fichier utils.ts pour shadcn
mkdir -p lib
cat > lib/utils.ts << 'EOF'
import { type ClassValue, clsx } from "clsx"
import { twMerge } from "tailwind-merge"

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}
EOF

# Supprimer les fichiers de configuration externes pour éviter les conflits avec Nuxt
rm -f postcss.config.js
rm -f postcss.config.ts
rm -f postcss.config.cjs
rm -f vite.config.js
rm -f vite.config.ts
rm -f vite.config.cjs
rm -f tailwind.config.cjs
rm -f tailwind.config.ts

# S'assurer que les dossiers nécessaires sont accessibles en écriture
mkdir -p /app/.nuxt /app/.output
chmod -R 777 /app/.nuxt /app/.output

# Nettoyer le cache et les dépendances
npm cache clean --force

# Exécuter la commande spécifiée
exec "$@" 