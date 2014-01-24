// Simple C shell produced by Hexiang Hu
// Copyright reserved
// This shell is just for the usage of some simple shell command
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

//Structure for the command string storage as well as the command argument storage

typedef int (*FUNC)(int, char**);

typedef struct{
	char *name;
	FUNC funct;
	char *desp;
} CMD;

//Function to prompt the current time
int print_prompt();
//Function for executing the command
int execute_command(int cmdArgc, char ** cmdArgv)
//Function for parse the string input
void parse_command(char cmd[], int * cmdArgc, char ** cmdArgv);

int command_cd(int argc, char** argv);
int command_pwd(int argc, char** argv);
int command_ls(int argc, char** argv);
CMD * command_search(char * commandName);

//Create a structure for the information about the command supported by this shell
CMD cmd[] = {
	{"pwd", command_pwd, "Print working directory."},
	{"ls", command_ls, "List information about the FILE."},
	{"cd", command_cd, "Change the current directory."},
	{ (char *)NULL, (Function *)NULL, (char *)NULL }
}

int main(int argc, char ** argv)
{
	char * cmdArgv[16];
	int cmdArgc;
	char cmdLine[128];	

	//Main loop for shell
	while(TRUE){
		print_prompt();

		// Get the content of  the command line string input
		if(!fgets(cmdLine, 128, stdin))
			//Output error status when error occurs
			printf("Error with reading commands\n");
		 
		parse_command(cmdLine, &cmdArgc, cmdArgv);

		execute_command(cmdArgv,cmdArgc);
	}
	return 0;
}


int print_prompt(){
	//Data structure for prompting current time
	static time_t current_time;
	static struct tm * local_time;
	static int count = 0;

	//For the first prompt, print some tips for the user
	if(count == 0)
	{
		count++;
		printf("Welcome to My shell~\n");
		printf("Enter quit/q/exit to exit the shell\n");
	}

	//Get the time and transfer into local time
	char c_time_string[25];
	current_time = time(NULL);
	local_time = localtime(&current_time);

	//Print the time as the time format goes
	strftime(c_time_string, 25, "%Y-%m-%d %H:%M:%S", local_time);
	//Print out the prompt
	printf("%s mycshell >>",c_time_string);

	return 0;
}
int execute_command(int cmdArgc, char ** cmdArgv){
	int childPid;
	int status;
	CMD command;

	if(!strcmp(cmdArgv[0], "quit") || !strcmp(cmdArgv[0],"q") || !strcmp(cmdArgv[0],"exit")){
		// if the input command is one of 'quit' or 'q' or 'exit', quit the shell with the exit code 0
		printf(">>===========Quiting mycshell==========<<\n");
		exit(0);
	}else if(isspace(*cmdArgv[0])){
		// if the input command is a space or spacelike characters, step into next iteration
		return 0;
	}

	//System call function fork to create a childprocess
	childPid = fork();
	if (childPid == -1){
		//If goes error, exit with status code -1
		printf("Process creation failed\n");
		exit(-1);
	}else if(childPid == 0){
		//In the child process, execute the command
		if(command = command_search(camArgv[0])){
			command->funct(cmdArgc, cmdArgv);
		}
		else{
			printf("The command is not in supported by the shell itself, execute system provided command\n");
			if (execvp(cmdArgv[0], cmdArgv) == -1) {    
				//If goes error, exit with status code 1
	            printf("ERROR: exec failed\n");
	            exit(1);
        	}
	    }
    }else{
		//Ignore the situation that process running in the background
		waitpid(childPid, &status, 0);
	}

	return 0;
}
char* skip_blank(char* str)
{
	//Skip the whitespace in the string
	while (isspace(*str)) ++str;
	return str;
}
void parse_command(char cmdLine[], int * cmdArgc, char ** cmdArgv){
	cmdLine = skip_blank(cmdLine);
	//Make the next pointer point to the next white character
	char * next;
	//variable for counting the last element
	int last = 0;

	while(next != NULL) {
		//seprate the first argument
		next[0] = '\0';
		cmdArgv[last] = cmdLine;
		++last;
		cmdLine = skip_blank(next + 1);
		next = strchr(cmdLine, ' ');
	}
 
	if (cmdLine[0] != '\0') {
		cmdArgv[last] = cmdLine;
		//process the last element 
		next = strchr(cmdLine, '\n');
		next[0] = '\0';
		++last; 
	}
	cmdArgv[last] = NULL;
	*cmdArgc = last;
}

CMD * command_search(char * commandName)
{
	int i = 0;
	while(cmd[i].name != NULL){
		if (!strcmp(commandName,cmd[i].name)){
			return &cmd[i];
		}
		i++;
	}

	return NULL;
}


int command_cd(int argc, char** argv){

}
int command_pwd(int argc, char** argv){

}
int command_ls(int argc, char** argv){

}