<template>
  <NuxtPage />

  <ClientOnly>
    <Toaster :expand="false" position="top-center" richColors  />
  </ClientOnly>
</template>


<script setup>
import { Toaster } from '@/components/ui/sonner'
import { onMounted } from 'vue'

// Initialiser le thème au chargement de l'application
onMounted(() => {
  if (process.client) {
    const savedTheme = localStorage.getItem('theme')
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches
    
    if (savedTheme === 'dark' || (!savedTheme && prefersDark)) {
      document.documentElement.classList.add('dark')
    }
  }
})
</script>

<style>
body {
  transition: background-color 0.3s ease, color 0.3s ease;
}
</style>

