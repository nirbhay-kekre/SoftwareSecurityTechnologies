// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pwd.h>
#define PORT 80 
int main(int argc, char const *argv[]) 
{ 
    int pipe_fd[2];
    pid_t fork_pid;
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
    const char *nobody = "nobody";
    // creating unidirectional pipe between parent and child 
    pipe(pipe_fd);
    switch(fork_pid = fork()){
    case -1:
        //fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    case 0:
        // child process: this process talks over the network in reduced privilege
        //changing root to /var/empty
        chroot("/var/empty");
        close(pipe_fd[0]); //closing read end of the pipe in child process
        // Creating socket file descriptor 
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        { 
            perror("socket failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        // Forcefully attaching socket to the port 80 
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                    &opt, sizeof(opt))) 
        { 
            perror("setsockopt"); 
            exit(EXIT_FAILURE); 
        } 
        address.sin_family = AF_INET; 
        address.sin_addr.s_addr = INADDR_ANY; 
        address.sin_port = htons( PORT ); 
        
        // Forcefully attaching socket to the port 80 
        if (bind(server_fd, (struct sockaddr *)&address,  
                                    sizeof(address))<0) 
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 
        //dropping privileges after bind since we require root privileges for binding to port 80
        struct passwd *pwd;
        // retrieving user information for nobody user
        if( (pwd = getpwnam(nobody)) == NULL){
            perror("getpawnam failed for nobody user"); 
            exit(EXIT_FAILURE); 
        }
        //dropping group privileges
        if(setgid(pwd->pw_gid) != 0){
            //setgid failed
            perror("set group id failed on dropping group privilege"); 
            exit(EXIT_FAILURE);
        }
        //dropping user privileges
        if(setuid(pwd->pw_uid) != 0){
            //setuid failed
            perror("set uid failed on dropping user privilege"); 
            exit(EXIT_FAILURE); 
        }
        
        if (listen(server_fd, 3) < 0) 
        { 
            perror("listen"); 
            exit(EXIT_FAILURE); 
        } 
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        valread = read( new_socket , buffer, 1024); 
        //providing data to parent via pipe 
        write(pipe_fd[1], buffer , 1024);
        send(new_socket , hello , strlen(hello) , 0 ); 
        printf("Hello message sent\n"); 
        break;
    default:
        // parent process : processes data provided by child
        close(pipe_fd[1]); //closing write end of the pipe in parent
        //reading data from child via pipe
        valread = read(pipe_fd[0], buffer, 1024);
        //processing data from child
        printf("%s\n",buffer ); 
        int child_exit_status;
        //waiting for child to finish processing
        waitpid(fork_pid, &child_exit_status, 0);
    }
    return 0; 
} 
