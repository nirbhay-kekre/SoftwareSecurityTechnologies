# SoftwareSecurity

## Assignment 1
* The server process is divided into two processes, one which process the request from client and the other talks over the network. Current implementation drops the privileges of the process which talks over the network to client.

* Changes in server.c
```c
pipe(pipe_fd);
switch(fork_pid = fork()){
    case -1:
        //fork failed
        ...
        exit(1);
    case 0:
        // child process: this process talks over the network in reduced privilege
        chroot("/var/empty"); //changing root to /var/empty
        close(pipe_fd[0]); //closing read end of the pipe in child process
        ....
        //code to create and bind socket.
        // we need to be root here to bind socket to port 80
        ....
        struct passwd *pwd = getpwnam(nobody) // retrieving the user and group id of nobody user
        //dropping group privileges 
        setgid(pwd->pw_gid);
        //dropping  user privileges
        setuid(pwd->pw_uid);
        ....
        // reading message from socket
        // writing to the pipe, to send the message to parent to process.
        break;
    default:
        // parent process : processes data provided by child
        close(pipe_fd[1]); //closing write end of the pipe in parent
        //reading data from child via pipe
        valread = read(pipe_fd[0], buffer, 1024);
        // processing data from child
        // waiting for child to finish
}
```

* how to run server:<br/>
```shell
$ gcc -o server server.c
$ sudo ./server

```
* how to run client:<br/>
```shell
$ gcc -o client client.c
$ sudo ./client

```

## Assignment 2
* Assignment two is build by keeping the code from assignment one as the start point. The child process which talks over the network via socket exec's itself and passes the socket file discriptor as an argument.

```c
char *args[]= { 
    (char *)argv[0], // process name to be exec-ed, argv[0] since we are self exec-ing 
    (char *) &pipe_fd[1], // passing pipe file discriptor, so that we can notify parent to process the message received from client.
    (char *) &server_fd, // passing server/socket file discriptor
    (char *)&address, // passing sockaddr_in
    (char *) NULL // END OF Arguments
  };
execv(args[0], args);
```
* how to run server:<br/>
```shell
$ gcc -o server server.c
$ sudo ./server
```
* how to run client:<br/>
```shell
$ gcc -o client client.c
$ sudo ./client
```
