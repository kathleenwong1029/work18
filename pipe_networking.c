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
  if(pipe==-1){
    printf("pipe failed\n");
    exit(0);
  }
  else{
    printf("[server] pipe made\n");
  }

  //recieves message from client
  int from_client = open("mario", O_RDONLY);
  char msg[HANDSHAKE_BUFFER_SIZE] ;
  read(from_client,msg,HANDSHAKE_BUFFER_SIZE);
  printf("[server] received msg from client:%s\n",msg);
  printf("[server] removing well known pipe\n");
  remove("mario");

  //acknowledges client
  * to_client = open(msg,O_WRONLY);
  write(*to_client,ACK, sizeof(ACK));

  //waits for final response
  char response[HANDSHAKE_BUFFER_SIZE];
  read(from_client,response,sizeof(response));
  printf("[server] recieved response:%s\n",response);

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
  int pid = getpid();
  char msg[HANDSHAKE_BUFFER_SIZE];
  sprintf(msg,"%d",pid);
  * to_server = open("mario",O_WRONLY);
  if(*to_server==-1){
    printf("[client] failed to connect\n");
    exit(0);
    printf("[client] connected to wkp\n");
  }
  else{
    printf("[client] connected to wkp\n");
  }
  //writes initial message to server
  printf("[client] wrote to server:%s\n",msg);
  write(*to_server,msg,sizeof(msg));

  int private = mkfifo(msg,0666);
  if(private==-1){
    printf("[client] failed to make private fifo\n");
    exit(0);
  }
  else{
    printf("[client] made private fifo\n");
  }

  int fd = open(msg,O_RDONLY);
  //waits for response
  char response[HANDSHAKE_BUFFER_SIZE];
  read(fd,response,sizeof(response));
  printf("[client] recieved response:%s\n",response);
  //writes final response
  write(*to_server,ACK,sizeof(ACK));
  remove(msg);

  return fd;
}
