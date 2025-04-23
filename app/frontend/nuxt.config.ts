// import tailwindcss from '@tailwindcss/vite'
// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2024-11-01',
  devtools: { enabled: true },
  
  css: [
    '~/assets/css/tailwind.css'
  ],

  modules: [
    // '@nuxt/content', // Temporairement désactivé pour éviter les problèmes avec better-sqlite3
    '@nuxt/fonts',
    '@nuxt/icon',
    '@nuxt/scripts',
    'shadcn-nuxt',
    '@nuxtjs/tailwindcss',
    '@nuxtjs/color-mode',
    '@nuxtjs/turnstile'
  ],
  
  tailwindcss: {
    cssPath: '~/assets/css/tailwind.css',
    configPath: '~/tailwind.config.js',
    exposeConfig: false,
    viewer: true,
  },
  
  colorMode: {
    classSuffix: ''
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
  
  // Résolution des alias pour Shadcn
  alias: {
    utils: '~/lib/utils'
  },
  
  turnstile: {
    siteKey: process.env.CLOUDFLARE_SITE_KEY
  },
  
  // Configuration Vite
  vite: {
    server: {
      allowedHosts: ['localhost', 'sae.gofindr.fr']
    }
  },
  
  runtimeConfig: {
    turnstileSecretKey: process.env.CLOUDFLARE_SECRET_KEY,
    public: {
      apiUrl: process.env.API_URL || 'http://localhost:4000',
      turnstileSiteKey: process.env.CLOUDFLARE_SITE_KEY
    }
  },
  
  // Configuration de fetch/useFetch global
  typescript: {
    shim: false
  }
})