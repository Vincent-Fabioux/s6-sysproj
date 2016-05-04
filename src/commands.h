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



void addInput(char *commande);


/**
 * Lit une commande dans le terminal et l'exécute.
 * \return 1 = continuer et 0 = arrêter la saisie de commandes
 */
int readCommand();


/**
 * Execute une commande donnée.
 * \param command Tableau contenant la commande et les arguments
 * \return 1 = succès et 0 = échec
 */
int executeCommand(char** command);


/**
 * Sépare une chaîne de caractère avec des espaces en plusieurs
 * autres chaînes.
 * \param string Chaîne de caractère à séparer
 * \return Pointeur vers le tableau de chaîne de caractères
 */
char** parseString(char* string);



/**
 * Libère la mémoire allouée pour un tableau de chaîne de caractères.
 * \param table	Tableau de chaînes de caractères
 */
void freeStringTable(char** table);


#endif
