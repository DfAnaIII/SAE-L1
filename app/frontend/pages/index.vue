<template>

    <div class="flex flex-col items-center h-screen justify-center ">
        <div class="w-full flex flex-row items-center justify-between px-6">
            <!-- Élément vide pour équilibrer la flexbox -->
            <div class="w-[150px]"></div>
            
            <!-- Titre centré -->
            <div class="flex text-center tracking-tighter text-2xl font-bold py-8">
                 <h1>SAE - Projet :<br> Ana D'erfurth et SILVA Florian</h1>
            </div>
            
            <!-- Boutons à droite -->
            <div class="flex items-center gap-4 w-[150px] justify-end pr-14">
                <Button as-child>
                    <a href="/documentation">
                        Documentation
                    </a> 
                </Button>
                <Button as-child>
                    <a href="https://github.com/DfAnaIII/SAE-L1">
                        <Icon name="mdi:github" />
                    </a> 
                </Button>
                <Button @click="toggleTheme">
                    <Icon name="mdi:theme-light-dark" />
                </Button>
            </div>
        </div>

        <div class="flex flex-row items-center justify-between h-full w-full tracking-wide">
            <div class="flex flex-col items-center justify-center tracking-wide w-full h-full">
                <Card class="w-3/4 h-4/5">
                    <CardHeader>
                        <CardTitle class="text-center font-bold text-2xl tracking-tighter">
                            <h2>Input</h2>
                        </CardTitle>
                    </CardHeader>
                    <CardContent class="h-3/4">
                        <Textarea placeholder="Type your message here." class="w-full h-full
                        max-h-full min-h-full" />
                    </CardContent>
                    <CardFooter>
                        <div class="flex flex-row items-center justify-center gap-2 w-full">
                            <Button class="w-full">
                                Vérifier
                            </Button>
                            <Button class="w-full">
                                Envoyer
                            </Button>
                        </div>
                    </CardFooter>
                </Card>
            </div>

            <Separator label="Puis" orientation="vertical" class="my-2 h-1/2" />

            <div class="flex flex-col items-center justify-center h-full tracking-wide w-full">

                <Card class="w-3/4 h-4/5">
                    <CardHeader>
                        <CardTitle class="text-center font-bold text-2xl tracking-tighter">
                            <h2>Output</h2>
                        </CardTitle>
                    </CardHeader>
                    <CardContent class="h-3/4">
                        <div class="flex flex-col space-y-3">
                            <Skeleton class="h-[125px] w-full rounded-xl" />
                            <div class="space-y-2">
                            <Skeleton class="h-4 w-full" />
                            <Skeleton class="h-4 w-3/4" />
                            </div>
                        </div>
                    </CardContent>
                    <CardFooter class="flex flex-row items-center justify-center gap-2 w-full">
                        <Button class="w-full">
                            Rafraîchir
                        </Button>
                    </CardFooter>
                </Card>
            </div>
        </div>
    </div>
</template>


<script setup lang="ts">
import { Textarea } from '@/components/ui/textarea'
import { Button } from '@/components/ui/button'
import { Card, CardHeader, CardTitle, CardContent, CardFooter } from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { Separator } from '@/components/ui/separator'

function toggleTheme() {
    const theme = document.documentElement.getAttribute('class');
    document.documentElement.setAttribute('class', theme === 'dark' ? 'light' : 'dark');
}

</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=DM+Sans:opsz,wght@9..40,400;9..40,500;9..40,700&display=swap');

* {
  font-family: 'DM Sans', sans-serif;
}
</style>