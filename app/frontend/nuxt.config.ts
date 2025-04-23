// import tailwindcss from '@tailwindcss/vite'
// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2024-11-01',
  devtools: { enabled: true },

  modules: [
    // '@nuxt/content', // Temporairement désactivé pour éviter les problèmes avec better-sqlite3
    '@nuxt/fonts',
    '@nuxt/icon',
    '@nuxt/scripts',
    'shadcn-nuxt',
    '@nuxtjs/turnstile'
  ],
  
  // Configuration CSS intégrée
  css: ['~/assets/css/tailwind.css'],
  
  // Configuration PostCSS intégrée
  postcss: {
    plugins: {
      'postcss-import': {},
      'tailwindcss/nesting': {},
      tailwindcss: {},
      autoprefixer: {}
    }
  },
  
  // Configuration Vite intégrée
  vite: {
    server: {
      allowedHosts: ['localhost', 'sae.gofindr.fr']
    },
    css: {
      preprocessorOptions: {
        scss: {
          additionalData: '@use "@/assets/css/tailwind.css" as *;'
        }
      }
    }
  },
  
  shadcn: {
    /**
     * Prefix for all the imported component
     */
    prefix: '',
    /**
     * Directory that the component lives in.
     * @default "./components/ui"
     */
    componentDir: './components/ui'
  },
  
  turnstile: {
    siteKey: process.env.CLOUDFLARE_SITE_KEY
  },
  
  runtimeConfig: {
    turnstileSecretKey: process.env.CLOUDFLARE_SECRET_KEY,
    public: {
      apiUrl: process.env.API_URL || 'http://localhost:4000',
      turnstileSiteKey: process.env.CLOUDFLARE_SITE_KEY
    }
  }
})