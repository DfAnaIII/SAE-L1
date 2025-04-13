<template>
  <div class="flex flex-col min-h-screen">
    <!-- Header fixe avec effet glassmorphism -->
    <div class="header-glass fixed top-0 left-0 w-full flex flex-row items-center justify-between px-6 py-2 z-50">
      <!-- Élément vide pour équilibrer la flexbox -->
      <div class="w-[150px]"></div>
      
      <!-- Titre centré -->
      <div class="flex text-center tracking-tighter text-2xl font-bold py-4">
        <h1>SAE - Projet :<br> Ana D'erfurth et SILVA Florian</h1>
      </div>
      
      <!-- Boutons à droite -->
      <div class="flex items-center gap-4 w-[180px] justify-end pr-6">
        <slot name="header-buttons">
          <NuxtLink to="/" class="glass-button px-4 py-2 rounded-md text-sm transition-colors">
            Menu Principal
          </NuxtLink>
          <NuxtLink to="/documentation" class="glass-button px-4 py-2 rounded-md text-sm transition-colors">
            Documentation
          </NuxtLink>
          <button @click="toggleTheme" class="glass-button p-2 rounded-md">
            <Icon name="mdi:theme-light-dark" />
          </button>
        </slot>
      </div>
    </div>

    <!-- Espace pour éviter que le contenu soit caché sous le header fixe -->
    <div class="mt-24"></div>

    <!-- Contenu principal -->
    <main class="flex-1">
      <slot />
    </main>
  </div>
</template>

<script setup lang="ts">
import { onMounted } from 'vue';

function toggleTheme() {
  const theme = document.documentElement.getAttribute('class');
  document.documentElement.setAttribute('class', theme === 'dark' ? 'light' : 'dark');
}

// Initialiser le thème au chargement de la page
onMounted(() => {
  // Vérifier les préférences utilisateur ou utiliser le thème système
  const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
  const savedTheme = localStorage.getItem('theme');
  
  if (savedTheme) {
    document.documentElement.setAttribute('class', savedTheme);
  } else {
    document.documentElement.setAttribute('class', prefersDark ? 'dark' : 'light');
  }
});
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=DM+Sans:opsz,wght@9..40,400;9..40,500;9..40,700&display=swap');

* {
  font-family: 'DM Sans', sans-serif;
}

.header-glass {
  backdrop-filter: blur(10px);
  background-color: rgba(255, 255, 255, 0.1);
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.05);
}

:deep(.dark) .header-glass {
  background-color: rgba(0, 0, 0, 0.2);
  border-bottom: 1px solid rgba(255, 255, 255, 0.05);
}

.glass-button {
  backdrop-filter: blur(5px);
  transition: all 0.3s ease;
}

.glass-button:hover {
  background-color: rgba(255, 255, 255, 0.2);
}

:deep(.dark) .glass-button:hover {
  background-color: rgba(255, 255, 255, 0.1);
}
</style> 