#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
//&&&&&&&&&&&&&&& Auxiliary functions for printing command and errors  &&&&&&&&&&&&& // 
const char ERROR_MSG[20] = "MPCS Shell error\n";
void printErr(){
    write(STDERR_FILENO, ERROR_MSG, strlen(ERROR_MSG));
}
void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}
//&&&&&&&&&&&&&&& Checking Input Validity &&&&&&&&&&&&& // 
int input_too_long(char* pinput){
    int too_long = 0;
    if(pinput[strlen(pinput)-1]!='\n'){
        too_long = 1;
    }
    return too_long;
}
//&&&&&&&&&&&&&&& Built in commands &&&&&&&&&&&&& // 
int built_in_command(int argc, char* argv[]){
    if (!strcmp(argv[0],"quit")){
        exit(0);
    }
    if(!strcmp(argv[0],"pwd")){
        if(argc!=1){
            printErr();
            return 1;
        }
        char buf[PATH_MAX + 1];
        char *ptr;
        ptr = getcwd(buf,PATH_MAX+1);
        myPrint(ptr);
        myPrint("\n");
        return 1;
    }
    if(!strcmp(argv[0],"cd")){
        if(argc==1){
            chdir(getenv("HOME"));
            return 1;
        }else if(argc==2){
            int chdir_success = chdir(argv[1]);
            if(!chdir_success){
                return 1;
            }else{
                printErr();
                return 1;
            }
        }else if(argc > 2){
            printErr();
            return 1;
        }
    }
    if(!strcmp(argv[0],"echo")){
        if(argc==1){
            myPrint("\n");
            return 1;
        }else if(argc==2){
            myPrint(argv[1]);
            myPrint("\n");
            return 1;
        }else{
            printErr();
            return 1;
        }
    }
    return 0;
}
//&&&&&&&&&&&&&&& Parallel &&&&&&&&&&&&& // 
int detect_parallel(char* input){
    // returns 1 if parallel
    // returns -1 if error 
    // returns 0 if no parallel 
    char* para = strchr(input, '&');
    char* mult = strchr(input, ';');
    if((para!=NULL) && (mult == NULL)){
        return 1;
    }else if((para!=NULL) && (mult!=NULL)){
        return -1;
    }else{
        return 0;
    }
}
//&&&&&&&&&&&&&&& Pipe &&&&&&&&&&&&& // 
int detect_pipe(char* input){
    //returns -1 if both pipe and redir present 
    //returns 0 if no pipe
    //returns 1 if valid pipe command
    char* pipe = strchr(input, '|');
    char* redir = strchr(input, '>');
    if((pipe!=NULL) && (redir==NULL)){
        return 1;
    }else if((pipe!=NULL) && (redir!=NULL)){
        return -1;
    }else{
        return 0;
    }
}
int count_pipe(char* input){
    // count the number of pipe chars in a command 
    int count = 0;
    int len = strlen(input);
    for(int j = 0; j < len; j++){
        if(input[j] == '|'){
            count++;
        }
    }
    return count;
}
int check_built_in_pipe(char *argv[]){
    // if pipe is present in a command, check that there is no built in commands
    if(!strcmp(argv[0],"quit")){
        return 1;
    }
    if(!strcmp(argv[0],"pwd")){
        return 1;
    }
    if(!strcmp(argv[0],"cd")){
        return 1;
    }
    if(!strcmp(argv[0],"echo")){
        return 1;
    }
    return 0;
}
int detect_pipe_within_string(char** argv, int argc,int* arg_pos, int* char_pos){
    for(int i=0;i<argc;i++){
        int len = strlen(argv[i]);
        if(len>=2){
            for(int j=0; j<len;j++){
                if(argv[i][j]=='|'){
                    *arg_pos = i;
                    *char_pos = j;
                    return 1;
                }
            }
        }
    }
    return 0;
}
//&&&&&&&&&&&&&&& Redirection &&&&&&&&&&&&& // 
int detect_redirection_within_string(char** argv, int argc,int* arg_pos, int* char_pos){
    for(int i=0;i<argc;i++){
        int len = strlen(argv[i]);
        if(len>=2){
            for(int j=0; j<len;j++){
                if(argv[i][j]=='>'){
                    *arg_pos = i;
                    *char_pos = j;
                    return 1;
                }
            }
        }
    }
    return 0;
}
char** create_redir_argv(char** argv, int argc, int arg_pos, int char_pos, int* change_argc){
    int len_of_arg = strlen(argv[arg_pos]);
    if(char_pos==0){
        *change_argc = 1;
        char* redir = malloc(2);
        char* after = malloc(len_of_arg);
        strcpy(redir,">");
        int i = 0;
        for(int j =1;j<len_of_arg;j++){
            after[i] = argv[arg_pos][j];
            i++;
        }
        after[len_of_arg] = '\0';
        char** new_argv = malloc((argc+1+1)*sizeof(char*));
        for(int k =0;k<arg_pos;k++){
            new_argv[k] = argv[k];
        }
        new_argv[arg_pos] = redir;
        new_argv[arg_pos+1] = after;
        for(int k=arg_pos+1;k<argc;k++){
            new_argv[k+1] = argv[k];
        }
        new_argv[argc+1] = NULL;
        free(argv);
        return new_argv;
    }else if(char_pos==len_of_arg-1){
        *change_argc = 1;
        char* redir = malloc(2);
        char* before = malloc(len_of_arg);
        strcpy(redir,">");
        int i = 0;
        for(int j=0;j<char_pos;j++){
            before[i] = argv[arg_pos][j];
            i++;
        }
        before[len_of_arg] = '\0';
        char** new_argv = malloc((argc+1+1)*sizeof(char*));
        for(int k=0; k<arg_pos;k++){
            new_argv[k] = argv[k];
        }
        new_argv[arg_pos] = before;
        new_argv[arg_pos+1] = redir;
        for(int k = arg_pos+1;k<argc;k++){
            new_argv[k+1] = argv[k];
        }
        new_argv[argc+1] = NULL;
        free(argv);
        return new_argv;
    }else{
        *change_argc = 2;
        char* redir = malloc(2);
        char* before = malloc(char_pos+1);
        char* after = malloc(len_of_arg-char_pos);
        strcpy(redir,">");
        int i_bef = 0;
        for(int j_bef  =0;j_bef<char_pos;j_bef++){
            before[i_bef] = argv[arg_pos][j_bef];
            i_bef++;
        }
        before[char_pos+1] = '\0';
        int i_af = 0;
        for(int j_af=char_pos+1;j_af<len_of_arg;j_af++){
            after[i_af] = argv[arg_pos][j_af];
            i_af++;
        }
        after[len_of_arg-char_pos] = '\0';
        char** new_argv = malloc((argc+2+1)*sizeof(char*));
        for(int k = 0; k<arg_pos;k++){
            new_argv[k] = argv[k];
        }
        new_argv[arg_pos] = before;
        new_argv[arg_pos+1] = redir;
        new_argv[arg_pos+2] = after;
        for(int k = arg_pos+1;k<argc;k++){
            new_argv[k+2] = argv[k];
        }
        new_argv[argc+2] = NULL;
        free(argv);
        return new_argv;
    }
}
int detect_redirection(char** argv,int argc){
    int i = 0;
    while(argv[i]){
        if(!strcmp(argv[i],">")){
            if(i==0){
                return -999;
            }
            return i;
        }else{
            int len = strlen(argv[i]);
            for(int j=0;j<len;j++){
                if(argv[i][j]=='>'){
                    return -999;
                }
            }
        }
        i++;
    }
    return 0;    
}
int get_fd(int redirection_index, char** argv, int argc){
    if(redirection_index != (argc-2)){ // only the file name should appear after >
        return -1;
    }
    int fd = open(argv[argc-1],O_RDWR|O_TRUNC);
    if(fd != -1){
        return fd;
    }else{
        fd = creat(argv[argc-1],S_IRUSR|S_IWUSR);
        return fd;
    }
    return fd;
}
int easy_detect_redir(char* input){
    char* redir = strchr(input, '>');
    if(redir!= NULL){
        return 1;
    }else{
        return 0;
    }
}
//&&&&&&&&&&&&&&& Parse one job &&&&&&&&&&&&& // 
int count_argc(char* cmd){
    char* token;
    int argc = 0;
    char* copy = malloc(strlen(cmd)+1);
    strcpy(copy,cmd);
    char* rest;
    
    token = strtok_r(copy, " \t",&rest);
    while(token){
        argc++;
        token = strtok_r(NULL, " \t",&rest);
    }
    free(copy);
    return argc;
}
char** create_argv(int argc, char* cmd){
    char* copy = malloc(strlen(cmd)+1);
    if(copy==NULL){
        myPrint("malloc failed! abort \n");
        exit(0);
    }
    strcpy(copy,cmd);
    char **argv = malloc((argc+1)*sizeof(char*));
    if(argv==NULL){
        myPrint("malloc failed! abort \n");
        exit(0);
    }
    char* token;
    int index = 0;
    char* rest;
    token = strtok_r(copy,"     \t",&rest);
    while(token){
        char* cpy_token = malloc(strlen(token)+1);
        if(cpy_token == NULL){
            myPrint("malloc failed! abort \n");
        }
        strcpy(cpy_token,token);
        argv[index] = cpy_token;
        token = strtok_r(NULL,"     \t",&rest);
        index++;
    }
    argv[argc] = NULL;
    free(copy);
    return argv;
}
char** combine_argv(int argc1, char** argv1, int argc2, char** argv2){
    int argc = argc1 + argc2;
    char** argv = malloc((argc+1) * sizeof(char*));
    argv[argc] = NULL;
    int index = 0;
    for(int i = 0; i < argc1; i++){
        argv[index] = argv1[i];
        index++;
    }
    for(int j = 0; j < argc2; j++){
        argv[index] = argv2[j];
        index++;
    }
    return argv;
}
void print_argv(char** argv, int argc){
    int i = 0;
    myPrint("printing argv now!!! \n");
    while(argv[i]){
        myPrint(argv[i]);
        myPrint("\n");
        i++;
    }
}
void free_argv(char** argv, int argc){
    for(int i=0;i<argc;i++){
        free(argv[i]);
    }
    free(argv);
    return;
}
int built_in_command_redir(int argc, char* argv[], int fd){
    if (!strcmp(argv[0],"quit")){
        exit(0);
    }
    if(!strcmp(argv[0],"pwd")){
        if(argc!=1){
            printErr();
            return 1;
        }
        int saved_stdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        char buf[PATH_MAX + 1];
        char *ptr;
        ptr = getcwd(buf,PATH_MAX+1);
        myPrint(ptr);
        myPrint("\n");
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        return 1;
    }
    if(!strcmp(argv[0],"cd")){
        if(argc==1){
            chdir(getenv("HOME"));
            return 1;
        }else if(argc==2){
            int chdir_success = chdir(argv[1]);
            if(!chdir_success){
                return 1;
            }else{
                printErr();
                return 1;
            }
        }else if(argc > 2){
            printErr();
            return 1;
        }
    }
    if(!strcmp(argv[0],"echo")){
        if(argc==1){
            int saved_stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            myPrint("\n");
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
            return 1;
        }else if(argc==2){
            int saved_stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            myPrint(argv[1]);
            myPrint("\n");
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
            return 1;
        }else{
            printErr();
            return 1;
        }
    }
    return 0;
}
void eval_one_command(char* cmd, int para){
    int easy_detect_redir_flag = easy_detect_redir(cmd);
    int is_built_in;
    int has_redirection = 0;
    pid_t pid;
    int argc = count_argc(cmd);
    if(argc==0){
        return;
    }
    char** argv = create_argv(argc,cmd);

    if(!easy_detect_redir_flag){
        is_built_in = built_in_command(argc, argv);
        if(is_built_in){
            return;
        }
    }

    if(!strcmp(argv[0],"quit")){
        exit(0);
    }

    int arg_pos = -1;
    int char_pos = -1;
    int change_argc = 0;
    int redir_within = detect_redirection_within_string(argv, argc, &arg_pos, &char_pos);
    if(redir_within){
        argv = create_redir_argv(argv,argc,arg_pos,char_pos,&change_argc);
        argc = argc+change_argc;
    }
    int fd = -1;
    has_redirection = detect_redirection(argv,argc);
    if(has_redirection){
        if(has_redirection == -999){
            printErr();
            return;
        }
        fd = get_fd(has_redirection, argv, argc);
        if(fd==-1){
            printErr();
            return;
        }
    }    
    if((pid=fork())==0){ /* Child runs user job */
        if(has_redirection){
            char** new_argv = malloc(has_redirection * sizeof(char*));
            for(int i=0;i<has_redirection;i++){
                new_argv[i] = argv[i];
            }
            new_argv[has_redirection] = NULL;
            is_built_in = built_in_command_redir(has_redirection, new_argv, fd);
            if(is_built_in){
                exit(0);
            }
            dup2(fd,STDOUT_FILENO);
            if(execvp(new_argv[0],new_argv)<0){
                printErr();
                exit(0);
            }
        }else{
            // myPrint("no redirection, gonna print out the argvs now: \n");
            // print_argv(argv,argc);
            if(execvp(argv[0],argv)<0){
                printErr();
                exit(0);
            }
        }
    }
    if(!para){
        waitpid(pid, NULL,0);
    }
    if(has_redirection){
        close(fd);
    }
    free_argv(argv,argc);
    return;
}

void eval_one_command_pipe(char* cmd, int para){
    /* set ups */
    int pipe_cnt = count_pipe(cmd); // number of pipeline char in the command 
    int argc[pipe_cnt + 1];
    char** argv[pipe_cnt + 1]; // argc and argv corresponding to pipe count
    /* processing through pipe char to create args for each program in pipe */
    char* rest;
    char* token;
    char* cpy = malloc(strlen(cmd)+1);
    strcpy(cpy,cmd);
    token = strtok_r(cpy,"|",&rest);
    int pipe_index=0; //if not equal pipe_cnt at the end, means one of the pipe input is invalid
    while(token){
        argc[pipe_index] = count_argc(token);
        if(argc[pipe_index]==0){
            printErr();
            return;
        }
        argv[pipe_index] = create_argv(argc[pipe_index],token);
        token = strtok_r(NULL, "|", &rest);
        if(token){
            pipe_index++;
        }
    }
    if(pipe_index != (pipe_cnt)){ //number of programs with valid argv must = pipe_cnt+1;
        printErr();
        return;
    }
    /* if any pipe is a built in command, error */ 
    for(int i = 0; i < pipe_index; i++){
        if(check_built_in_pipe(argv[i])){
            printErr();
            return;
        }
    }
    /* debugging for multiple pipes */
    pid_t pid[pipe_cnt+1]; // total number of processes should be pipe_cnt+1;
    int pipe_fds[pipe_cnt][2]; // all the pipes
    int current_process = 0; // there should be pipe_cnt+1 amount of processes, so max process_cnt = pipe_cnt
    int closed = 0; //keep track if you have closed everything already 
    for(int i = 0; i < pipe_cnt; i++){
        pipe(pipe_fds[i]);
    }
    while(current_process <= pipe_cnt){
        // myPrint("executing the while loop: \n");
        pid[current_process] = fork();
        if(pid[current_process]==0){ //child process needs to chain together processes
            if(current_process==0){
                //first program in pipe chain, no need to change input
                dup2(pipe_fds[current_process][1],STDOUT_FILENO); // write end is now STDOUT
                for(int i = 0; i < pipe_cnt; i++){ // close all unnecessary pipe fds
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }
                if(execvp(argv[current_process][0],argv[current_process])<0){
                    printErr();
                    exit(0);
                }
            }else if(current_process==pipe_cnt){
                //last program in pipe chain, no need to change output
                dup2(pipe_fds[pipe_cnt - 1][0],STDIN_FILENO);
                for(int i = 0; i < pipe_cnt; i++){
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }
                if(execvp(argv[current_process][0],argv[current_process])<0){
                    printErr();
                    exit(0);
                }
            }else{
                //program in the middle of chain, need to change both input and output
                dup2(pipe_fds[current_process-1][0],STDIN_FILENO);
                dup2(pipe_fds[current_process][1],STDOUT_FILENO);
                for(int i = 0; i < pipe_cnt; i++){
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }
                if(execvp(argv[current_process][0],argv[current_process])<0){
                    printErr();
                    exit(0);
                }
            }
        }else{ // parent process updates current_process count 
            current_process++;
        }
    }
    //myPrint("pipe executed corretly \n");
    for(int i = 0; i < pipe_cnt; i++){
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }
    //myPrint("now gonna wait \n");
    if(!para){
        waitpid(pid[current_process] , NULL , 0);
    }
 
    for(int i = 0; i < pipe_cnt;i++){
        free_argv(argv[i],argc[i]);
    }
    free(cpy);
    return;
        
}
//&&&&&&&&&&&&&&& Parse multiple jobs &&&&&&&&&&&&& // 
int check_blank(char *pinput){
    char* copy = malloc(strlen(pinput)+1);
    if(copy==NULL){
        myPrint("malloc error!!! \n");
        exit(0);
    }
    strcpy(copy,pinput);
    char* rest;
    char* token = strtok_r(copy," \t\n",&rest);
    if(token==NULL){
        free(copy);
        return 1;
    }else{
        free(copy);
        return 0;
    }
}
void eval_multiple_commands(char *pinput, int parallel){
    // process the command: 
    char* rest;
    char* token;
    pid_t wpid;
    int status = 0;
    if(parallel==1){
        token = strtok_r(pinput, "&", &rest);
    }else{
        token = strtok_r(pinput, ";", &rest);
    }
    while(token){
        char* cpy = malloc(strlen(token)+1);
        strcpy(cpy,token);
        int pipe = detect_pipe(cpy);
        if(pipe == -1){
            // both pipe and redir exists, invalid command;
            printErr();
        }else if(pipe == 1){
            eval_one_command_pipe(cpy, parallel);
        }else{
            eval_one_command(cpy,parallel);
        }
        free(cpy);
        if(parallel==1){
            token = strtok_r(NULL,"&", &rest);
        }else{
            token = strtok_r(NULL, ";", &rest);
        }
    }
    if(parallel==1){
        while ((wpid = wait(&status)) > 0); // waits for all child processes
    }
    return;
}
//&&&&&&&&&&&&&&& Main Shell &&&&&&&&&&&&& // 
void interactive_mode(){
    char cmd_buff[66]; //should be 66 (64 bytes, new line, null termination)
    char *pinput;
    int too_long = 0;
    int para = 0;
    while(1){
        myPrint("mpcs-shell> ");
        pinput = fgets(cmd_buff, 66, stdin); //should be 66
        too_long = input_too_long(pinput);
        para = detect_parallel(pinput);
        if(too_long){
            myPrint("input too long!! \n");
            myPrint(pinput);
            while(pinput[strlen(pinput)-1]!='\n'){
                pinput = fgets(cmd_buff,66,stdin);
                myPrint(pinput);
            }
            printErr();
        }else if(para == -1){
            myPrint(pinput);
            printErr();
        }else{
            char *cpy = malloc(strlen(pinput)+1);
            strcpy(cpy,pinput);
            cpy[strlen(cpy)-1] = ' ';
            eval_multiple_commands(cpy,para);
            free(cpy);
        }
    }
    return;
}
void run_batch_mode(FILE* fp){
    char cmd_buff[66];
    char *pinput;
    int too_long;
    int is_blank = 0;
    int para = 0;
    pinput = fgets(cmd_buff,66,fp);
    while(pinput!=NULL){
        para = detect_parallel(pinput);
        too_long = input_too_long(pinput);
        if(too_long || (para==-1)){
            myPrint(pinput);
            while(pinput[strlen(pinput)-1]!='\n'){
                pinput = fgets(cmd_buff,66,fp);
                myPrint(pinput);
            }
            printErr();
        }else if(para == -1){
            myPrint(pinput);
            printErr();
        }else{
            myPrint(pinput);
            is_blank = check_blank(pinput);
            if(!is_blank){
                char* cpy = malloc(strlen(pinput)+1);
                strcpy(cpy, pinput);
                cpy[strlen(cpy)-1] = ' ';
                eval_multiple_commands(cpy,para);
                free(cpy);
            }
        }
        pinput = fgets(cmd_buff,66,fp);
    }
}

int main(int argc, char *argv[]) 
{   
    int batch_mode;
    char * file_path;
    FILE *fp;
    if(argc>2){
        printErr();
        exit(1);
    }else if(argc==2){
        batch_mode = 1;
    }else if(argc==1){
        batch_mode = 0;
    }
    if(!batch_mode){
        interactive_mode();
    }
    if(batch_mode){
        file_path = argv[1];
        fp = fopen(file_path, "r");
        if(fp==NULL){
            printErr();
            exit(1);
        }
        // just_read_file(fp);
        run_batch_mode(fp);
    }
    return 0;
}
//this is the final version!
    
