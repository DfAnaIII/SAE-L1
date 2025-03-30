<template>
  <div class="flex flex-col items-center h-screen justify-center">
    <div class="w-full flex flex-row items-center justify-between px-6">
      <!-- Élément vide pour équilibrer la flexbox -->
      <div class="w-[150px]"></div>

      <!-- Titre centré -->
      <div class="flex text-center tracking-tighter text-2xl font-bold py-8">
        <h1>SAE - Projet :<br> A    na D'erfurth et SILVA Florian</h1>
      </div>

      <!-- Boutons à droite -->
      <div class="flex items-center gap-4 w-[150px] justify-end pr-14">
        <TooltipProvider>
          <Tooltip>
            <TooltipTrigger as-child>
              <Button as-child>
                <a href="/documentation">Documentation</a>
              </Button>
            </TooltipTrigger>
            <TooltipContent side="bottom">
              <p>Accéder à la documentation</p>
            </TooltipContent>
          </Tooltip>
        </TooltipProvider>

        <TooltipProvider>
          <Tooltip>
            <TooltipTrigger as-child>
              <Button as-child>
                <a href="https://github.com/DfAnaIII/SAE-L1">
                  <Icon name="mdi:github" />
                </a>
              </Button>
            </TooltipTrigger>
            <TooltipContent side="bottom">
              <p>Voir le code source sur GitHub</p>
            </TooltipContent>
          </Tooltip>
        </TooltipProvider>

        <TooltipProvider>
          <Tooltip>
            <TooltipTrigger as-child>
              <Button @click="toggleTheme">
                <Icon name="mdi:theme-light-dark" />
              </Button>
            </TooltipTrigger>
            <TooltipContent side="bottom">
              <p>Changer le thème</p>
            </TooltipContent>
          </Tooltip>
        </TooltipProvider>
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
            <Textarea v-model="textareaContent" placeholder="Collez ou écrivez votre fichier ici..." class="w-full h-full max-h-full min-h-full" />
          </CardContent>
          <CardFooter>
            <div class="flex flex-row items-center justify-center gap-2 w-full">
              <TooltipProvider>
                <Tooltip>
                  <TooltipTrigger as-child>
                    <Button :class="buttonClass" @click="sendContentTest">
                      Vérifier
                    </Button>
                  </TooltipTrigger>
                  <TooltipContent side="bottom">
                    <p>Vérifie le fichier</p>
                  </TooltipContent>
                </Tooltip>
              </TooltipProvider>

              <TooltipProvider>
                <Tooltip>
                  <TooltipTrigger as-child>
                    <Button class="w-full" @click="sendContent">
                      Envoyer
                    </Button>
                  </TooltipTrigger>
                  <TooltipContent side="bottom">
                    <p>Envoyer le contenu</p>
                  </TooltipContent>
                </Tooltip>
              </TooltipProvider>
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
            <TooltipProvider>
              <Tooltip>
                <TooltipTrigger as-child>
                  <Button class="w-full">
                    Rafraîchir
                  </Button>
                </TooltipTrigger>
                <TooltipContent side="bottom">
                  <p>Rafraîchir le contenu</p>
                </TooltipContent>
              </Tooltip>
            </TooltipProvider>
          </CardFooter>
        </Card>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import { Textarea } from '@/components/ui/textarea';
import { Button } from '@/components/ui/button';
import { Card, CardHeader, CardTitle, CardContent, CardFooter } from '@/components/ui/card';
import { Skeleton } from '@/components/ui/skeleton';
import { Separator } from '@/components/ui/separator';
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from '@/components/ui/tooltip';

const textareaContent = ref('');
const buttonClass = ref('bg-green-500 hover:bg-green-600');

async function sendContentTest() {
  try {
    const response = await fetch('https://localhost:4000/test', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ content: textareaContent.value }),
    });

    if (response.ok) {
      const data = await response.json();
      if (data.status === 'Correct') {
        buttonClass.value = 'bg-green-500 hover:bg-green-600';
      } else {
        buttonClass.value = 'bg-red-500 hover:bg-red-600';
      }
    } else {
      buttonClass.value = 'bg-orange-500 hover:bg-orange-600';
    }
    } catch (error) {
      buttonClass.value = 'bg-orange-500 hover:bg-orange-600';
    }
  }

async function sendContent() {
  try {
    const response = await fetch('https://localhost:4000/content', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ content: textareaContent.value }),
    });

    if (response.ok) {
      const data = await response.json();
      textareaContent.value = "";
    }
  } catch (error) {
    console.error('Error sending content:', error);
  }
}

function toggleTheme() {
  const theme = document.documentElement.getAttribute('class');
  document.documentElement.setAttribute('class', theme === 'dark' ? 'light' : 'dark');
}

import { onMounted } from 'vue';

onMounted(() => {
  const TextAreaContent = document.querySelector('textarea');
  if (TextAreaContent) {
    TextAreaContent.addEventListener('input', function() {
      const text = TextAreaContent.value;
      const lines = text.split('\n');
      const lineCount = lines.length;
      const charCount = text.length;
      console.log('Nombre de lignes : ' + lineCount);
      console.log('Nombre de caractères : ' + charCount);
    });
  }
});
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=DM+Sans:opsz,wght@9..40,400;9..40,500;9..40,700&display=swap');

* {
  font-family: 'DM Sans', sans-serif;
}
</style>
