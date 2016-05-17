#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "step2.h"

#define ARG_MAX_LENGTH			128
#define ARG_MAX_NUMBER			128
#define COMMAND_MAX_LENGTH		1024


/**
 * Rajoute une commande saisie dans le fichier history.txt 
 * \param command a rajouter dans le fichier history
 * \param pathFileHistory contenant le chemin du fichier texte d'History
 */
void addInput(char *commande,char *pathFileHistory);


/**
 * Lit une commande dans le terminal
 * \param pathFileHistory contenant le chemin du fichier texte d'History
 * \return 1 = continuer et 0 = arrêter la saisie de commandes
 */
int readCommand(char * pathFileHistory);


/**
 * Execute une commande donnée.
 * \param command chaine contenant les commandes a executer
 * \param nb_pipes contenant le nombre de pipes  
 * \param nb_redirections contenant le nombre de redirections
 * \param positionRedireciton conteant les positions dans la chaine 
 * des redirections
 * \param pathFileHistory contenant le chemin du fichier texte d'history
 * \return 1 = succès et 0 = échec
 */
int executeCommand(char** command,int nb_pipes,int nb_redirections,int *positionRedirection ,char * pathFileHistory);

/**
 * Sépare une chaîne de caractère avec des espaces en plusieurs
 * autres chaînes.
 * \param string Chaîne de caractère à séparer
 * \return Pointeur vers le tableau de chaîne de caractères
 */
char** parseString(char* string);

/**
 * Sépare une chaîne de caractère avec des pipes et des redirections 
 * en plusieurs autres chaînes sans pipes ni redirections
 * \param string Chaîne de caractère à séparer
 * \param nb_pipes nombre de pipes dans la chaine 
 * \param nb_redirections nombre de redirections dans la chaine 
 * \param positionRedirection tableau d'entier contenant les positions 
 * des differentes redirections 
 * \return Pointeur vers le tableau de chaîne de caractères
 */
char ** parseStringPipesAndRedirections(char* string, int * nb_pipes,int *nb_redirections,int *positionRedirection);


/**
 * Libère la mémoire allouée pour un tableau de chaîne de caractères.
 * \param table	Tableau de chaînes de caractères
 */
void freeStringTable(char** table);


#endif
