# SoftwareSecurity

## Assignment 1
* The server process is divided into two processes, one process the request from client and the other talks over the network. Current implementation drops the privileges of the process which processes the request from  client.

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
