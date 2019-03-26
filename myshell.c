#include<stdio.h>
#include<unistd.h> 
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h> 
#define IN_NUM 5000
#define HUND 100

void getIn();
void executeStuff();
int simpleParse();
void doEverything();
void complicatedExecute();
void launch_process();
//comment
int main(){
	int flag[HUND];
	char input[IN_NUM];
	char *parse[HUND];


	printf("\033[H\033[J");
	printf("*********MYSHELL*************\n Make sure to put a single space between all tokens including special symbols such as ';', '>', and '|' \n");
	printf("Note: I was unable to get grep working but piping in general should work \n");
	while (1){
		getIn(input);
		if (strlen(input) == 1)
			continue;
		//printf("%li here %s\n",strlen(input),input
		int flag = simpleParse(input, parse);
		if (!flag){
			//printf("at the flag\n");
			executeStuff(parse);
			continue;
		}
		doEverything(parse);

	}
}

struct process{
  struct process * next;
  pid_t pid;
  char **argv;
};
struct job{
  struct process * pipe;
  int in_fd; // stdin (could be changed to a file)
  int out_fd; // stdout (again, could be redirected to file)
  int err_fd;
};

void doEverything(char ** parse){
//printf("doEverything\n");
	struct job * jP = malloc(sizeof(struct job));
	jP->in_fd = STDIN_FILENO;
	jP->out_fd = STDOUT_FILENO;
	struct process *sP = malloc(sizeof (struct process) );
	sP->argv = malloc(2048);
	jP->pipe = sP;
	int k = 0;
	for (int i = 0; i < HUND; i++){
//printf("%i\n", i);

		if (parse[i] == NULL){
			complicatedExecute(jP);
			/*if(jP->pipe->next == NULL){
				printf("JP THIS IS null\n");
			//printf("%s\n",jP->pipe->argv[0]);
			}else{ 
				printf("%s in the place\n", jP->pipe->next->argv[1]);
			}*/
			break;
		}

//printf("%ihere\n", i);
		if (strcmp( parse[i], ";") == 0){
			complicatedExecute(jP);//assuming that part works
			struct job *j = malloc(sizeof(struct job));
			jP = j;
			struct process *s = malloc(sizeof (struct process));
			s = sP;
			sP->argv = malloc(2048);
			jP->pipe = sP;
			jP->in_fd = STDIN_FILENO;
			jP->out_fd = STDOUT_FILENO;
			k = 0;
			continue;
		}
		if (strcmp( parse[i], "|") == 0){
//printf("in pipe\n");
			struct process *s = malloc(sizeof (struct process));
			sP->next = s;
			sP = s;
			/*if(jP->pipe->argv[0] == NULL){
				printf("JP THIS IS null\n");
			//printf("%s\n",jP->pipe->argv[0]);
			}else{
				printf("NOT null\n");
			}*/
			sP->argv = malloc(2048);
			k = 0;
			continue;
		}
		sP->argv[k] = malloc(sizeof(parse[i]));
		strcpy(sP->argv[k],parse[i]);
//printf("%s at %i in jP \n", jP->pipe->argv[k], k);
		k++;
		
	}
	free(jP);
}

void complicatedExecute (struct job * j){
	struct process * p;
	pid_t pid;
	int pipe_fd[2];
	int in_fd;
	int out_fd;
	in_fd = j->in_fd;
	for (p = j->pipe; p; p = p->next){
		if (p->next != NULL){ // if this is not the last process we need to pipe
	    	if (pipe(pipe_fd) < 0){
	      		perror("pipe");
	      		exit(EXIT_FAILURE);
	    	}
	    	out_fd = pipe_fd[1];//write
	  	}else
	    	out_fd = j->out_fd;
	 	pid = fork();
	  	if (pid == (pid_t) 0){
	    	launch_process(p, in_fd, out_fd, j->err_fd);

	  	}
	  	else if (pid > (pid_t) 0){
	    	p->pid = pid;
	    	if (in_fd != j->in_fd)
	      		close(in_fd);
	    	if (out_fd != j->out_fd)
	      		close(out_fd);
	    	in_fd = pipe_fd[0];//read
	    	wait(NULL);
	  	}
	}
}

void launch_process(struct process * p, int in_fd, int out_fd, int err_fd){
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
    if (execvp(p->argv[0], p->argv) < 0) { 
            printf("\nCould not execute..\n"); 
    } 
    close(in_fd);
    close(out_fd);
    exit(0);
}

int simpleParse(char* str, char** parsed) { 
	int r = 0;
	if(strchr(str, '|') != NULL || strchr(str, '<') != NULL ||strchr(str, '>') != NULL || strchr(str, ';') != NULL) {
		r = 1;
	}
    for (int i = 0; i < HUND; i++) { 
        parsed[i] = strsep(&str, " "); 
        //printf("Parsed at %i: %s\n",i,parsed[i] );
        if (parsed[i] == NULL) 
            break; 
        if (parsed[i][0] == ' ')
        	i--;
    } 

    return r;
} 

void executeStuff(char** parsed){
    pid_t pid = fork();  
    //lprintf("%i\n",pid);
  
	if (pid == 0) { 
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute..\n"); 
        } 
        exit(0); 
    } else { 
        wait(NULL);  
        return; 
    } 
}

void getIn(char* str){ 
//basically a function just for doing all the over head you need for getline in c
	char buffer[IN_NUM];
	char *b = buffer;
    size_t bufsize = 32;
	getline(&b,&bufsize,stdin);
	buffer[strlen(buffer)-1] = '\0';//getting rid of the \n character 
	strcpy(str,buffer);//copying buffer to str

}