#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <time.h>
#include "list.h"
#include <pthread.h> // thread
#include <sys/types.h> // socket
#include <sys/socket.h> // socket
#include <arpa/inet.h>
#include <unistd.h> 
#include <netdb.h> // gai_strerror

struct timeval timer;

List *user_message; // message buffer
List *get_message;

char *onlineStatus = "off";

struct addrinfo local, remote;
struct addrinfo *remoteInfo, *localData;  // pointer to result

int fd;
// pthread_mutex_t lock;
pthread_mutex_t m1;
pthread_mutex_t m2;

pthread_cond_t inputCond;
pthread_cond_t sendCond;
// pthread_cond_t statusCond;

pthread_cond_t receiveCond;
pthread_cond_t printCond;

bool on = true;
int checkOnline = 0;

int available = 1;
int available2 = 1;

int crpytKey = 2;


// encrypt message
void encrypt(char crpyt[4000]){
    for(int i = 0; crpyt[i] != '\0'; i++){
        crpyt[i] = (crpyt[i] + 2) % 256;
    }
}

// decrpyt message
void decrypt(char crpyt[4000]){
    for(int i = 0; crpyt[i]  != '\0'; i++){
        crpyt[i] = (crpyt[i] - 2) % 256;
    }
}

void *user_input(void *ptr) {
  printf("Welcome to Lets-Talk! Please type your message now.\n");
  char buff[4000];
  char *arr[4000];
  int pos;

  while(on) {
    pthread_mutex_lock(&m1);
    while (available == 0) {
      pthread_cond_wait(&inputCond, &m1);   
    }
    /* critical section */
    available = 0;
    memset(buff, '\0', 4000);
    if (fgets(buff, 4000, stdin) == NULL) {
      return EXIT_SUCCESS;
    }

    char * token = strtok(buff, "\n");   //delimiter: \n
    pos = 0;
    while (token != NULL) {
      arr[pos++] = token;         //assigns and increments i
      List_add(user_message, arr[pos-1]);
      token = strtok(NULL, "\n");
    } 
    pthread_mutex_unlock(&m1);
    pthread_cond_signal(&sendCond);
    }
  pthread_exit(NULL);
}

void *sendMessage(void *ptr) {
    char buff2[4000];
    int count = 0;
    
  while (on) {
    // lock thread and wait for signal
    pthread_mutex_lock(&m1);
    while (available == 1) {
    // unlocks mutex while it waits
    pthread_cond_wait(&sendCond, &m1);        
    }

    available = 1;
    int len = List_count(user_message);
    memset(buff2, '\0', 4000);
    count = 0;

    while(count != len) {
      strcpy(buff2, List_first(user_message));
      List_remove(user_message);
      if (strcmp("!exit", buff2) == 0) { // terminate thread
        encrypt(buff2);
        sendto(fd, buff2, 4000, 0, remoteInfo->ai_addr, remoteInfo->ai_addrlen);
        exit(0);
      }
      encrypt(buff2);
      // send message to remote socket
      sendto(fd, buff2, 4000, 0, remoteInfo->ai_addr, remoteInfo->ai_addrlen);
      count++;      
    }
    pthread_mutex_unlock(&m1);
    pthread_cond_signal(&inputCond);
  }

  pthread_exit(NULL);
}

void *receiveMessage(void *ptr) {
  
  while(on) {
    pthread_mutex_lock(&m2);
    while (available2 == 0) {
      pthread_cond_wait(&receiveCond, &m2);   
    }

    while (available2 == 1) {
      char buff3[4000];
      memset(buff3, '\0', 4000);
      // get message from socket
      recvfrom(fd, buff3, 4000, 0, remoteInfo->ai_addr, &remoteInfo->ai_addrlen);
      fflush(stdout); // fflush after receiving message
      decrypt(buff3);
      // check for command input
      if (strcmp(buff3, "!status") == 0) {
        onlineStatus = "on";
        // send message encrypted qp = "on" back
        sendto(fd, "qp", 2, 0, remoteInfo->ai_addr, remoteInfo->ai_addrlen);
      } 

      else if(strcmp("on", buff3) == 0) {
        printf("Online\n"); // requester status outputted
        onlineStatus = "on";
      } else if (strcmp("!exit", buff3) == 0) {
        printf("!exit\n");
        exit(0);
      }

      else {
        // print user text input
        List_add(get_message, buff3);
        available2 = 0;
        pthread_mutex_unlock(&m2);
        pthread_cond_signal(&printCond);
      }
    }
  }
  pthread_exit(NULL);
}

void *printMessage(void *ptr) {
  while (on) {
    pthread_mutex_lock(&m2);
    while (available2 == 1) {
    // unlocks mutex while it waits
    pthread_cond_wait(&printCond, &m2);        
    }

    /* critical section */
    available2 = 1;

    char buff4[4000];
    memset(buff4, '\0', 4000);

    // print user text input
    while (List_count(get_message) > 0) {
      strcpy(buff4, List_first(get_message));
      printf("%s\n", buff4);       
      List_remove(get_message);
    }
    pthread_mutex_unlock(&m2);
    pthread_cond_signal(&receiveCond);
  }
  pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{

  if (argv[3] == NULL || argc != 4) {
    printf("need to provide executable, localport #, localhost, remoteport #\n");
    exit(1);
  }

  user_message = List_create(); // list of user input to be sent
  get_message = List_create(); // receiving input datagram list

  // create local socket/local address
  memset(&local, 0, sizeof(local)); // ensure empty struct
  local.ai_family = AF_INET;
  local.ai_socktype = SOCK_DGRAM; // udp datagram-based protocol

  // null, port #, local includes filled info, result
  // structure values used for socket. error checking for getaddrinfo()
  int localRes = getaddrinfo(NULL, argv[1], &local, &localData);
  if (localRes != 0) {
      fprintf(stderr, "error in getaddrinfo() %s\n", gai_strerror(localRes));
      exit(1);
  }

  struct addrinfo *k = localData;
    // socket() used for system calls. error checking for socket()  
  if((fd = socket(k->ai_family, k->ai_socktype, k->ai_protocol)) < 0) {
      perror("failure to create socket");
      exit(EXIT_FAILURE);
  }

  // bind socket to host program port
  if (bind(fd, k->ai_addr, k->ai_addrlen) < 0) {
    perror("failure to bind socket");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // free linked-list when done with list
  // freeaddrinfo(localData);

  // address destination
  memset(&remote, 0, sizeof(remote)); // ensure empty struct
  remote.ai_family = AF_INET;
  remote.ai_socktype = SOCK_DGRAM;

  // contains the (IP address, port #, pointer to struct addrinfo, pointer to linked-list of results)
  int rInfo = getaddrinfo(argv[2], argv[3], &remote, &remoteInfo);
  if (rInfo != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rInfo));
      exit(1);
  }

  // freeaddrinfo(remoteInfo); // free linked-list when done with list

  pthread_t tr1, tr2, tr3, tr4;

  // initialize mutex & sendCond variable objects
  // pthread_mutex_init(&lock, NULL);
  // pthread_cond_init(&statusCond, NULL);

  pthread_mutex_init(&m1, NULL);
  pthread_cond_init (&sendCond, NULL);
  pthread_cond_init(&inputCond, NULL);

  pthread_mutex_init(&m2, NULL);
  pthread_cond_init (&receiveCond, NULL);
  pthread_cond_init(&printCond, NULL);

  // create 4 threads each of which will execute a function
  pthread_create(&tr1, NULL, user_input, NULL); // user input
  pthread_create(&tr2, NULL, sendMessage, NULL); // udp send
  pthread_create(&tr3, NULL, receiveMessage, NULL);
  pthread_create(&tr4, NULL, printMessage, NULL);

  // wait for threads to finish
  pthread_join(tr1,NULL);
  pthread_join(tr2,NULL);
  pthread_join(tr3,NULL);
  pthread_join(tr4, NULL);

  // clean up
  // pthread_mutex_destroy(&lock);
  pthread_mutex_destroy(&m1);
  // pthread_cond_destroy(&statusCond);
  pthread_cond_destroy(&sendCond);
  pthread_cond_destroy(&inputCond);
  pthread_mutex_destroy(&m2);
  pthread_cond_destroy(&receiveCond);
  pthread_cond_destroy(&sendCond);

  close(fd);

  pthread_exit(NULL);
}

