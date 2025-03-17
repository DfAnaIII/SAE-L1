// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2024-11-01',
  devtools: { enabled: true },

  modules: [
    '@nuxtjs/tailwindcss',
    '@nuxt/fonts',
    '@nuxt/icon',
    '@nuxt/scripts',
    '@nuxt/test-utils',
    '@nuxt/eslint',
    '@nuxt/content',
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
    }

})