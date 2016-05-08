#include "commands.h"
#include "step3.h"


/* A COMMENTER/VERIFIER */
void addInput(char *commande,char *pathFileHistory){
	//On ouvre le fichier contenant les commandes et on le cree s'il n'existe pas.
	//De plus on mets le pointeur a la fin du fichier pour pouvoir ecrire facilement dedans.
	int fileDestination=open(pathFileHistory, O_WRONLY|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR);
	//S'il y a un probleme lors de l'ouverture
	if(fileDestination==-1){
		printf("Probleme d'ouverture ");
	}
	//Sinon on ecrit la commande a l'interieur de celui-ci.
	write(fileDestination,commande,strlen(commande));
	close(fileDestination);
}


int readCommand(char *pathFileHistory){	
	char buffer[COMMAND_MAX_LENGTH];
	
	if (fgets(buffer, 200, stdin)) // Commande entrée
	{	int taille=0;
		char** command = parseString(buffer,&taille);
		addInput(buffer,pathFileHistory);
		
		if(strcmp(command[0], "exit") == 0)
		{
			freeStringTable(command); // Libération de la liste de commandes
			return 0;
		}
		executeCommand(command,taille,pathFileHistory);
		freeStringTable(command); // Libération de la liste de commandes
		
		return 1;
	}
	else // CTRL+D utilisé: fin du programme
		return 0;
}

int executeCommand(char **command, int nb_pipes ,char * startingDirectory) {
    int i = 0;
    int j = 0;
    int s = 1;
    int pid;
    int status;
    char *path=NULL;	//Chemin contenant la commande a executer
    int place;	//place dans la chaine de la commande a executer
    int pipefds[2*nb_pipes];//Tableau pour pouvoir piper les commandes
    const int commands = nb_pipes + 1;	//Nombre de commandes a executer 
    int commandStarts[ARG_MAX_NUMBER];	//Table des positions des commandes a exectuer
	
	commandStarts[0] = 0;
	
	//S'il sagit de la commande cd on ne fait pas de fork et on l'execute directement
	if(strcmp(command[0], "cd") == 0)
	{
		commandCd(command);
		return EXIT_SUCCESS;
	}
	//S'il n'est pas possible d'utiliser un pipe entre les expressions
    for(i = 0; i < nb_pipes; i++){  
        if(pipe(pipefds + i*2) < 0) {
            perror("Couldn't Pipe");
            exit(EXIT_FAILURE);
        }
    }
    
	//On va chercher l'endroit ou commence chaque commande, en reperant les pipes
	i=0;j=1;
    while (command[i] != NULL){
        if(!strcmp(command[i], "|")){
            command[i] = NULL;
            commandStarts[j] = i+1;
            j++;
        }
        i++;
    }
    j=0;
    //On va chercher le chemin des commandes si elles sont externes ,ensuite on va utiliser un fork,  puis on va les executer
    for (i = 0; i < commands; ++i) {
        place = commandStarts[i];
        path=findPath(getenv("PATH"),command[place]);

        pid = fork();
        
        if(pid == 0) {	// Processus fils: exécute la commande
            if(i < nb_pipes){
                if(dup2(pipefds[j + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            for(s = 0; s < 2*nb_pipes; s++){
                    close(pipefds[s]);
            }
            if(strcmp(command[0], "touch") == 0)
				commandTouch(command);
			else if(strcmp(command[0], "cp") == 0)
				commandCopy(command);
			else if(strcmp(command[0], "cat") == 0)
				commandCat(command);
			else if(strcmp(command[0], "history") == 0){
				commandHistory(command, startingDirectory);
			}
			else if(execv(path, command+place) < 0)
			{
				printf("Erreur: Commande non existante...\n");
				exit(0);
			}
        }
        else if(pid < 0){	// Processus inconnu
            printf("Erreur: processus non reconnu...\n");
            exit(EXIT_FAILURE);
        }
        j+=2;
    }

    for(i = 0; i < 2 * nb_pipes; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < nb_pipes + 1; i++){
        wait(&status);
    }
    return 1;
}


void freeStringTable(char** table){
	if(table)
	{
		int i = 0;
		while(table[i])
		{	
			table[i] = NULL;
			free(table[i]);
			i++;
		}
		free(table);
	}
}


char** parseString(char* string,int *taille){
	int i = 0; // Increment du numéro de caractère dans string
	int j = 0; // Increment du numéro de chaîne de caractère
	int k = 0; // Increment du numéro de caractère dans une chaîne
	
	// Allocation du tableau de pointeur vers les chaînes de caactères
	char** buffer = malloc(ARG_MAX_NUMBER*sizeof(char*));
	for(i = 0; i < ARG_MAX_NUMBER; i++)
		buffer[i] = NULL;
	i = 0;
	
	// Allocation de la première chaîne de caractères
	buffer[0] = malloc(ARG_MAX_LENGTH*sizeof(char));
	
	while(string[i] == ' ') // On ignore les premiers espaces
		i++;
		
	while(1) // Stoppé avec un return
	{
		if(string[i] == ' ' || string[i] == '\n') // Nouveau mot ou fin
		{
			buffer[j][k] = '\0'; // On termine la chaîne de caractères en cours
			while(string[i] == ' ') // On ignore les espaces suivants
				i++;
			k = 0;
			if(!strcmp(buffer[j],"|")){
				*taille=*taille+1;
			}
			j++; // On passe à la chaîne de caractères suivante
			if(string[i] == '\n'){ // Plus aucun mot: fin
				return buffer;
			}
			// Allocation de la chaîne de caractère suivante
			buffer[j] = malloc(ARG_MAX_LENGTH*sizeof(char));
		}
		else
		{	
			buffer[j][k] = string[i];
			k++;
			i++;
		}
	}
}
