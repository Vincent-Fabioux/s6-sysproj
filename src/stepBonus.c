#include "stepBonus.h"

void commandFind(char **command, char *path, char *dir)
{
	int search = 0;

	struct stat *type = malloc(sizeof(struct stat));
	char *tempSource = NULL;
	struct dirent *file = NULL;
	char *temp = NULL;
	DIR *dirSource = NULL;

	if(!command[1])
		search = 0;
	else if(!strcmp(command[1], ".") && !strcmp(command[2], "-name")
			&& command[3] != NULL)
		search = 1;

	dirSource = opendir(path);	/*Ouverture du repertoire ou du fichier source rentre en parametre */

	if(dirSource == NULL)
	{
		printf("Impossible d'ouvrir le repertoire \n ");
		return;
	}

	//Va servir pour transmettre le dossier precedent pour la recursivité de la fonction
	temp = calloc(DIRECTORY_MAX_LENGTH, sizeof(char));

	if(strcmp(dir, " "))
	{
		strcat(temp, dir);
	}

	while(1)
	{							/*On va lire chaque fichier ou repertoire qui se trouve dans le repertoire rentré en parametre ou uniquement un seul fichier */
		file = readdir(dirSource);
		if(file == NULL)
		{						/*S'il n'y pas ou plus de fichiers ou repertoires à lire */
			break;
		}
		else
		{
			if(file->d_name[0] != '.'
			   || (file->d_name[0] == '.' && file->d_name[1] != '.'
				   && file->d_name[1]))
			{
				concatenate(&tempSource, path, file->d_name);
				if(stat(tempSource, type) == 0)
				{				/*La fonction stat permet de connaitre quel type de fichiers il s'agit */
					if(S_ISREG(type->st_mode))
					{			/*S'il s'agit d'un fichier de type "courant" */
						//Si l'on a trouvé le fichier que l'on cherche ou que l'on veut afficher tous les fichiers 
						if((search == 1
							&& !strcmp(command[3], file->d_name))
						   || search == 0)
						{
							printf("./%s%s \n", temp, file->d_name);
							if(search==1)
								break;
						}
					}
					if(S_ISDIR(type->st_mode))
					{			/*S'il s'agit d'un repertoire */
						char newDir[DIRECTORY_MAX_LENGTH] = "";
						strcat(newDir, temp);
						strcat(newDir, file->d_name);
						strcat(newDir, "/");
						if(!search)
							printf("./%s \n", newDir);
						commandFind(command, tempSource, newDir);	/*On utilise la récursivité de la fonction pour copier ce qui se trouve a l'intérieur du répertoire */
					}
				}
				free(tempSource);
			}

		}
	}
	free(temp);

	if(closedir(dirSource))
	{							/*S'il le repertoire ou le fichier n'a pas reussi a se fermer */
		printf("Probleme de fermeture de dossier \n");
		return;
	}
}


void concatenate(char **temp, char source[], char file[])
{
	int longueur = 0, longueur2 = 0;
	while(source[longueur] != '\0')
	{							/*On va tout d'abord compter la longueur de la 1ere chaine de caracteres */
		longueur = longueur + 1;
	}
	while(file[longueur2] != '\0')
	{							/*Puis la longueur de la 2eme chaine de caracteres */
		longueur2 = longueur2 + 1;
	}
	if(source[longueur] != '/')
	{							/*Si le dossier rentre ne contient pas le '/' a la fin */
		*temp = calloc((longueur + longueur2 + 1), sizeof(char));	/*On alloue la memoire de la taille des deux chaines plus le caractere '/' puis on concatene les chaines */
		strncat(*temp, source, strlen(source));
		strncat(*temp, "/", 1);
		strncat(*temp, file, strlen(file));
	}
	else
	{
		*temp = calloc((longueur + longueur2), sizeof(char));	/*On alloue la memoire de la taille des deux chaines puis on concatene */
		strncat(*temp, source, strlen(source));
		strncat(*temp, file, strlen(file));
	}
}
