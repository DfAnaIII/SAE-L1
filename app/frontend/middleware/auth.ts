export default defineNuxtRouteMiddleware(async (to) => {
  // Ignorer la page de challenge pour éviter une boucle de redirection
  if (to.path === '/challenge') {
    return;
  }

  // Vérifier si nous sommes côté client
  if (process.client) {
    const sessionId = localStorage.getItem('sessionId');
    
    // Si aucune session n'existe, rediriger vers la page de challenge
    if (!sessionId) {
      return navigateTo('/challenge');
    }
    
    // Vérifier la validité de la session
    try {
      const config = useRuntimeConfig();
      const response = await fetch(`${config.public.apiUrl}/session/check`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ sessionId }),
      });
      
      // Si la session n'est pas valide, supprimer l'ID de session et rediriger
      if (!response.ok) {
        localStorage.removeItem('sessionId');
        return navigateTo('/challenge');
      }
    } catch (error) {
      console.error('Erreur lors de la vérification de la session:', error);
      return navigateTo('/challenge');
    }
  }
}); 