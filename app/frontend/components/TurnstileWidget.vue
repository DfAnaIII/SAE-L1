<template>
  <div class="turnstile-container">
    <div ref="turnstileRef"></div>
    <div v-if="debugInfo" class="debug-info">
      <p>Site Key: {{ config.public.turnstileSiteKey || 'Non définie' }}</p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue';

const emit = defineEmits<{
  (e: 'verify', token: string): void
}>();

const turnstileRef = ref<HTMLDivElement | null>(null);
const config = useRuntimeConfig();
const debugInfo = ref(process.dev);

onMounted(() => {
  console.log('TurnstileWidget monté avec la clé:', config.public.turnstileSiteKey);
  
  // Attendre que Turnstile soit chargé
  if (typeof window !== 'undefined') {
    if (!window.turnstile) {
      console.log('Chargement du script Turnstile...');
      const script = document.createElement('script');
      script.src = 'https://challenges.cloudflare.com/turnstile/v0/api.js?render=explicit';
      script.async = true;
      script.defer = true;
      document.head.appendChild(script);
      
      script.onload = () => {
        console.log('Script Turnstile chargé!');
        renderTurnstile();
      };
    } else {
      console.log('Turnstile est déjà chargé.');
      renderTurnstile();
    }
  }
});

function renderTurnstile() {
  if (!turnstileRef.value || !window.turnstile) {
    console.error('Impossible de rendre Turnstile: ', { 
      ref: !!turnstileRef.value, 
      turnstile: !!window.turnstile 
    });
    return;
  }
  
  console.log('Rendu du widget Turnstile avec la clé:', config.public.turnstileSiteKey);
  try {
    window.turnstile.render(turnstileRef.value, {
      sitekey: config.public.turnstileSiteKey,
      callback: (token: string) => {
        console.log('Turnstile validé avec succès, token reçu');
        emit('verify', token);
      },
      'refresh-expired': 'auto',
      theme: 'auto'
    });
  } catch (error) {
    console.error('Erreur lors du rendu Turnstile:', error);
  }
}
</script>

<style scoped>
.turnstile-container {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  min-height: 65px;
  width: 100%;
}

.debug-info {
  margin-top: 10px;
  padding: 10px;
  background-color: rgba(255, 0, 0, 0.1);
  border-radius: 4px;
  font-size: 12px;
}
</style>

<script lang="ts">
// Définir l'interface pour l'API Turnstile
declare global {
  interface Window {
    turnstile?: {
      render: (
        container: HTMLElement,
        options: {
          sitekey: string;
          callback: (token: string) => void;
          'refresh-expired': string;
          theme: string;
        }
      ) => void;
      reset: (widgetId: string) => void;
    };
  }
}
</script> 