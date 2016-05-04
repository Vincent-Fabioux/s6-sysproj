#ifndef STEP3_H_INCLUDED
#define STEP3_H_INCLUDED

#define DIRECTORY_MAX_LENGTH	1024

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
/**
 * Execute une commande donnée.
 * @param path contenant le resultat du PATH ,command Tableau contenant la premiere commande
 * @return chemin de la commande a executer , NULL si elle n'existe pas
 */
char *findPath(char *path,char *command);

#endif
