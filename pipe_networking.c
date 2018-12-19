#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  printf("[server] making well known pipe\n");
  int pipe = mkfifo("mario", 0666);
  if(pipe){
    printf("[server] pipe made\n");
  }
  else{
    printf("pipe failed\n");
    exit(0);
  }

  //recieves message from client
  int from_client = open("mario", O_RDONLY);
  char msg[HANDSHAKE_BUFFER_SIZE] ;
  read(from_client,msg,HANDSHAKE_BUFFER_SIZE);
  printf("[server] received msg from client\n");
  printf("[server] removing well known pipe\n");
  remove("mario");

  //acknowledges client
  * to_client = open(msg,O_WRONLY);
  write(*to_client,ACK, sizeof(ACK));

  //waits for final response
  char response[HANDSHAKE_BUFFER_SIZE];
  read(from_client,response,sizeof(response));
  printf("[server] recieved response\n");

  return from_client;

}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char msg[HANDSHAKE_BUFFER_SIZE];
  * to_server = open("mario",O_WRONLY);
  if(to_server){
    printf("[client] connected to wkp\n");
  }
  else{
    printf("[client] failed to connect\n");
    exit(0);
  }
  //writes initial message to server
  printf("[client] wrote to server\n");
  write(*to_server,msg,sizeof(msg));

  int private = mkfifo("luigi",0666);
  if(private){
    printf("[client] made private fifo\n");
  }
  else{
    printf("[client] failed to make private fifo\n");
    exit(0);
  }

  //waits for response
  char response[HANDSHAKE_BUFFER_SIZE];
  read(private,response,sizeof(response));
  printf("[client] recieved response\n");
  //writes final response
  write(*to_server,ACK,sizeof(ACK));
  remove("private");

  return private;
}
