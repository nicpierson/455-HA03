#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <time.h>


#define MAX 256
#define PORT 1234

char line[MAX];
char ans[MAX];
int cfd;
int n;
char *temp;

int main(int argc, char *argv[], char *env[])
{
    int sfd, len; 
    struct sockaddr_in saddr, caddr; 
    char *buf[1];
    char *tbuf[1];
    char mbuf[MAX];
    FILE *putptr;
    FILE *getptr;

    sfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sfd < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }

    
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    //saddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    saddr.sin_port = htons(PORT);
    
    
    if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0) { 
        printf("socket bind failed\n"); 
        exit(0); 
    }
      
    // Now server is ready to listen and verification 
    if ((listen(sfd, 5)) != 0) { 
        printf("Listen failed\n"); 
        exit(0); 
    }
    chroot(getcwd(line, MAX));

    while(1){
        len = sizeof(caddr);
        cfd = accept(sfd, (struct sockaddr *)&caddr, &len);
        if (cfd < 0){
            printf("server: accept error\n");
            exit(1);
        }
        while(1){
            bzero(line, MAX);
            n = read(cfd, line, MAX);
            if (n==0){
                printf("server: client died, server loops\n");
                close(cfd);
                exit(1);
                break;
            }
            temp = malloc(sizeof(line));
            memcpy(temp, line, sizeof(line));
            buf[0] = strtok(line, " ");
            buf[1] = strtok(NULL, " ");
            
            

            if (strcmp(buf[0], "givefile") == 0)
            {
                putptr = fopen(buf[1],"w+");
                n = read(cfd, ans, MAX);
                while(strcmp(ans, "chill") != 0)
                {
                    //printf("%s", ans);
                    fprintf(putptr, "%s", ans);
                    //bzero(ans, MAX);
                    n = read(cfd, ans, MAX);
                }
                fclose(putptr);
                printf("Server obtained file from client\n");
            }
            else{
                printf("From Client: %s\n",temp);
            }

            printf("Enter Message: ");
            bzero(line, MAX);
            fgets(line, MAX, stdin);         

            line[strlen(line)-1] = 0;        // discard \n at end
            if (line[0]==0)                  
                exit(0);

            temp = malloc(sizeof(line));
            memcpy(temp, line, sizeof(line));

            tbuf[0] = strtok(line, " ");
            tbuf[1] = strtok(NULL, " ");

            if (strcmp(temp, "exit") == 0)
            {
                exit(1);
            }
;
            char *sah = tbuf[1];
            

            if(strcmp(tbuf[0], "givefile") ==0)
            {
                
                getptr = fopen(tbuf[1], "r"); 

                if (getptr == NULL)
                {
                printf("File doesn't exist, couldn't execute  command\n");
                continue;
                printf("server couldn't find the file %s \n", tbuf[1]);
                }
                else
                {
                    n = write(cfd, temp, MAX);
                    while (fgets(line, MAX, getptr))
                    {
                        write(cfd, line, MAX);
                    }
                    fclose(getptr);
                    strcpy(mbuf, "chill");
                    write(cfd, mbuf, MAX);
                    //printf("test = %s\n\n\n", sah);
                    printf("File Successfully Transfered to Client\n");
                }
            }
            else{
                n = write(cfd, temp, MAX);

            }
        }

    }


    return 1;
}