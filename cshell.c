// Simple C shell produced by Hexiang Hu
// Copyright reserved
// This shell is just for the usage of some simple shell command
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//UNIX directory API library
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

//Structure for the command string storage as well as the command argument storage

typedef int (*FUNC)(int, char**);

typedef struct{
	char *name;
	FUNC funct;
} CMD;

//Function to prompt the current time
int print_prompt();
//Function for executing the command
int command_execute(int argc, char ** argv);
//Function for parse the string input
void command_parse(char cmd[], int * argc, char ** argv);

int command_cd(int argc, char** argv);
int command_pwd(int argc, char** argv);
int command_ls(int argc, char** argv);
int command_mkdir(int argc, char** argv);
int command_rmdir(int argc, char** argv);
CMD * command_search(char * commandName);

int is_quit(char * str);

//Create a structure for the information about the command supported by this shell
CMD cmd[] = {
	{"pwd", command_pwd},
	{"ls", command_ls},
	{"cd", command_cd},
	{"mkdir", command_mkdir},
	{"rmdir", command_rmdir},
	{ (char *)NULL, (FUNC)NULL }
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
			fprintf(stderr, "ERROR READ COMMANDS:%s\n",strerror( errno ));

		command_parse(cmdLine, &cmdArgc, cmdArgv);
		if(cmdArgv[0] != NULL){
			if(is_quit(cmdArgv[0])){
				// if the input command is one of 'quit' or 'q' or 'exit', quit the shell with the exit code 0
				printf(">>===========Quiting mycshell==========<<\n");
				exit(0);
			}

			command_execute(cmdArgc, cmdArgv);
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
int command_execute(int argc, char ** argv){
	int childPid;
	int status;
	CMD * command;

	//System call function fork to create a childprocess
	childPid = fork();
	if (childPid == -1){
		//If goes error, exit with status code -1
		fprintf(stderr, "ERROR CREATING PROCESS:%s\n", strerror( errno ));
		exit(-1);
	}else if(childPid == 0){
		//In the child process, execute the command

		if((command = command_search(argv[0]))){
			if(-1 == command->funct(argc, argv)){
				//fprintf(stderr, "ERROR: Shell function execute failed\n");
				fprintf(stderr, "ERROR EXECUTING SHELL COMMAND: %s\n", strerror( errno ));
			}
		}
		else{
			if (execvp(argv[0], argv) == -1) {    
				//If goes error, exit with status code 1
				fprintf(stderr, "ERROR EXECUTING SYSTEM PROGRAM: %s\n",strerror( errno ));

				exit(1);
			}
		}

	}else{
		//Ignore the situation that process running in the background
		waitpid(childPid, &status, 0);
	}

	return 0;
}
char* blank_skip(char* str)
{
	//Skip the whitespace in the string
	while (isspace(*str)) ++str;
	return str;
}
void command_parse(char cmd[], int * argc, char ** argv){
	cmd = blank_skip(cmd);
	//Make the next pointer point to the next white character
	char* next = strchr(cmd, ' ');
	//variable for counting the last element
	int last = 0;

	while(next != NULL) {
		//seprate the first argument
		next[0] = '\0';
		argv[last] = cmd;
		++last;
		cmd = blank_skip(next + 1);
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
	// Search the name in the shell command library
	while(cmd[i].name != NULL){
		if (!strcmp(commandName,cmd[i].name)){
			return &cmd[i];
		}
		i++;
	}

	return NULL;
}

int command_cd(int argc, char** argv){
	char * dir;
	struct stat buf;

	if(strcmp(argv[argc-1], "cd") == 0) {
		// If no parameters, then change to home directory
		dir = getenv("HOME");
	}else{
		// Else change to the specific directory
		dir = argv[argc-1];
	}

	// Get the file of the path and see if it is a directory
	if((-1 == lstat(dir, &buf)) || !S_ISDIR(buf.st_mode))
		return -1;

	return chdir(dir);

}
int command_pwd(int argc, char** argv){
	long size;
	char * buf, * ptr;

	// Return the maximum value of a directory size for the path
	size = pathconf(".", _PC_PATH_MAX);

	// Alocate memory for the path string
	if ((buf = (char *)malloc((size_t)size)) != NULL) {
		// Get the path name string to the buffer
		ptr = getcwd(buf, (size_t)size);
		printf("%s\n", ptr);
		return 0;
	}else{
		return -1;
	}
}

int command_ls(int argc, char** argv){
	DIR * dir;
	char * dir_path = ".";
	struct dirent *entry;


	// If there is a path name variable, set the directory path to be that argument
	if((strcmp(argv[argc - 1],"ls")))
	{
		dir_path = argv[argc-1];
	}

	// Test if the directory path exists
	if ((dir = opendir (dir_path)) == NULL) {
		return -1;
	}

	while ((entry = readdir (dir)) != NULL) { 
			// Filter the current directory and parent directory
			if((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
				continue;
			}

			printf("%s\n", entry->d_name);
	}
	closedir(dir);

	return 0;
}

int command_mkdir(int argc, char** argv){
	char * dir;
	mode_t cmask = (mode_t) 0777;

	if(argc < 2){
		return -1;
	}

	dir = argv[argc-1];
	if(mkdir(dir,cmask) == -1){
		return -1;
	}

	return 0;
}
int command_rmdir(int argc, char** argv){

	if(argc < 2){
		return -1;
	}

	char * dir = argv[argc-1];
	struct stat buf;

	if((-1 == lstat(dir, &buf)) || !S_ISDIR(buf.st_mode)){
		return -1;
	}

	if(!rmdir(dir)){
		return 0;
	}else{
		return -1;
	}
}