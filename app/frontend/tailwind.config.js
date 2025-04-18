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
