// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  devtools: { enabled: true },
  modules: [
    '@nuxtjs/tailwindcss',
    'shadcn-vue',
    '@nuxt/fonts',
    '@nuxt/icon',
    '@nuxt/scripts'
  ],
  
  tailwindcss: {
    exposeConfig: true,
    viewer: true
  },

  runtimeConfig: {
    public: {
      apiUrl: process.env.API_URL || 'http://localhost:4040',
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
  },

  components: {
    dirs: [
      {
        path: '~/components',
        pathPrefix: false,
        global: true
      },
      {
        path: '~/components/ui',
        prefix: 'Ui',
        pathPrefix: false
      }
    ]
  },

  build: {
    transpile: ['vue-sonner']
  }
});