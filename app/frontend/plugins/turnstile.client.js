export default defineNuxtPlugin(() => {
    const config = useRuntimeConfig();
    
    return {
        provide: {
            turnstile: {
                render: (elementId, options) => {
                    if (typeof window !== 'undefined' && window.turnstile) {
                        return window.turnstile.render(elementId, {
                            sitekey: config.public.cloudflareSiteKey,
                            ...options
                        });
                    }
                }
            }
        }
    };
}); 