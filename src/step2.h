#ifndef STEP2_H_INCLUDED
#define STEP2_H_INCLUDED

#include <dirent.h>
#include <errno.h>
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


#include "commands.h"



void commandHistory(char **command , char * pathFileHistory);

/**
 * Change le répertoire courant: commande "cd".
 * \param command Commande et arguments entrés
 */
void commandCd(char** command);


/**
 * Met à jour les dates de modification et d'accès d'un fichier.
 * Option -m: uniquement la date de modification.
 * \param command Commande et arguments entrés
 */
void commandTouch(char** command);


void commandCat(char** command);


void commandCopy(char **command);


/**
 * Copie un fichier vers une destination.
 * 
 * \param	source Chemin du fichier source
 * \param	dest Chemin du fichier destination
 * \return	1 = succès ou 0 = échec
 */
int copyFile(char source[], char dest[]);


/**
 * Créé un dossier vide vers une destination avec les permissions d'un
 * dossier source.
 * 
 * \param source Chemin du dossier source
 * \param dest Chemin du dossier destination
 */
void copyDir(char source[], char dest[]);


/**
 * Copie un dossier et son contenu vers un autre dossier.
 * 
 * \param	source Chemin du dossier source
 * \param	dest Chemin du dossier destination
 * \return	1 = succès ou 0 = échec
 */
int copyFromDir(char source[], char dest[]);


/**
 * Concatène un chemin et un nom de fichier. La mémoire liée au pointeur
 * retourné doit être libérée.
 * 
 * \param	path Chemin du fichier
 * \param	dest Nom du fichier
 * \return	Pointeur vers la chaîne résultante
 */
char* catDirFile(char path[], char file[]);

#endif
