import tailwindcss from '@tailwindcss/vite'
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
  css: ['~/assets/css/tailwind.css'],
  postcss: {
    plugins: {
      tailwindcss: {},
      autoprefixer: {},
    },
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