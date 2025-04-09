export default defineNuxtRouteMiddleware(async (to) => {
    // Ne pas vérifier l'authentification sur la page de challenge
    if (to.path === '/challenge') {
        return;
    }

    const sessionId = localStorage.getItem('sessionId');
    
    if (!sessionId) {
        return navigateTo('/challenge');
    }

    try {
        const response = await fetch(`${useRuntimeConfig().public.apiUrl}/session/check`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ sessionId })
        });

        if (!response.ok) {
            localStorage.removeItem('sessionId');
            return navigateTo('/challenge');
        }
    } catch (error) {
        console.error('Erreur lors de la vérification de la session:', error);
        return navigateTo('/challenge');
    }
}); 