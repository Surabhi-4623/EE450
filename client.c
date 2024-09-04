#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <ctype.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 45833

//To check if username is in lowercase
bool isInLowercase(const char *username){
   for(int i=0;username[i]!='\0';++i){
      if(isalpha(username[i])){
          if(!islower(username[i])){
             return false;
           }
    }
    }
    return true;
}

void encryptValue(char *value, char *encryptedValue){
   for(int i=0;value[i]!='\0';++i){
   if(isalpha(value[i])){
     char encrypt=isupper(value[i]) ? 'A' : 'a' ;
     encryptedValue[i]=(value[i]-encrypt+5) % 26 +encrypt;
   }
   else if(isdigit(value[i])){
     encryptedValue[i]=(value[i] - '0' + 5) %10 +'0';
   }
   else{
     encryptedValue[i]=value[i];//do nothing for symbols
   }
   }
}

void justAlphaNum(char* original, char* new){
   int i=0;
   for(int j=0;original[j]!='\0';++j){
   if(isalnum((unsigned char)original[j])){
    new[i++]=original[j];
   }
   }
   new[i]='\0';
}

int main() {
  //  while(1){
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
     // Set up server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0) {
        perror("Error converting IP address");
        exit(EXIT_FAILURE);
    }
    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    
      printf(" Client is up and running\n");
      // Get username and password from the user
      char username[50], password[50];
      char encryptUsername[50], encryptPassword[50];
     
     while(1){
    do{
    printf("Please enter the username in lowercase: ");
    scanf("%s", username);
    }while(!isInLowercase(username));
     printf("Please enter the password: ");
    scanf("%s", password);

    //Encrypt username and password
    encryptValue(username,encryptUsername);
    encryptValue(password,encryptPassword);

    // Concatenate username and password with a separator ( ';')
    char message[1024];
    sprintf(message, "%s;%s", username, password);
     sprintf(message, "%s;%s", encryptUsername, encryptPassword);
  
    // Send the message to the server
    send(clientSocket, message, strlen(message), 0);
     
    //print message
    printf("%s sent an authentication request to the Main Server\n",encryptUsername);
     char buffer[1024];
    ssize_t bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';

    printf("%s receieved the result of authentication from main server using TCP over port %d:  %s\n", encryptUsername,SERVER_PORT,buffer);

// Depending upon authentication proceed with asking book code
if (strcmp(buffer, "Authentication successful") == 0) {
    while(1){
    printf("Please enter the book code to query: \n");
    char bookCode[1024];
    scanf("%s",bookCode);
    send(clientSocket,bookCode,strlen(bookCode),0);
    printf("%s sent the request to the main server\n",encryptUsername);
    
       char response[1024];
       ssize_t response_received = recv(clientSocket, response, sizeof(response), 0);
response[response_received] = '\0';


    printf("Response received from the main server on TCP port : %d\n",SERVER_PORT);
    printf("%s\n", response);
      }  
    break;
  
    } else {
    printf("Authentication failed. Please try again.\n");
    // Ask for the username again
}

    }
    
    // Close the socket
    close(clientSocket);
    //}

    return 0;
} 
   
    



