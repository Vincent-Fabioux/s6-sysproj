#ifndef STEPBONUS_H_INCLUDED
#define STEPBONUS_H_INCLUDED


#define DIRECTORY_MAX_LENGTH	1024
#define ARG_MAX_LENGTH			128
#define ARG_MAX_NUMBER			128
#define COMMAND_MAX_LENGTH		1024

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


/**
 * Recherche un fichier ou affiche tous les fichiers
 * 
 * \param	command commande find avec ses arguments
 * \param	directory repertoire dans lequel on se toruve 
 * \param	dir repertoire precedent
 */
void commandFind(char **command,char * directory,char *dir);

/**
 * Concatene le chemin avec le nouveau dossier ou fichier 
 * 
 * \param	temp resultat de la concatenation
 * \param	source dossier actuel
 * \return	1file fichier ou dossier a rajouter
 */
void concatenate(char **temp,char source[],char file[]);

#endif
