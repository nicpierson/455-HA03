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
int sfd;
int n;
char *temp;


int main(int argc, char *argv[], char *env[])
{
    struct sockaddr_in saddr;
    char *buf[1];
    char *tbuf[1];
    FILE *fptr;
    FILE *putptr;
    FILE *getptr;
    char mbuf[MAX];
    
    sfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sfd < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }
    
    
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    saddr.sin_port = htons(PORT); 
  
    
    if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 


    while(1){
        printf("Enter Message: ");
        bzero(line, MAX);
        fgets(line, MAX, stdin);         

        line[strlen(line)-1] = 0;        // discard \n at end
        if (line[0]==0)                  
            exit(0);


        temp = malloc(sizeof(line));
        memcpy(temp, line, sizeof(line));

        buf[0] = strtok(line, " ");
        buf[1] = strtok(NULL, " ");
        // printf("temp = %s\nbuf[0] = %s\nbuf[1] = %s\n",temp, buf[0], buf[1]);            

        if (strcmp(temp, "exit") == 0)
        {
            exit(1);
        }
        if (strcmp(buf[0], "givefile") == 0)
        {
            putptr = fopen(buf[1], "r");
            if (putptr == NULL)
            {
                printf("File doesn't exist, couldn't execute command\n");
                continue;
            }
            else
            {
                n = write(sfd, temp, MAX);
                
                
                
                while (fgets(line, MAX, putptr))
                {
                    write(sfd, line, MAX);
                }
                fclose(putptr);
                strcpy(mbuf, "chill");
                write(sfd, mbuf, MAX);
                printf("File Succesfully Transfered to Server\n");
            }   
            
        }
        
        else{
            n = write(sfd, temp, MAX);
            
        }
        n = read(sfd, ans, MAX);
        temp = malloc(sizeof(ans));
        memcpy(temp, ans, sizeof(ans));
        tbuf[0] = strtok(ans, " ");
        tbuf[1] = strtok(NULL, " ");

        if (strcmp(tbuf[0], "givefile") == 0)
        {
            getptr = fopen(tbuf[1],"w+");
            n = read(sfd, ans, MAX);
            while(strcmp(ans, "chill") != 0)
            {
                //printf("%s", ans);
                fprintf(getptr, "%s", ans);
                //bzero(ans, MAX);
                n = read(sfd, ans, MAX);
            }
            fclose(getptr);
            printf("Client successfully obtained file from server\n");
            
        }
        else{
            printf("From Server: %s\n", temp);
        }

       
        
        

    }

    return 1;
}