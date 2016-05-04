#include "commands.h"



/* A COMMENTER/VERIFIER */
void addInput(char *commande){
	//On ouvre le fichier contenant les commandes et on le cree s'il n'existe pas.
	//De plus on mets le pointeur a la fin du fichier pour pouvoir ecrire facilement dedans.
	int fileDestination=open("history.txt", O_WRONLY|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR);
	//S'il y a un probleme lors de l'ouverture
	if(fileDestination==-1){
		printf("Probleme d'ouverture");
	}
	//Sinon on ecrit la commande a l'interieur de celui-ci.
	write(fileDestination,commande,strlen(commande));
	close(fileDestination);
}


int readCommand(){	
	
	char buffer[COMMAND_MAX_LENGTH];
	
	if (fgets(buffer, 200, stdin)) // Commande entrée
	{
		char** command = parseString(buffer);
		addInput(buffer);
		
		if(strcmp(command[0], "exit") == 0)
		{
			freeStringTable(command); // Libération de la liste de commandes
			return 0;
		}
		
		executeCommand(command);
		freeStringTable(command); // Libération de la liste de commandes
		
		return 1;
	}
	else // CTRL+D utilisé: fin du programme
		return 0;
}


int executeCommand(char** command){
	if(strcmp(command[0], "cd") == 0)
	{
		commandCd(command);
		return EXIT_SUCCESS;
	}
	
	pid_t pid = fork();
	int pstatus;
	
	if(pid == 0) // Processus fils: exécute la commande
	{
		if(strcmp(command[0], "touch") == 0)
			commandTouch(command);
		else if(strcmp(command[0], "cp") == 0)
			commandCopy(command);
		else if(strcmp(command[0], "cat") == 0)
			commandCat(command);
		else if(strcmp(command[0], "history") == 0)
			commandHistory(command);
		else if(execvp(command[0], command) < 0)
		{
			printf("Erreur: Commande non existante...\n");
			exit(0);
		}
		exit(1); // Fin du processus fils
	}
	else if(pid < 0) // Processus inconnu
	{
		printf("Erreur: processus non reconnu...\n");
		exit(0);
	}
	else // Processus père: attends le fils
	{
		while(wait(&pstatus) != pid); // Attente
		if(!strcmp(command[0], "cd") && command[1])
		{		
			exit(0);
		}
	}
	return EXIT_SUCCESS;
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


char** parseString(char* string){
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
			j++; // On passe à la chaîne de caractères suivante
			if(string[i] == '\n') // Plus aucun mot: fin
				return buffer;
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
