<template>
    <div class="flex flex-col items-center min-h-screen">
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
                <button @click="toggleTheme" class="glass-button p-2 rounded-md">
                    <Icon name="mdi:theme-light-dark" />
                </button>
                <Button as-child variant="ghost" class="glass-button">
                    <a href="https://github.com/DfAnaIII/SAE-L1" target="_blank">
                        <Icon name="mdi:github" />
                    </a> 
                </Button>
            </div>
        </div>

        <!-- Espace pour éviter que le contenu soit caché sous le header fixe -->
        <div class="mt-32"></div>

        <!-- Contenu de la page de challenge -->
        <div class="container mx-auto px-4 pb-16 max-w-2xl">
            <Card class="mb-8">
                <CardHeader>
                    <CardTitle class="text-3xl font-bold text-center">
                        Vérification de Sécurité
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <div class="flex flex-col items-center justify-center space-y-6">
                        <div class="w-full h-[400px] bg-gray-100 dark:bg-gray-800 rounded-lg flex items-center justify-center">
                            <TurnstileWidget @verify="handleVerify" />
                        </div>
                        <p class="text-center text-gray-600 dark:text-gray-400">
                            Veuillez compléter le challenge de sécurité pour accéder à l'application.
                        </p>
                        <div v-if="debugMode" class="debug-panel p-4 bg-gray-100 dark:bg-gray-800 rounded-lg w-full">
                            <h3 class="font-bold mb-2">Informations de débogage:</h3>
                            <p>Clé Turnstile: {{ config.public.turnstileSiteKey || 'Non définie' }}</p>
                            <p>API URL: {{ config.public.apiUrl || 'Non définie' }}</p>
                            <p>Status: {{ debugStatus }}</p>
                        </div>
                    </div>
                </CardContent>
            </Card>
        </div>
    </div>
</template>

<script setup lang="ts">
import { onBeforeMount, ref } from 'vue'
import { useRouter } from 'vue-router'

// Ne pas appliquer le middleware d'authentification à cette page
definePageMeta({
    middleware: []
})

const router = useRouter()
const config = useRuntimeConfig()
const debugMode = ref(true)
const debugStatus = ref('En attente de vérification')

console.log('Page challenge initialisée')
console.log('Configuration:', {
    apiUrl: config.public.apiUrl,
    turnstileSiteKey: config.public.turnstileSiteKey
})

// Vérification de la session au chargement de la page
onBeforeMount(async () => {
    console.log('onBeforeMount appelé')
    
    if (process.client) {
        console.log('Exécution côté client')
        const sessionId = localStorage.getItem('sessionId')
        if (sessionId) {
            console.log('Session ID trouvé:', sessionId)
            try {
                debugStatus.value = 'Vérification de session en cours...'
                const response = await fetch(`${config.public.apiUrl}/session/check`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ sessionId })
                })
                
                if (response.ok) {
                    console.log('Session valide, redirection vers l\'accueil')
                    debugStatus.value = 'Session valide, redirection...'
                    router.push('/')
                } else {
                    console.log('Session invalide, suppression de la session')
                    debugStatus.value = 'Session invalide'
                    // Si la session est invalide, on la supprime
                    localStorage.removeItem('sessionId')
                }
            } catch (error) {
                console.error('Erreur lors de la vérification de la session:', error)
                debugStatus.value = `Erreur: ${error.message}`
            }
        } else {
            console.log('Aucune session trouvée')
            debugStatus.value = 'Aucune session trouvée'
        }
    }
})

const handleVerify = async (token: string) => {
    console.log('Token Turnstile reçu:', token.substring(0, 20) + '...')
    debugStatus.value = 'Token reçu, validation en cours...'
    
    try {
        const response = await fetch(`${config.public.apiUrl}/turnstile/validate`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ token })
        })
        
        if (response.ok) {
            const data = await response.json()
            console.log('Validation réussie, session créée:', data)
            debugStatus.value = 'Validation réussie'
            localStorage.setItem('sessionId', data.sessionId)
            router.push('/')
        } else {
            const error = await response.json()
            console.error('Échec de la validation:', error)
            debugStatus.value = `Échec: ${error.error || 'Erreur de validation'}`
        }
    } catch (error) {
        console.error('Erreur lors de la vérification du challenge:', error)
        debugStatus.value = `Erreur: ${error.message}`
    }
}

function toggleTheme() {
    const theme = document.documentElement.getAttribute('class')
    document.documentElement.setAttribute('class', theme === 'dark' ? 'light' : 'dark')
}
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=DM+Sans:opsz,wght@9..40,400;9..40,500;9..40,700&display=swap')

* {
  font-family: 'DM Sans', sans-serif
}

.header-glass {
  backdrop-filter: blur(10px)
  background-color: rgba(255, 255, 255, 0.1)
  border-bottom: 1px solid rgba(255, 255, 255, 0.1)
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.05)
}

:deep(.dark) .header-glass {
  background-color: rgba(0, 0, 0, 0.2)
  border-bottom: 1px solid rgba(255, 255, 255, 0.05)
}

.glass-button {
  backdrop-filter: blur(5px)
  transition: all 0.3s ease
}

.glass-button:hover {
  background-color: rgba(255, 255, 255, 0.2)
}

:deep(.dark) .glass-button:hover {
  background-color: rgba(255, 255, 255, 0.1)
}

.debug-panel {
  font-size: 0.8rem
  border: 1px solid rgba(255, 0, 0, 0.2)
}
</style> 