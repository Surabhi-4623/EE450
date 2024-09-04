#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <ctype.h>
#define SERVER_PORT 45833
#define SERVER_UDP_PORT 44833
#define ServerS_PORT 41833
#define ServerL_PORT 42833
#define ServerH_PORT 43833
#define BACKEND_IP "127.0.0.1"
#define MAX_USERS 30
#define membersFile "./member.txt"

struct user {
    char usernames[20];
    char passwords[20];
};
struct user users[MAX_USERS];

//Function to validate the username and password
void validateUser(char *username, char *password, char msg[20]) {
    FILE *file = fopen(membersFile, "r");
    
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    printf("Main Server loaded the member list\n");

    int index = 0;
    bool userFound = false;

    while (fscanf(file, "%19[^,],%19s ", users[index].usernames, users[index].passwords) == 2)  {
        if (strcmp(username, users[index].usernames) == 0 && strcmp(password, users[index].passwords) == 0) {
            userFound = true;
            strcpy(msg, "Authentication successful");
            printf("Username and password match. Authentication successful\n");
            fclose(file);
            return;
        }
        index++;
    }
    fclose(file);

    if (!userFound) {
        strcpy(msg, "Authentication failed: Username not found");
        printf("%s not registered\n", username);
    } else {
        strcpy(msg, "Authentication failed: Password does not match");
        printf("Password %s doesn't match the username\n", password);
    }
}

void forwardRequest(char* bookcode,char msg[20],char decryptUsername[20]){
// Extract the prefix from the book code
    char prefix = bookcode[0];

    // Create a UDP socket for forwarding requests
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }
 // Set up the address for the destination server based on the prefix
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    char backServer;
    int portNo;
     // Set the IP address and port based on the prefix
    switch (prefix) {
        case 'S':
          //  printf("S\n");
            inet_pton(AF_INET, BACKEND_IP, &serverAddress.sin_addr);
            serverAddress.sin_port = htons(ServerS_PORT);
            backServer='S';
            portNo=ServerS_PORT;
            break;
        case 'L':
       // printf("L\n");
            inet_pton(AF_INET, BACKEND_IP, &serverAddress.sin_addr);
            serverAddress.sin_port = htons(ServerL_PORT);
            backServer='L';
            portNo=ServerL_PORT;
           break;
        case 'H':
        //printf("H\n");
            inet_pton(AF_INET, BACKEND_IP, &serverAddress.sin_addr);
            serverAddress.sin_port = htons(ServerH_PORT);
            backServer='H';
            portNo=ServerH_PORT;
            break;
        default:
            printf("Did not find %s in the book code list\n",bookcode);
            sprintf(msg,"Requested book %s is not present in the library\n",bookcode);
            return;
    }
        // Forward the request to the appropriate server
    sendto(udpSocket, bookcode, strlen(bookcode), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
      printf("Main Server received the book code list from the server%c using UDP over port %d\n",backServer,portNo);
      printf("Found %s located at server%c. send to server%c\n",bookcode,backServer,backServer);
     //send the username
      sendto(udpSocket,decryptUsername , strlen(decryptUsername), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        
    
    //receive from the servers
    socklen_t serverAddrLen = sizeof(serverAddress);
    
    char receivedMessage[1024];
    ssize_t receivedBytes = recvfrom(udpSocket, receivedMessage, sizeof(receivedMessage) - 1, 0,
                                  (struct sockaddr *)&serverAddress, &serverAddrLen);

    if (receivedBytes == -1) { 
    perror("Error receiving data");
     }   else {
    receivedMessage[receivedBytes] = '\0';
    if(strcmp(decryptUsername,"admin")==0){
    printf("Received message from server%c: %s\n",backServer, receivedMessage);
    }
    else{
    printf("Received message from server%c: %s\n",backServer, receivedMessage);
    }
    strcpy(msg,receivedMessage);
}

    // Close the UDP socket
    close(udpSocket);
    
}

void decryptUser(char *encryptedValue, char *decryptedValue) {
    for (int i = 0; encryptedValue[i] != '\0'; ++i) {
        if (isalpha(encryptedValue[i])) {
            char decrypt = isupper(encryptedValue[i]) ? 'A' : 'a';
            decryptedValue[i] = (encryptedValue[i] - decrypt + 21) % 26 + decrypt;
        } 
        else if (isdigit(encryptedValue[i])) {
            decryptedValue[i] = (encryptedValue[i] - '0' + 5) % 10 + '0';
        } else {
            decryptedValue[i] = encryptedValue[i]; //do nothing for symbols
        }
    }
}


int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
 // Set up server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
     // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

  //  printf("ServerM is listening on port %d...\n", SERVER_PORT);
     printf("Main Server is up and running\n");
   
    while(1){
     // Accept incoming connections
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }
    
    while(1){  
    // Receive data from the client
    char buffer[1024];
    ssize_t bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
    char username[50];
    char password[50];
    if (bytes_received == -1) {
        perror("Error receiving data");
    } else {
    buffer[bytes_received] = '\0';
      
        strncpy(username, strtok(buffer, ";"), sizeof(username) - 1);
        strncpy(password, strtok(NULL, ";"), sizeof(password) - 1);
        printf("Main Server received the username and password from the client using TCP over port %d\n",SERVER_PORT);
        printf("Received username: %s\n", username);
        printf("Received password: %s\n", password);
    }
   char msg[1024];
   char bookMsg[1024];
   char decryptUsername[20];
   validateUser(username,password,msg);
 
    send(clientSocket, msg, strlen(msg), 0);
  if (strcmp(msg, "Authentication successful") == 0){
  decryptUser(username,decryptUsername);
  char bookcode[1024];
   ssize_t bookcode_bytes=recv(clientSocket,bookcode,sizeof(bookcode),0);
   if(bookcode_bytes==-1){
   printf("Error in receiving the book code\n");
   }
   else{
   bookcode[bookcode_bytes]='\0';
}
   if(strlen(bookcode)>0){   
   printf("Main Server received the book request for %s from client using TCP over port%d\n",bookcode,SERVER_PORT);
   
   forwardRequest(bookcode,bookMsg,decryptUsername);
   }
   else{
   printf("Received empty book code\n");
   }
   //send book status to client
   printf("Main server sent the book status to the client\n");
   }
   

 //   // Send a response back to the client
       send(clientSocket, bookMsg, strlen(bookMsg), 0);

    // Close the sockets
    }
    close(clientSocket);
    }
    close(serverSocket);

    return 0;
}
