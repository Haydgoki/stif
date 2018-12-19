Malamelli Mehdi - TIIR 2018/2019

# Projet STIF

Ce projet consiste à implémenter un paser d'image de format STIF. Le but est de gérer tous les cas d'erreur possible

## Instructions

#### Compilation

Afin de compiler les test et le projet il faut se mettre à la racine et taper la commande : `make`

#### Execution

Pour lancer le parser d'images, il faut se mettre à la racine et exécuter la commande : `./bin/spcheck`

#### Tests


Pour lancer le parser d'images, il faut se mettre à la racine et exécuter la commande : `./bin/check_main`

## Solutions mise en place

Pour chacune des situations menant à un crash, j'ai free les datas déjà alloués, et je retourne ensuite NULL, afin que les fonctions appellantes puissent à leur tour gérer les erreurs.

Afin de protéger mon code, j'ai tout d'abord fait des checks sur :
* les buffers passés en paramètre afin de voir si ils n'étaient pas NULL
* sur les buffers size. En effet il faut voir si il y a assez de data pour que le buffer puisse comporter soit un block soit une image etc ...
*

## Outils utilisés

## Conception


## Améliorations pour le projet.

### Améliorations possible pour le projet

* mettre des types unsigned pour les size dans les struct
* Retourner des codes d'erreur dans les ft au lieu d'une structure. Et mettre en param un pointeur vers la structure que l'on veut ou inversement

### Améliorations personnelles :
