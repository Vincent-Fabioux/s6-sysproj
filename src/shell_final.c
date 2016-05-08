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
#include "step2.h"

#define bool	int
#define TRUE	1
#define FALSE	0

#define DIRECTORY_MAX_LENGTH	1024
#define MACHINE_MAX_LENGTH		128
#define USERNAME_MAX_LENGTH		128



int main(int argc, char** argv)
{
	char pathFileHistory[DIRECTORY_MAX_LENGTH];
	char directoryName[DIRECTORY_MAX_LENGTH];
	char userName[USERNAME_MAX_LENGTH];
	char computerName[MACHINE_MAX_LENGTH];
	char *networkCut=NULL;
	bool mainLoop = TRUE;
	
	getcwd(pathFileHistory, DIRECTORY_MAX_LENGTH*sizeof(char));
	strcat(pathFileHistory,"/history.txt");
	// Récupération du nom d'utilisateur et du nom de machine
	if(getlogin_r(userName, USERNAME_MAX_LENGTH*sizeof(char))
	|| gethostname(computerName, MACHINE_MAX_LENGTH*sizeof(char)) == -1)
		return EXIT_FAILURE;
	
	// Si le nom de machine contient un . (machine.réseau), on coupe le nom au niveau du point
	networkCut = strchr(computerName, '.');
	if (networkCut)
		*networkCut = '\0';
	
	while(mainLoop)
	{
		if(getcwd(directoryName, DIRECTORY_MAX_LENGTH*sizeof(char))) // Récupération du répertoire courant
		{
			printf("%s@%s:%s>", userName, computerName, directoryName);
			mainLoop = readCommand(pathFileHistory); // Lecture de la commande
		}
		else
			return EXIT_FAILURE;
	}
	
	printf("\n");
	return EXIT_SUCCESS;
}
