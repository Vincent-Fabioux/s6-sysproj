#include "step3.h"

char *findPath(char *path,char *command){
	char *buffer=NULL;
	int i=0;//Increment pour la taille de toute la chaine du PATH global 
	int k=0;//Increment pour la copie des paths

	//On lui alloue la memoire necessaire
	buffer=calloc(DIRECTORY_MAX_LENGTH,sizeof(char));
	while(i<strlen(path)){
		//Si on arrive a la fin d'un path
		if(path[i]==':'){
			// On lui rajoute le / et la commande 
			strcat(buffer,"/");
			strcat(buffer,command);
			//Si la commande que l'on recherche existe dans ce path
			if(!access(buffer,X_OK)){
				return buffer;
			}
			else{
				//Sinon on la supprime et on realloue la memoire necessaire
				free(buffer);
				buffer=calloc(DIRECTORY_MAX_LENGTH,sizeof(char));
			}
			k=0;//On remets a 0 pour la copie des paths
		}
		else{
			//Sinon on copie les differents paths
			buffer[k]=path[i];
			k++;
		}
		i++;
	}
	//Si aucun des paths est valable on retourne NULL
	return NULL;
}
