<template>
  <Button variant="outline" size="icon" @click="toggleTheme" class="rounded-full">
    <SunIcon v-if="theme === 'dark'" class="h-[1.2rem] w-[1.2rem]" />
    <MoonIcon v-else class="h-[1.2rem] w-[1.2rem]" />
    <span class="sr-only">Toggle theme</span>
  </Button>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { Button } from '@/components/ui/button'
import { SunIcon, MoonIcon } from '@/components/ui/icons'

const theme = ref('light')

function toggleTheme() {
  theme.value = theme.value === 'light' ? 'dark' : 'light'
  updateTheme()
}

function updateTheme() {
  if (theme.value === 'dark') {
    document.documentElement.classList.add('dark')
  } else {
    document.documentElement.classList.remove('dark')
  }
  
  // Stocker le thème dans localStorage
  if (process.client) {
    localStorage.setItem('theme', theme.value)
  }
}

onMounted(() => {
  if (process.client) {
    // Récupérer le thème depuis localStorage ou utiliser les préférences du système
    const savedTheme = localStorage.getItem('theme')
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches
    
    theme.value = savedTheme || (prefersDark ? 'dark' : 'light')
    updateTheme()
  }
})
</script> 