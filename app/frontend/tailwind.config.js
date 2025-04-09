/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./components/**/*.{js,vue,ts}",
    "./layouts/**/*.vue",
    "./pages/**/*.vue",
    "./plugins/**/*.{js,ts}",
    "./app.vue",
    "./error.vue"
  ],
  theme: {
    extend: {
      colors: {
        gray: {
          100: '#f3f4f6',
          800: '#1f2937'
        }
      }
    }
  },
  plugins: [],
  darkMode: 'class'
} 