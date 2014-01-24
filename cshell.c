// Simple C shell produced by Hexiang Hu
// Copyright reserved
// This shell is just for the usage of some simple shell command
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//Mac OS X directory API library
#include <dirent.h>


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
int execute_command(int argc, char ** argv);
//Function for parse the string input
void parse_command(char cmd[], int * argc, char ** argv);

int command_cd(int argc, char** argv);
int command_pwd(int argc, char** argv);
int command_ls(int argc, char** argv);
CMD * command_search(char * commandName);

int is_quit(char * str);

//Create a structure for the information about the command supported by this shell
CMD cmd[] = {
	{"pwd", command_pwd, "Print working directory."},
	{"ls", command_ls, "List information about the FILE."},
	{"cd", command_cd, "Change the current directory."},
	{ (char *)NULL, (FUNC)NULL, (char *)NULL }
};

char * cmdArgv[128];
int cmdArgc;
char cmdLine[1024];		

int main(int argc, char ** argv)
{
	//Main loop for shell
	while(TRUE){
		print_prompt();
		fflush(NULL);
		// Get the content of  the command line string input
		if(!fgets(cmdLine, 128, stdin))
			//Output error status when error occurs
			fprintf(stderr, "Error with reading commands\n");

		parse_command(cmdLine, &cmdArgc, cmdArgv);
		if(cmdArgv[0] != NULL){
			if(is_quit(cmdArgv[0])){
				// if the input command is one of 'quit' or 'q' or 'exit', quit the shell with the exit code 0
				printf(">>===========Quiting mycshell==========<<\n");
				exit(0);
			}

			execute_command(cmdArgc, cmdArgv);
		}
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
int execute_command(int argc, char ** argv){
	int childPid;
	int status;
	CMD * command;

	//System call function fork to create a childprocess
	childPid = fork();
	if (childPid == -1){
		//If goes error, exit with status code -1
		fprintf(stderr, "Process creation failed\n");
		exit(-1);
	}else if(childPid == 0){
		//In the child process, execute the command
		
		if((command = command_search(argv[0]))){
			command->funct(argc, argv);
		}
		else{
			if (execvp(argv[0], argv) == -1) {    
				//If goes error, exit with status code 1
	            fprintf(stderr, "ERROR: execvp failed\n");
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
void parse_command(char cmd[], int * argc, char ** argv){
	cmd = skip_blank(cmd);
	//Make the next pointer point to the next white character
	char* next = strchr(cmd, ' ');
	//variable for counting the last element
	int last = 0;

	while(next != NULL) {
		//seprate the first argument
		next[0] = '\0';
		argv[last] = cmd;
		++last;
		cmd = skip_blank(next + 1);
		next = strchr(cmd, ' ');
	}

	if (cmd[0] != '\0') {
		argv[last] = cmd;
		//process the last element
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++last; 
	}
	*argc = last;
	argv[last] = NULL;

}

int is_quit(char * str){
	if(!strcmp(str, "quit") || !strcmp(str,"q") || !strcmp(str,"exit"))
		return TRUE;
	return FALSE;
}
//Shell Command 

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
	return 0;
}
int command_pwd(int argc, char** argv){
	return 0;
}
int command_ls(int argc, char** argv){
	DIR *dir;
	char * dir_path = ".";
	struct dirent *dp;

	if((strcmp(argv[argc - 1],"ls")))
	{
		dir_path = argv[argc-1];
	}

    if ((dir = opendir (dir_path)) == NULL) {
        fprintf(stderr, "%s is not a directory.\n", dir_path);
        return -1;
    }

    while ((dp = readdir (dir)) != NULL) { 
            if((strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)) {
                continue;
            }

            printf("%s\n", dp->d_name);
    }
	closedir(dir);

	return 0;
}