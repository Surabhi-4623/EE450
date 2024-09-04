#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define ServerS_PORT 41833
#define SERVERM_IP "127.0.0.1"
#define SERVERM_PORT 44833

#define FILENAME "./science.txt"
#define MAX_BOOKS 100

struct BookStatus {
    char code[20];
    int available;
};
struct BookStatus bookStatuses[MAX_BOOKS];
int bookCount=0;

void readAndSendBookStatus() {
    //read the science file
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
      }
     int index = 0;
     while (fscanf(file, " %19[^,], %d", bookStatuses[index].code, &bookStatuses[index].available) == 2) {
        bookCount++;
        // Increment the index to get the next book status
        index++;

        // the maximum number of books
        if (index >= MAX_BOOKS) {
            fprintf(stderr, "Maximum number of books exceeded\n");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}


int main() {
    // Create a UDP socket(referred Beej)
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }
 // Set up server address for ServerS(referred Beej)
    struct sockaddr_in serverSAddress;
    serverSAddress.sin_family = AF_INET;
    serverSAddress.sin_addr.s_addr = INADDR_ANY;
    serverSAddress.sin_port = htons(ServerS_PORT);

    // Bind the socket to the specified address and port for ServerS(referred Beej)
    if (bind(udpSocket, (struct sockaddr *)&serverSAddress, sizeof(serverSAddress)) == -1) {
        perror("Error binding socket");
        close(udpSocket);
        exit(EXIT_FAILURE);
    }
    
    printf("ServerS is up and running using UDP on port %d\n", ServerS_PORT);

    // Receive and handle incoming requests from ServerM
    char userBookcode[1024];
    char user[1024];
    struct sockaddr_in serverMAddress;
    socklen_t serverMAddressLength = sizeof(serverMAddress);
    
    //read the file
    readAndSendBookStatus();
   // printf("Book count:%d\n",bookCount);
   
    while (1) {
         //Receive data from serverM(referred Beej)
        ssize_t bytesReceived = recvfrom(udpSocket, userBookcode, sizeof(userBookcode) - 1, 0,
                                         (struct sockaddr *)&serverMAddress, &serverMAddressLength);
                                         
      if (bytesReceived == -1) {
            perror("Error receiving data");
        } else {  
            userBookcode[bytesReceived] = '\0';
            printf("ServerS received %s code from the main server\n", userBookcode);
             char truncatedCode[5];//For some reason special characters are getting appended so using truncate
             strncpy(truncatedCode, userBookcode, 4);
             truncatedCode[4] = '\0';

        int f=0;//flag
        char message[1024];
         ssize_t bytesReceivedUser = recvfrom(udpSocket, user, sizeof(user) - 1, 0,
                                         (struct sockaddr *)&serverMAddress, &serverMAddressLength);
          user[bytesReceivedUser]='\0';
     
       for (int i = 0; i < bookCount; i++) {
       //check if book code is present in the file
        if(strcmp(truncatedCode,bookStatuses[i].code)==0){
         f=1;
         //if present check if the count is >0
        if(bookStatuses[i].available>0){
           printf("The requested book %s is available\n",truncatedCode);
           if(strcmp(user,"admin")==0){
           sprintf(message, "The requested book %s is available and the count : %d\n", truncatedCode,bookStatuses[i].available);
           }
           else{
           sprintf(message,"The requested book is available");
           }
         }
        else{
         printf("The requested book %s not available\n",truncatedCode);
         sprintf(message, "The requested book %s not available\n", truncatedCode);
    }
    //send the data to serverM(referred beej)
    sendto(udpSocket, message, strlen(message), 0, (struct sockaddr*)&serverMAddress, serverMAddressLength);
    printf("ServerS finished sending the availability status of code %s to the main server using UDP on port %d",truncatedCode,ServerS_PORT);
    break;
      }
    }
      
   
   if(!f){
     printf("The requested book %s doesn't exist\n", truncatedCode);

    //Send message to ServerM telling that book doesn't exist
    char message[1024];
    sprintf(message, "The requested book %s is doesn't exist\n", truncatedCode);
    sendto(udpSocket, message, strlen(message), 0, (struct sockaddr*)&serverMAddress, serverMAddressLength);
    }  
   }
  }

    // Close the socket
    close(udpSocket);

    return 0;
}
    
