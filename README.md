# client_server_rotx
Scripts implement a client and server that communicate with each other using an encryption technique known as RotX.

The scritps can be compiler, respectively, with the follow commands: 

Server : `gcc -o server server.c -lpthread`
Client : `gcc -o client client.c`

To execute, follow an exemple: 

Server : `./server 5152` <br/>
Client : `./client 127.0.0.1 5152 yourmessage 2`


The Server param is the port that the server will listener;
The Client parameters are, respectively, Server IP, Server Port, a message and the key of cipher ceaser. 

The message must contain just letters [a-z] in lowercase. 
