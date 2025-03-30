export default defineNuxtPlugin(() => {
    return {
        provide: {
            localStorage: {
                getItem(key: string) {
                    if (process.client) {
                        return localStorage.getItem(key)
                    }
                    return null
                },
                setItem(key: string, value: string) {
                    if (process.client) {
                        localStorage.setItem(key, value)
                        return true
                    }
                    return false
                },
                removeItem(key: string) {
                    if (process.client) {
                        localStorage.removeItem(key)
                        return true
                    }
                    return false
                }
            }
        }
    }
}) 