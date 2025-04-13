/** @type {import('tailwindcss').Config} */
export default {
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
        gray: {
          100: 'rgb(243 244 246)',
          200: 'rgb(229 231 235)',
          300: 'rgb(209 213 219)',
          400: 'rgb(156 163 175)',
          500: 'rgb(107 114 128)',
          600: 'rgb(75 85 99)',
          700: 'rgb(55 65 81)',
          800: 'rgb(31 41 55)',
          900: 'rgb(17 24 39)',
        }
      }
    }
  },
  plugins: []
} 