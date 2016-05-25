#include "step2.h"

#define BUFFER_LENGHT	100



void commandHistory(char **command, char * pathFileHistory){
	FILE * fp;//Fichier history.txt contenant toutes les commandes deja tapées 
    char * line = NULL;//Buffer contenant les lignes qu'on va lire au fur et a mesure.
    size_t len = 0;
    ssize_t read;
    int i=0 ; //Increment du numero de ligne
    int n=0 ; //Nombres de lignes a afficher 
    int taille=0 ; //Nombre de lignes du fichier 
    int continuer=0;//Entier permettant de savoir si l'on veut executer une certaine commande
  
	//Si l'on rentre plus d'un argument history ne doit pas s'executer
    if(command[2]!=NULL){
		printf("Erreur: Commande non existante...\n");
		return;
	}
	//Si l'on veut afficher un nombre n de lignes ou executer une ligne n on regarde le 1er argument
    if(command[1]!=NULL){
		//Si le premier caractere est un ! on va le remplacer par un 0 pour pouvoir le convertir en int
		if(command[1][0]=='!'){
			command[1][0]='0';
			continuer=1;
		}
		n=atoi(command[1]);//On convertir le deuxieme argument		
		//Si le nombre saisi n'est pas valide
		if(n==0){
			return;
		}
	}
	//On ouvre le fichier contenant toutes les commandes saisies
    fp = fopen(pathFileHistory, "r");
    //S il y a eu une erreur lors de l'ouverture
    if (fp == NULL)
        exit(EXIT_FAILURE);
	//On va lire les lignes du fichiers pour connaitre sa taille
	while ((read = getline(&line, &len, fp)) != -1) {
		taille=taille+1;
		//Si l on voulait executer une certaine ligne on va le faire lors de l incrementation de la taille
		if(continuer==1){
			if(taille==n){
				int nb_pipes=0;
				int nb_redirections=0;
				int *positionRedirection=malloc(sizeof(int)*ARG_MAX_NUMBER-1);
				char ** command=parseStringPipesAndRedirections(line,&nb_pipes,&nb_redirections,positionRedirection);
				executeCommand(command,nb_pipes,nb_redirections,positionRedirection,pathFileHistory);
				if(command)
				{	
					int k = 0;
					while(command[k])
					{	
						command[k] = NULL;
						free(command[k]);
						k++;
					}
					free(command);
				}
				free(positionRedirection);
				fclose(fp);
				return;
			}
		}
	}
	//Si le nombre de lignes n que l'on voulait afficher ou la ligne qu'on voulait executer est plus grande que la taille du fichier  
	if (n>taille){
		printf("L'argument n'est pas valide : il doit etre compris entre 1 et %d \n",taille);
		fclose(fp);
		return;
	}
	//On remets le pointeur au debut du fichier
	rewind(fp);
	
	while ((read = getline(&line, &len, fp)) != -1) {
		i=i+1;//On incremente le numero de ligne 
		//Si l'on veut afficher toutes les commandes(juste history) ou si l'on veut afficher un nombre n de commandes(history n)
		if((n!=0&&i>(taille-n))||n==0){
			printf("%d : ", i);
			printf("%s", line);	
		}
    }
    
    free(line);
    fclose(fp);
}


void commandCd(char** command){
	if(command[2]==NULL) // La commande cd ne prend qu'un seul argument
	{
		if(chdir(command[1]))
		{
			printf("Erreur: aucun dossier portant le nom %s.\n", command[1]);
		}
	}
	else // Plus d'un argument: erreur
	{
		printf("Erreur de syntaxe: cd [dossier]\n");
	}
}



void commandTouch(char** command)
{
	if(!command[1] || strcmp(command[1], "") == 0)
	{
		printf("Erreur de syntaxe: touch [optionel: -m] [fichier]\n");
		return;
	}
	
	// Ouverture du fichier
	int file = open(command[1], O_WRONLY | O_CREAT);
	if(file == -1)
	{
		printf("Erreur: impossible d'ouvrir le fichier %s.\n", command[1]);
		return;
	}
	
	struct timespec buffer[2];
	buffer[1].tv_nsec = UTIME_NOW;
	if(command[2] && strcmp(command[2], "-m") == 0)
		buffer[0].tv_nsec = UTIME_OMIT;
	else
		buffer[0].tv_nsec = UTIME_NOW;
	futimens(file, buffer);
	
	close(file);
}


void commandCat(char **command){
	int i=1;	
	int j=0;						//Increment pour le nombre de lignes du fichier
	int taille;						//Nombre de fichiers a lire
	ssize_t read; 					//Permet de lire le nombre de lignes 
	FILE *fp;						//Fichiers a lire 
    size_t len = 0;					// Permet de lire le nombre de lignes
    int printLines=0;				//Entier pour savoir si on doit afficher ou non les lignes(-n)
    char * line = NULL;				//Buffer contenant les lignes qu'on va lire au fur et a mesure.    
	while(command[i]!=NULL){
		if(!strcmp(command[i],"-n"))
			printLines=1;
		else
			taille++;
		i++;
	}
	//On va lire tous les fichiers
	for(i=1;i<=taille;i++){
		fp = fopen(command[i], "r");
		if (fp == NULL)
			exit(EXIT_FAILURE);
		//On va lire ligne par ligne
        while ((read = getline(&line, &len, fp)) != -1) {
			j=j+1;
			//Si l'on a activé l'affichage des lignes
			if(printLines==1)
				printf("%d : ",j);
			printf("%s", line);	
		}
		line=NULL;
		fclose(fp);

	}
}


void commandCopy(char **command)
{
	if(!command[1] || !command[2]) // Erreur s'il manque l'un des deux paramètres
	{
		printf("Erreur de syntaxe: cp [source] [destination]\n");
		return;
	}
	
	// Vérification du type de source (fichier/dossier)
	struct stat statsBuffer;
	stat(command[1], &statsBuffer);
	
	if(S_ISREG(statsBuffer.st_mode)) // C'est un fichier
		copyFile(command[1], command[2]);
	else if(S_ISDIR(statsBuffer.st_mode)) // C'est un dossier
		copyFromDir(command[1], command[2]);
	else // Cela n'existe pas
		printf("Erreur: %s introuvable.\n", command[1]);
}


int copyFile(char source[], char dest[])
{
	int sourceFile, destFile;
	
	// Ouverture du fichier source
	sourceFile = open(source, O_RDONLY, O_NONBLOCK);
	if(sourceFile == -1)
	{
		printf("Impossible d'ouvrir le fichier \"%s\"!\n", source);
		return 0;
	}
	
	// Ouverture du fichier destination
	destFile = open(dest, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR);
	if(destFile == -1)
	{
		printf("Impossible d'ouvrir le fichier \"%s\"!\n", dest);
		close(sourceFile);
		return 0;
	}
	
	// Copie des droits d'accès de la source vers la destination
	struct stat statsBuffer;
	fstat(sourceFile, &statsBuffer);
	fchmod(destFile, statsBuffer.st_mode);
	
	// Copie du contenu de la source vers la destination
	char buffer[BUFFER_LENGHT];
	int bufferLength; // Nombre de caractères copiés
	while(1)
	{
		bufferLength = read(sourceFile, buffer, BUFFER_LENGHT);
		if(bufferLength == 0) // Plus rien à copier
			break;
		if(errno == EAGAIN) // Erreur de lecture
			continue;
			
		write(destFile, buffer, bufferLength);
		if(errno == EINTR) // Erreur d'écriture
			continue;
	}
	
	// Fermeture des fichiers ouverts
	close(sourceFile);
	close(destFile);
	return 1;
}


void copyDir(char source[], char dest[])
{
	// Récupération des attributs du dossier source
	struct stat statsBuffer;
	stat(source, &statsBuffer);
	
	// Création du dossier destination
	mkdir(dest, statsBuffer.st_mode);
}


int copyFromDir(char source[], char dest[])
{
	DIR *sourceDir = NULL, *destDir = NULL;
	
	// Ouverture du dossier source
	sourceDir = opendir(source);
	if(!sourceDir)
	{
		printf("Impossible de trouver le dossier \"%s\"!\n", source);
		return 0;
	}
	
	// Ouverture du dossier destination
	destDir = opendir(dest);
	if(!destDir)
	{
		printf("Impossible de trouver le dossier \"%s\"!\n", dest);
		closedir(sourceDir);
		return 0;
	}
	
	// Lecture de tous les fichiers et sous-dossiers de la source
	struct dirent* buffer;
	char *sourceFile, *destFile;
	do {
		buffer = readdir(sourceDir);
		if(buffer) // Si il reste une entrée à traiter
		{
			if(buffer->d_type == DT_REG) // C'est un fichier
			{
				// Concaténations des chemins
				sourceFile = catDirFile(source, buffer->d_name);
				destFile = catDirFile(dest, buffer->d_name);
				// Copie du fichier
				copyFile(sourceFile, destFile);
				// Libération de la mémoire après concaténation
				free(sourceFile);
				free(destFile);
			}
			else if(buffer->d_type == DT_DIR) // C'est un dossier
			{
				if(buffer->d_name[0] != '.') // Ce n'est pas . ou ..
				{
					// Concaténation des chemins
					sourceFile = catDirFile(source, buffer->d_name);
					destFile = catDirFile(dest, buffer->d_name);
					// Création d'un nouveau dossier
					copyDir(sourceFile, destFile);
					// Copie du dossier
					copyFromDir(sourceFile, destFile);
					// Libération de la mémoire après concaténation
					free(sourceFile);
					free(destFile);
				}
			}
		}
	} while(buffer);
	
	// Fermeture des dossiers ouverts
	closedir(sourceDir);
	closedir(destDir);
	return 1;
}


char* catDirFile(char path[], char file[])
{
	int i = 0, pathLength, fileLength;
	char* concat;
	
	// Récupération de la longueur du chemin
	while(path[i] != '\0')
		i++;
	if(path[i-1] == '/') // On enlève le '/' de fin s'il est présent
		i--;
	pathLength = i;
	
	// Récupération de la longueur du nom du fichier
	i = 0;
	while(file[i] != '\0')
		i++;
	fileLength = i;

	// Allocation de la mémoire pour le nom final
	concat = calloc(pathLength + fileLength + 2, sizeof(char));
	
	// Copie du chemin dans le nom final
	for(i = 0; i < pathLength; i++)
		concat[i] = path[i];
	concat[i] = '/';
	
	// Copie du nom du fichier dans le nom final
	for(i = pathLength + 1; i < fileLength + pathLength + 2; i++)
		concat[i] = file[i - pathLength - 1];
	concat[i] = '\0';
	
	return concat;
}

