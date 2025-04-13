# Documentation du Projet SAE - GPS

Ce document explique en détail chaque partie du projet, comment les exécuter, les solutions implémentées ainsi que les problèmes rencontrés et leurs résolutions.

## Table des matières

1. [Introduction](#introduction)
2. [Partie 1 - Parsing et structures de données](#partie-1---parsing-et-structures-de-données)
3. [Partie 2 - Implémentation de l'algorithme GPS](#partie-2---implémentation-de-lalgorithme-gps)
4. [Partie 3 - Optimisations et améliorations](#partie-3---optimisations-et-améliorations)
5. [Partie 4 - Interface utilisateur avancée](#partie-4---interface-utilisateur-avancée)
6. [Partie Bonus](#partie-bonus)
7. [Problèmes rencontrés et solutions](#problèmes-rencontrés-et-solutions)

## Introduction

Ce projet implémente un système GPS (General Problem Solver) en langage C. Le système est capable de résoudre des problèmes en utilisant une approche de planification basée sur des états, des actions et des règles.

## Partie 1 - Parsing et structures de données

### Description
Cette partie établit les fondations du système GPS avec les structures de données essentielles et le parsing des fichiers de problèmes.

### Comment exécuter
```bash
cd data/Part_1
gcc -o gps_part1 main.c
./gps_part1
```

### Structures implémentées
- `State` : Représente un état avec une liste de faits
- `Action` : Définit une action avec un nom, des préconditions, des ajouts et des suppressions

### Fonctionnalités
- Parsing de fichier au format spécifié
- Chargement des états initiaux, des buts et des actions
- Affichage des données chargées

## Partie 2 - Implémentation de l'algorithme GPS

### Description
Cette partie implémente l'algorithme GPS complet avec une recherche en largeur (BFS) pour trouver un chemin de solution.

### Comment exécuter
```bash
cd data/Part_2
gcc -o gps_part2 main-solved.c
./gps_part2
```

### Solution implémentée
Le fichier `main-solved.c` contient:
- Une implémentation de recherche en largeur (BFS) qui explore systématiquement tous les états possibles
- Des structures de données optimisées pour représenter les règles et les états
- Un système de suivi des états visités pour éviter les boucles infinies
- Une reconstruction du chemin de solution
- Une interface utilisateur pour charger et analyser différents fichiers de problèmes

### Algorithme principal
L'algorithme de recherche de solution (fonction `rechercheSolution`) :
1. Utilise une file d'attente pour explorer les états en largeur d'abord
2. Pour chaque état, vérifie s'il atteint les buts
3. Pour chaque règle applicable, génère un nouvel état
4. Évite les états déjà visités
5. Retourne l'indice de l'état solution si trouvé, sinon -1

### Fonctionnalités 
- Chargement de fichiers au format spécifié
- Analyse automatique des règles, préconditions et effets
- Affichage de la solution et du chemin d'actions
- Menu utilisateur interactif

## Partie 3 - Optimisations et améliorations

### Description
Cette partie introduit des optimisations de performance et des améliorations de l'algorithme.

### Comment exécuter
```bash
cd data/Part_3
gcc -o gps_part3 main.c
./gps_part3
```

### Améliorations implémentées
- Optimisation de la recherche de faits dans les états
- Gestion de mémoire améliorée
- Interface utilisateur enrichie
- Documentation et commentaires enrichis

## Partie 4 - Interface utilisateur avancée

### Description
Cette partie ajoute une interface utilisateur plus élaborée et des fonctionnalités supplémentaires.

### Comment exécuter
```bash
cd data/Part_4
gcc -o gps_part4 main.c
./gps_part4
```

### Fonctionnalités ajoutées
- Interface utilisateur améliorée avec des menus interactifs
- Gestion des fichiers de problèmes personnalisés
- Statistiques sur les solutions trouvées
- Visualisation du chemin de solution

## Partie Bonus

### Description
La partie bonus contient des extensions et des expérimentations supplémentaires.

### Comment exécuter
```bash
cd data/Part_Bonus
gcc -o gps_bonus main.c
./gps_bonus
```

## Problèmes rencontrés et solutions

### Problème 1: Gestion de mémoire
**Problème** : Lors de l'implémentation de l'algorithme BFS, nous avons rencontré des fuites de mémoire et des dépassements de tableau.

**Solution** : 
- Définition de tailles maximales pour les tableaux (MAX_ETATS, MAX_MOTS, etc.)
- Vérification systématique des limites avant d'ajouter des éléments
- Libération appropriée des ressources

### Problème 2: Détection des états identiques
**Problème** : L'algorithme entrait dans des boucles infinies en ne reconnaissant pas les états déjà visités.

**Solution** :
- Implémentation de la fonction `etatsIdentiques` qui compare correctement deux états
- Vérification systématique des états déjà visités avant d'ajouter un nouvel état à la file

### Problème 3: Parsing des fichiers d'entrée
**Problème** : Le format des fichiers d'entrée était parfois inconsistant, ce qui causait des erreurs de parsing.

**Solution** :
- Implémentation d'une fonction `diviserChaine` robuste qui gère les espaces et les délimiteurs
- Validation des données d'entrée
- Gestion des erreurs améliorée

### Problème 4: Performance de l'algorithme
**Problème** : L'algorithme devenait très lent pour des problèmes complexes.

**Solution** :
- Optimisation de la fonction de recherche en évitant les calculs redondants
- Utilisation de structures de données efficaces
- Ajout d'une limitation du nombre d'états explorés pour éviter l'explosion combinatoire 