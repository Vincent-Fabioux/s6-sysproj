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
	write(fileDestination,"\n",strlen("\n"));
	close(fileDestination);
}


int readCommand(char *pathFileHistory){	
	char * buffer= NULL;
    char *entryPosition = NULL;
	int *positionRedirection=malloc(sizeof(int)*ARG_MAX_NUMBER-1);
	
	buffer=malloc(COMMAND_MAX_LENGTH*sizeof(char));
	int nb_pipes=0;
	int nb_redirections=0;

    if(fgets(buffer, COMMAND_MAX_LENGTH, stdin)){
        /*on cherche le \n ajoute automatiquement par fgets*/
        entryPosition = strchr(buffer, '\n');
        /*si on en trouve un, on le remplace par \0*/
        if (entryPosition != NULL) *entryPosition = '\0';

		char ** command=parseStringPipesAndRedirections(buffer,&nb_pipes,&nb_redirections,positionRedirection);
		addInput(buffer,pathFileHistory);
		if(strcmp(command[0], "exit") == 0)
		{
			freeStringTable(command); // Libération de la liste de commandes
			return 0;
		}
		
		executeCommand(command,nb_pipes,nb_redirections,positionRedirection,pathFileHistory);
		freeStringTable(command); // Libération de la liste de commandes
		free(buffer);
		free(positionRedirection);
		return 1;
	}
	else{// CTRL+D utilisé: fin du programme
		return 0;
	}
}

int executeCommand(char** command,int nb_pipes,int nb_redirections,int *positionRedirection ,char * pathFileHistory){
	int i=0;
	int j=0;
	int pid; // Permet de réaliser le fork 
 	int status; // Permet d'attendre les différents processus
	int pipes[nb_pipes*2]; // Permet de réaliser les pipes 
	int out, in;//Correspondant au fichier pour les redirections 
	int saved_stdin = dup(0), saved_stdout = dup(1); // Permet de sauvergarder les entrées et sorties standards
	char **command2; // Permet de redecouper chaque commande avec ses arguments
	char * path=malloc(sizeof(char)*COMMAND_MAX_LENGTH); // Permet de trouver le chemin de chaque commande

	command2=parseString(command[i]);// On decoupe une premiere fois la 1ere commande pour voir s'il ne s'agit pas de cd 
	
	//S'il s'agit de cd 
	if(strcmp(command2[0], "cd") == 0)
	{
		commandCd(command2);
		return EXIT_SUCCESS;
	}
	
	//On cree les pipes
	for(i = 0; i < nb_pipes; i++){  
		if(pipe(pipes + i*2) < 0) {
			perror("Couldn't Pipe");
            exit(EXIT_FAILURE);
        }
	}
	
	//Gestion des redirections
    for(i=0;i<nb_redirections;i++){
	
		//Case '<' : entrée standard
		if(positionRedirection[i]<0){
			in = open(command[-1*positionRedirection[i]], O_RDONLY);	// On ouvre le fichier en question 
			dup2(in,0);	 // On lui attribue l'entrée standard 
			close(in);	// On le ferme
		}
	
		//Case '>' : sortie standard
		else{
			out = open(command[positionRedirection[i]], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);	// On ouvre le fichier en question et on le cree s'il n'existe pas
			dup2(out,1);	//On lui attribue la sortie standard
			close(out);	// On le ferme
		}
	}
    //Gestion des pipes 
    for (i = 0; i < nb_pipes+1; ++i) {	
		if(i!=0){
			path=NULL;
			free(path);
		}
		for(j=0;j<ARG_MAX_NUMBER;j++){
			command2[j]=NULL;
			free(command2[j]);
		}
		free(command2);
		
		command2=parseString(command[i]); // On decoupe chaque commande avec ses arguments 
		path=findPath(getenv("PATH"),command2[0]); // On cherche le chemin de chaque commande
        
        pid = fork();
        if(pid == 0) {	// Processus fils: exécute la commande
			//Ecriture
            if(i < nb_pipes){
                if(dup2(pipes[(i*2) + 1], 1) < 0){
                    exit(EXIT_FAILURE);
                }
            }
            //Lecture
            if((i*2) != 0 ){
                if(dup2(pipes[(i*2)-2], 0) < 0){
                    exit(EXIT_FAILURE);
                }
            }
            //Fermeture
            for(j= 0; j < 2*nb_pipes; j++){
                    close(pipes[j]);
            }
            if(strcmp(command2[0], "touch") == 0) // S'il s'agit de la commande touch
				commandTouch(command2);
			else if(strcmp(command2[0], "cp") == 0) // S'il s'agit de la commande cp
				commandCopy(command2);
			else if(strcmp(command2[0], "cat") == 0) // S'il s'agit de la commande cat
				commandCat(command2);
			else if(strcmp(command2[0], "history") == 0) // S'il s'agit de la commande history
				commandHistory(command2,pathFileHistory);
			else if(execv(path, command2) < 0) // Sinon on execute la commande a l'aide de son chemin
			{
				printf("Erreur: Commande non existante...\n");
				exit(0);
			}
        }
        else if(pid < 0){	
			// Process inconnu
            printf("Erreur: processus non reconnu...\n");
            exit(EXIT_FAILURE);
        }
    }
	// Fermeture des pipes
    for(i = 0; i < 2 * nb_pipes; i++){
        close(pipes[i]);
    }
 
	// Restauration de l'input/output de la console
	dup2(saved_stdin, 0);
	close(saved_stdin);
	dup2(saved_stdout, 1);
	close(saved_stdout);
	
	// Attente du processus 
    for(i = 0; i < nb_pipes + 1; i++){
        wait(&status);
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
		if(string[i] == ' ' || string[i] == '\n' || string[i] == '\0') // Nouveau mot ou fin
		{
			buffer[j][k] = '\0'; // On termine la chaîne de caractères en cours
			while(string[i] == ' ') // On ignore les espaces suivants
				i++;
			k = 0;
			j++; // On passe à la chaîne de caractères suivante
			if(string[i] == '\n' || string[i] == '\0') // Plus aucun mot: fin
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

char ** parseStringPipesAndRedirections(char* string, int * nb_pipes,int *nb_redirections,int *positionRedirection){
	
	char ** buffer=calloc(ARG_MAX_LENGTH,sizeof(char*));
	
	int i = 0; // Increment du numéro de caractère dans string
	int j = 0; // Increment du numéro de chaîne de caractère
	int k = 0; // Increment du numéro de caractère dans une chaîne
	
	while(string[i] == ' ') // On ignore les premiers espaces
		i++;
	// Allocation du tableau de pointeur vers les chaînes de caactères	
	buffer[0]=calloc(COMMAND_MAX_LENGTH,sizeof(char));
	while(1) // Stoppé avec un return
	{	
		if(string[i]>=0){ // Si les caracteres sont bien representés
			if(string[i] == '|' ||string[i] == '\n' || string[i] == '\0' || string[i] == '>' || string[i] == '<' ) // Nouveau mot ou fin
			{			
				buffer[j][k] = '\0'; // On termine la chaîne de caractères en cours
				k = 0;
				j++; // On passe à la chaîne de caractères suivante
				if(string[i] == '\n'|| string[i] == '\0' ) // Plus aucun mot: fin
				{
					return buffer;
				}
				else if(string[i] == '>'){
					positionRedirection[*nb_redirections]=j;
					*nb_redirections=*nb_redirections+1;
				}
				else if(string[i] == '<'){
					positionRedirection[*nb_redirections]=-1*j;
					*nb_redirections=*nb_redirections+1;
				}
				else if(string[i] == '|'){
					*nb_pipes=*nb_pipes+1;
				}
				i++;
				// Allocation de la chaîne de caractère suivante
				buffer[j]=calloc(COMMAND_MAX_LENGTH,sizeof(char));
			}
			else {	
				if((k==0&&string[i]==' ')){ // Si des espaces se trouvent avant un nouveau mot 
					i++;
				}
				else{
					buffer[j][k] = string[i];
					k++;
					i++;
				}					
			}
		}
		else{
			i++;
		}
	}
}
