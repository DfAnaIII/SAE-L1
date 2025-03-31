// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2024-11-01',
  devtools: { enabled: true },

  modules: [
    '@nuxtjs/tailwindcss',
    '@nuxt/eslint',
    'shadcn-nuxt'
  ],
  
  tailwindcss: {
    exposeConfig: true,
    viewer: true,
    // and more...
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

  runtimeConfig: {
    public: {
      apiUrl: process.env.API_URL || 'http://localhost:4000',
      cloudflareSiteKey: process.env.CLOUDFLARE_SITE_KEY
    }
  },

  app: {
    head: {
      script: [
        {
          src: 'https://challenges.cloudflare.com/turnstile/v0/api.js',
          async: true,
          defer: true
        }
      ]
    }
  },

  nitro: {
    routeRules: {
      '/challenge': { appMiddleware: { auth: false } },
      '/**': { appMiddleware: ['auth'] }
    }
  }
})