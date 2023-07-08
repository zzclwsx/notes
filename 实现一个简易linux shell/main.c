#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<pwd.h>
#include<string.h>
void PrintName()
{
    struct passwd* pass;
    pass = getpwuid(getuid());
    printf("\e[1;32m""[%s@",pass->pw_name);

    char name[128];
    gethostname(name,sizeof(name)-1);
    printf("%s",name);

    char pwd[128];
    getcwd(pwd,sizeof(pwd)-1);
    int len = strlen(pwd);
    //char* p = pwd+len-1;
    //while(*p != '/' && len--)
    //{
    //    p--;
    //}
    //p++;
    printf(" %s]@""\e[0m",pwd);
}

int ispipe(char* command[]){
    int pos = 0;
    while(command[pos] != NULL){
        if (strcmp(command[pos],"|") == 0)
            return pos+1;
        pos++;
    }
    return 0;
}

int main()
{
    printf("\e[1;32m""here is 202100202062's shell\n");
    printf("  .   .  \n");
    printf("    ^\n""\e[0m");
    printf("type \"help\" for help\n");

    while(1)
    {
        PrintName();
        fflush(stdout);
        //读取字符串
        char buf[1024];
        int s = read(0,buf,1024);
        if(s > 0)//有读取到字符
        {
            int i = 0;
            for( i = 0; i < s; ++i)
            {
                if(buf[i] == '\b' && i >= 1)
                {
                    int j = 0;
                    for( j = i+1; j < s; ++j)
                    {
                        buf[j-2] = buf[j];
                    }
                    buf[s-2] = 0;
                    i-=1;
                }
                else if(buf[i] == '\b' && i == 0)
                {
                    int j = 0;
                    for( j = 1; j < s; ++j)
                    {
                        buf[j-1] = buf[j];
                    }
                    buf[s-1] = 0;
                }
                else
                {
                    continue;
                }
            }
            buf[s] = 0;
        }
        else
        {
            continue;
        }
        //将读取到的字符串分成多个字符串
        char* start = buf;
        int i =1;
        char* command[10] = {0};
        command[0] = start;
        while(*start)
        {
            if(isspace(*start))
            {
                *start = 0;
                start++;
                command[i++] = start;
            }
            else
            {
                ++start;
            }
        }
        command[i-1] = NULL;

        //打印一下字符串信息
        int m = 0;
        for(m = 0; m <i-1;++m)
        {
            printf("receive command%d:%s\n",m,command[m]);
        }
        //fork新的进程
        if (strcmp(command[0],"exit") == 0){
            exit(EXIT_SUCCESS);
        }else if (strcmp(command[0],"help") == 0){
            printf("This is a shell\nYou can type \"exit\" and press Enter to exit\n");
        }else if (strcmp(command[0],"cd") == 0){
            if (command[2] != NULL){
                printf("\e[1;31m""ERROR:Received More Than One Path\n""\e[0m");
            }
            else if(chdir(command[1])<0){
                perror("error");
            }
        }else if (ispipe(command)){
            printf("Is pipe command!\n");
            int pos = ispipe(command);
            char *argv1[10],*argv2[10];
            int len1 = 0,len2 = 0;
            int i = 0;
            while (strcmp(command[i],"|") != 0){
                argv1[len1++] = command[i++];
            }
            argv1[len1] = NULL;
            int t = pos;
            while (command[t] != NULL){
                argv2[len2++] = command[t++];
            }

            argv2[len2] = NULL;

            //printf("1");
            //getchar();
            int id = fork();
            if (id == -1){
                perror("fork:");
            }
            else if (id == 0){
                int p[2];
                int ret = pipe(p);
                if (ret == 0)   printf("create pipe\n");
                //getchar();

                if (fork() == 0){
                    close(p[0]);
                    dup2(p[1],1);

                    execvp(argv1[0],argv1);
                }
                wait(NULL);



                close(p[1]);
                dup2(p[0],0);

                execvp(argv2[0],argv2);
                perror("pipe_error");
                exit(0);


            }else{
                wait(NULL);
            }
        }else{
            int  id = fork();
            if(id == 0)
            {
                //child,执行替换操作
                execvp(command[0],command);
                perror("error");
                exit(1);
            }
            else
            {

                wait(NULL);
            }
        }
    }
    return 0;
}