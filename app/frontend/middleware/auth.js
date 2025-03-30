export default defineNuxtRouteMiddleware(async (to) => {
    const sessionId = localStorage.getItem('sessionId');
    
    // Si on est sur la page de challenge, on ne vérifie pas la session
    if (to.path === '/challenge') {
        return;
    }

    // Si pas de session, rediriger vers la page de challenge
    if (!sessionId) {
        return navigateTo('/challenge');
    }

    try {
        const response = await fetch(`${useRuntimeConfig().public.apiUrl}/check`, {
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
        localStorage.removeItem('sessionId');
        return navigateTo('/challenge');
    }
}); 