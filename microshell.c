#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>
#include <termios.h>

#define PINK "\x1B[35m"
#define PURPLE "\x1B[34m"
#define END "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define SIZE 200
#define MAX_HISTORY 1000

int count;
int counter_history_entries;
int how_many;
char character;
char before_command[SIZE];
char entry[MAX_HISTORY][SIZE];
int get_char();

void prompt(char *current_directory){
    char *login=getlogin();
    printf(PURPLE"*("END RED"%s"END":"GREEN"%s"END PURPLE")*"END PINK"\n$ "END,login,current_directory);
}
void help(){
    printf(PINK"Maksymilian Mikołajczak is an author of this microshell\n"END);
    printf(PURPLE"exit - this command is going to end the action of microshell\n"END);
    printf(PINK"pwd - this command is going to show current working directory path\n"END);
    printf(PURPLE"HOME - this command is going to show home path\n"END);
    printf(PINK"cd - you can change directory by this command using . or .. or - or path\n"END);
    printf(PURPLE"touch - make file, if exsits then remake file\n"END);
    printf(PINK"history - it is showing history of commands\n"END);
}
void history(){
    FILE *file;
    int num=0;
    char character_from_history;
    if(!(file=fopen("history.txt","r"))){
        file=fopen("history.txt","w");
    }else{
        file=fopen("history.txt","a+");
        character_from_history=getc(file);
        while(character_from_history!=EOF){
            if(character_from_history=='\n'){
                num++;
            }
            character_from_history=getc(file);
        }
    }
    fprintf(file,"%d     %s\n",num+1,before_command);
    fclose(file);
}
void to_open_history_file(){
    FILE *file;
    file=fopen("history.txt","r");
    char element[200];
    char *history_element=fgets(element,200,file);
    printf("%s",history_element);
    while(fgets(element,200,file)){
        printf("%s",history_element);
    }
    fclose(file);
}
int history_how_many(){
    FILE *file;
    int how_many=-1;
    char character_from_history;
    if(file=fopen("history.txt","r")){    
        how_many=0;
        character_from_history=getc(file);
        while(character_from_history!=EOF){
            if(character_from_history=='\n'){
                how_many++;
            }
            character_from_history=getc(file);
        }
        fclose(file);
    }
    return how_many;
}
void make_array_of_history_commands(){
    FILE *file;
    file=fopen("history.txt","r");
    char helper_pointer[200];
    int pointer=0;
    for(int i=0;i<how_many;i++){
        char *get_entry=fgets(helper_pointer,200,file);
        get_entry[strlen(helper_pointer)-1]='\0';
        if(i<9){
            pointer=6;
        }else if(i<99){
            pointer=7;
        }else{
            pointer=8;
        }
        strcpy(entry[i],&get_entry[pointer]);
    }
    fclose(file);
}
void arrows(){
    get_char();
    char arrow_key=get_char();
    switch(arrow_key){
        case 'A' :
            for(int x=0;x<=count;x++){
                printf("\b \b");
            }
            counter_history_entries--;
            if(counter_history_entries<0){
                counter_history_entries=0;
                printf("%s",entry[counter_history_entries]);
                count=strlen(entry[counter_history_entries])-1;
                strcpy(before_command, entry[counter_history_entries]);
            }else{
                printf("%s",entry[counter_history_entries]);
                count=strlen(entry[counter_history_entries])-1;
                strcpy(before_command, entry[counter_history_entries]);
            }
            break;
        case 'B' :
            for(int x=0;x<=count;x++){
                printf("\b \b");
            }
            counter_history_entries++;
            if(counter_history_entries>=how_many){
                counter_history_entries=how_many;
                count=-1;
                memset(before_command,0,SIZE);
            }else{
                printf("%s",entry[counter_history_entries]);
                count=strlen(entry[counter_history_entries])-1;
                strcpy(before_command, entry[counter_history_entries]);
            }
            break;
    }
}
void splitter(char **command_split){                                                                                                                                                                                   
    command_split[0]=strtok(before_command," ");
    int idx_command=0;
    while(command_split[idx_command]!=NULL){
        command_split[idx_command+1] = strtok(NULL," ");
        idx_command++;
    }
}
int get_char(void){
    struct termios oldattr, newattr;
    int character;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr=oldattr;
    newattr.c_lflag &=~( ICANON | ECHO );
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    character=getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return character;
}
void touch(char *filename){
    FILE *file;
    struct stat fs;
    time_t modtime;
    struct utimbuf new_time;
    file=fopen(filename,"r");
    if(file==NULL){
        file=fopen(filename,"a");
        fclose(file);
        file=fopen(filename,"r");
        if(file==NULL){
            perror(RED"TYPE ERROR : \n"END);
        }
    }else{
        stat(filename,&fs);
        modtime=fs.st_mtime;
        new_time.actime=fs.st_atime+fs.st_mtime;
        new_time.modtime=time(NULL);
        if(utime(filename,&new_time)<0){
            printf(RED"TYPE ERROR :\n*unable to touch this file*"END);
        }
    }
}
int main()
{
    char home[200];
    char prev_1[200];
    getcwd(prev_1,sizeof(prev_1));
    getcwd(home,sizeof(home));
    char *command;
    char *command_split[100];
    counter_history_entries=0;
    for(;;){
        int PATH_MAX=200;
        char cwd[PATH_MAX];
        getcwd(cwd,sizeof(cwd));
        prompt(cwd);
        count=-1;
        char *com_mem_help;
        chdir(home);
        how_many=history_how_many();
        chdir(cwd);
        if(how_many<0){
            how_many=0;
        }
        if(how_many>0){
            chdir(home);
            make_array_of_history_commands();
            chdir(cwd);
        }
        counter_history_entries=how_many;
        memset(before_command,0,SIZE);
        character=get_char();
        while(character!='\n'){
            if(character!=127 && character!='\033'){
                count++;
                before_command[count]=character;
                printf("%c",before_command[count]);
            }else if(character=='\033'){
                if(how_many>0){
                    arrows();
                }else{
                    get_char();
                    get_char();
                }
            }else{
                if(count>=0){
                    before_command[count]='\0';
                    count--;
                    printf("\b \b");
                }
            }
            character=get_char();
        }
        printf("\n");
        if(strlen(before_command)!=0 && before_command[0]!=' '){
            chdir(home);
            history();
            chdir(cwd);
        }
        if(before_command[0]==' '){
            char command_helper[SIZE-1];
            strcpy(command_helper,&before_command[1]);
            strcpy(before_command,command_helper);
        }
        if(strcmp(before_command,"exit")==0){
            exit(0);
        }else if(strcmp(before_command,"help")==0){
            help();
        }else if(strcmp(before_command,"history")==0){
            chdir(home);                                                                                                                                         
            to_open_history_file();
            chdir(cwd);
        }else if(strcmp(before_command,"history -c")==0){
            chdir(home);
	    fopen("history.txt","w");
            chdir(cwd);
        }else if(strcmp(before_command,"pwd")==0){
            printf("%s\n",cwd);
        }else if(strcmp(before_command,"HOME")==0){
            printf("%s\n",home);
        }else if(strcmp(before_command,"cd")==0 || strcmp(before_command,"cd ~")==0){
            getcwd(prev_1,sizeof(prev_1));
            chdir(home);
        }else if(strcmp(before_command,"cd -")==0){
            printf("%s\n",prev_1);
            chdir(prev_1);
            strcpy(prev_1,cwd);
        }else if(strlen(before_command)==0){
            continue;
        }else{
            splitter(command_split);
            if(strcmp(command_split[0],"touch")==0){
                if(command_split[2]==NULL && command_split[1]!=NULL){
                    touch(command_split[1]);
                }else{
                    printf(RED"TYPE ERROR : \n*incorrect syntax*\n"END);
                }
            }else if(strcmp(command_split[0],"cd")==0){
                if(command_split[2]==NULL){
                    if(chdir(command_split[1])!=0){
                        perror(RED"TYPE ERROR : \n"END);
                    }else{
                        strcpy(prev_1,cwd);
                    }
                }
            }else{
                int status=0;
                if(fork()==0){
                    exit(execvp(command_split[0],command_split));
                }else{
                    wait(&status);
                    if(status){
                        printf(RED"TYPE ERROR : *unknown command*\n"END);
                    }
                }
            }
        }

    }
    return 0;
}
