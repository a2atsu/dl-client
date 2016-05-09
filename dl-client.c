#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* read(),write(),fork(),close(),...       */
#include <sys/socket.h>  /* socket(), bind(), listen(), accept()... */
#include <netinet/in.h>  /* struct sockaddr_in,...                  */
#include <arpa/inet.h>   /* inet_ntoa(),....                        */
#include <sys/un.h>      /* struct sockaddr_un                      */
#include <fcntl.h>       /* open(),...                              */
#include <netdb.h>       /* gethostbyname(),....                    */
#include <sys/types.h>

#include <syslog.h>

#define PORT_NO (1235)   /* default port number for INET domain */

static int copy_client(char* server,  char* fn , int port)
{
  int    sock;
  char   buf[BUFSIZ] = {};
  ssize_t    rc;
  ssize_t write_size = 0;
  ssize_t ret = 0;
  ssize_t read_size = 0;
  int retval;
  FILE *fp;
  int fd;
  int i = 0;

  sock = client_socket_procedure(server,port);

  strcpy(buf, fn);
  write(sock, buf, strlen(buf)); //send filename
  printf("sent filename as %s\n", buf);

  fd = open("copied_file",O_CREAT | O_WRONLY | O_TRUNC, 0666);
  
/*
    fp = fdopen(fd, "wb");
*/
    
  memset((char*)&buf, 0, sizeof(buf));

  while(1){
    rc = read(sock, buf, sizeof(buf));
    if ( rc <= 0 ){
      break;
    }
    printf("readcount: %zd sizeofBuf:%zd\n",rc,sizeof(buf));
//    ret = write(fd, buf, rc*sizeof(buf));
    ret = write(fd, buf, rc);
    i++; 
    printf("%d: Read: %lu \t Write: %lu\n",i, rc, ret);
    write_size += ret;
    read_size += rc;
   }
//  fclose(fp);
  printf("Done Reading\n");
   close(fd);
  printf("\n------\nRead: %ld\n",read_size);
  printf("\n------\nWrite: %zd\n",write_size);

  close(sock);

  return 0;
}

int client_socket_procedure(char* server,int port) //Make client socket
{
  int sock;
  struct sockaddr_in  addr;
  struct hostent*     hp;

  if( (sock=socket(AF_INET, SOCK_STREAM, 0)) <0 ) {
    perror("socket");
    exit(1);
  }

  memset((char*)&addr, 0, sizeof(addr));

  if( (hp=gethostbyname(server))==NULL) {
    perror("getaddrinfo");
    exit(1);
  }

  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
    perror("connect");
    exit(1);
  }

  return sock;
}

//Copy remote file.
int main(int argc,char *argv[])
{
  int port = PORT_NO;
  if( argc < 3 ) {
    printf("Usage: %s SERVER FILENAME [PORT]\n",argv[0]);
    return 1;
  }

  if( argc < 4) {
    port = PORT_NO;
  } else {
    port = atoi(argv[3]);
  }
  printf("port no. = %d\n", port);
  return   copy_client(argv[1], argv[2], port);
}

