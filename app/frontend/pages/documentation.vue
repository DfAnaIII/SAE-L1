<template>
    <NuxtLayout>
        <div class="container mx-auto px-4 pb-16 max-w-5xl">
            <Card class="mb-8">
                <CardHeader>
                    <CardTitle class="text-3xl font-bold text-center">
                        Documentation GPS (General Problem Solver)
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="text-lg mb-4">
                        Le GPS (General Problem Solver) est un système de résolution de problèmes basé sur des règles.
                        Il utilise un ensemble de faits, de règles et d'objectifs pour trouver une séquence d'actions permettant
                        d'atteindre un état objectif à partir d'un état initial.
                    </p>
                    <p class="text-lg">
                        Ce projet a été développé en plusieurs parties, chacune implémentant des fonctionnalités 
                        supplémentaires pour améliorer les capacités du système.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie 1 -->
            <Card class="mb-8" id="part1">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie 1: Chargement et Représentation des Données
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie concerne la représentation et le chargement des données nécessaires au fonctionnement du GPS:
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Définition de la structure <code>Regle</code> qui représente une règle avec son action, ses préconditions, les éléments à ajouter et à supprimer.</li>
                        <li>Implémentation de la fonction <code>parseLine</code> pour analyser les lignes de texte au format "mot:mot1,mot2,...".</li>
                        <li>Création des fonctions <code>chargerRegles</code> et <code>chargerFaits</code> pour lire les fichiers contenant les règles, faits initiaux et buts.</li>
                    </ul>
                    <p>
                        À ce stade, le système peut uniquement charger les données sans encore effectuer de résolution de problèmes.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie 2 -->
            <Card class="mb-8" id="part2">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie 2: Recherche en Avant (Forward Chaining)
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie implémente l'algorithme de recherche en avant:
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Définition de la structure <code>Etat</code> qui représente un état du système avec ses faits et son historique.</li>
                        <li>Implémentation des fonctions <code>contientFait</code> et <code>preconditionsSatisfaites</code> pour vérifier si un état satisfait les conditions d'application d'une règle.</li>
                        <li>Création de la fonction <code>appliquerRegle</code> qui transforme un état en appliquant une règle.</li>
                        <li>Implémentation de <code>rechercheSolution</code> qui explore les états possibles pour trouver une solution.</li>
                        <li>Ajout de la fonction <code>afficherSolution</code> pour présenter à l'utilisateur la séquence d'actions à effectuer.</li>
                    </ul>
                    <p>
                        Le système peut maintenant trouver une solution en appliquant successivement des règles et en vérifiant si les objectifs sont atteints.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie 3 -->
            <Card class="mb-8" id="part3">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie 3: Backtracking
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie améliore l'algorithme de recherche avec une approche par backtracking:
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Evolution de la structure <code>Etat</code> pour inclure la mémoire du chemin parcouru.</li>
                        <li>Implémentation de <code>rechercheSolutionBacktrack</code> qui utilise le backtracking pour explorer l'espace des solutions.</li>
                        <li>Lorsqu'un chemin ne mène pas à une solution, l'algorithme revient en arrière ("backtrack") et essaie une autre règle.</li>
                        <li>Amélioration de l'affichage de la solution pour montrer le chemin complet.</li>
                    </ul>
                    <p>
                        Cette approche est plus robuste et complète, permettant d'explorer systématiquement tous les chemins possibles jusqu'à trouver une solution.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie 4 -->
            <Card class="mb-8" id="part4">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie 4: Recherche en Largeur (BFS)
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie remplace le backtracking par un algorithme de recherche en largeur (BFS):
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Refactorisation des structures <code>State</code>, <code>Goal</code>, <code>Action</code> et <code>Node</code> pour une meilleure modularité.</li>
                        <li>Implémentation de <code>bfs</code> qui explore tous les états possibles par niveaux de profondeur.</li>
                        <li>Vérification de la présence d'états déjà visités pour éviter les cycles.</li>
                        <li>Reconstruction du plan d'actions à partir de l'état solution.</li>
                    </ul>
                    <p>
                        L'algorithme BFS garantit de trouver la solution optimale (avec le moins d'étapes possible) si elle existe.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie Interface -->
            <Card class="mb-8" id="interface">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie Interface: Interface Utilisateur
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie améliore l'expérience utilisateur avec une interface en ligne de commande:
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Création d'un menu ASCII pour présenter les différentes options à l'utilisateur.</li>
                        <li>Possibilité d'utiliser un fichier de problème par défaut (monkeys.txt).</li>
                        <li>Option pour spécifier un fichier personnalisé contenant un problème différent.</li>
                        <li>Fonction pour créer un nouveau fichier de problème interactivement.</li>
                    </ul>
                    <p>
                        Cette interface rend le système plus accessible et permet aux utilisateurs de résoudre différents types de problèmes sans modifier le code.
                    </p>
                </CardContent>
            </Card>

            <!-- Partie Bonus -->
            <Card class="mb-8" id="bonus">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Partie Bonus: Fonctionnalités Étendues
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p class="mb-4">
                        Cette partie ajoute des fonctionnalités supplémentaires au système:
                    </p>
                    <ul class="list-disc pl-6 mb-4 space-y-2">
                        <li>Extension de la structure <code>Action</code> avec des préconditions négatives (<code>notpreconds</code>).</li>
                        <li>Mesure du temps d'exécution des algorithmes de résolution.</li>
                        <li>Possibilité d'exécuter le programme avec des arguments en ligne de commande.</li>
                        <li>Amélioration des options de personnalisation et de création de problèmes.</li>
                    </ul>
                    <p>
                        Ces fonctionnalités bonus rendent le système plus puissant et flexible pour différents types de problèmes.
                    </p>
                </CardContent>
            </Card>

            <!-- Conclusion -->
            <Card class="mb-8">
                <CardHeader>
                    <CardTitle class="text-2xl font-bold">
                        Conclusion
                    </CardTitle>
                </CardHeader>
                <CardContent>
                    <p>
                        Le projet GPS est une implémentation progressive d'un système de résolution de problèmes,
                        évoluant d'une simple représentation des données à un système complet avec différentes
                        stratégies de recherche (chaînage avant, backtracking, BFS) et une interface utilisateur conviviale.
                        Il illustre les concepts fondamentaux de l'intelligence artificielle symbolique et de la recherche
                        dans des espaces d'états.
                    </p>
                </CardContent>
            </Card>
        </div>
    </NuxtLayout>
</template>

<script setup lang="ts">
// Appliquer le middleware d'authentification à cette page
definePageMeta({
    middleware: ['auth']
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