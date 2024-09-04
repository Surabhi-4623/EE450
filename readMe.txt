Name : Surabhi
Student ID: 5725833833

Implemented the extra credit part:
But admin username and password should be present in the member.txt(firns,firns)

client.c:
client keeps on asking for username until all the letters are in lowercase(it can include symbols but no space)
The username and password will be encrypted and sent to the server
If the authentication fails then the program will ask for username/password again
If the authentication goes through then client asks the user to enter book code(Book code is case sensitive)
Client then sends the bookcode this to serverM
Client prints the book status it receives from serverM through TCP
If it is an admin user then the number of books available will also be printed

serverM.c:
The server authenticates username/password from the client and sends the result to the client over TCP
After server receives the book code from the client,depending upon the prefix of the book code serverM will send it over to serverL or serverH or serverS
serverM returns the result obtained from the backendserver to the client through TCP

serverL.c/serverH.c/serverS.c:
The particular backend server checks in its database for the availability of that book received from serverM
Then it returns the result to serverM through UDP

members.txt:
username and password should be comma separated

literature.txt,science.txt,history.txt:
book code and availability should be comma separated

Output messages:
Username and password are printed in separate line
Example :
client.c output:
     Client is up and running
     Please enter the username in lowercase: welcome
     Please enter the password: welcome
     bjqhtrj sent an authentication request to the Main Server 
     bjqhtrj receieved the result of authentication from main server using TCP over por 12347:  Authentication successful
     Please enter the book code to query: 
     H202
     bjqhtrj sent the request to the main server
     Response received from the main server on TCP port : 12347
     The requested book H202 is doesn't exist

    
serverM.c output:
    Main Server is up and running
    Main Server received the username and password from the client using TCP over port 45833
    Received username: bjqhtrj
    Received password: bjqhtrj
    Main Server loaded the member list
    Password bjqhtrj matches the username.Authentication successful
    Main Server received the book request for H202 from client using TCP over port45833
    Main Server received the book code list from the serverL using UDP over port 42833
    Found H202 located at serverL. send to serverL
    Received message from serverL: The requested book H202 doesn't exist
    
serverH.c output:
    ServerH is up and running using UDP on port 43833
    ServerH received H202 code from the main server
    The requested book H202 doesn't exist
    
Limitations:
The book code in the file should be 4 chars long like L101(and it is case sensitive)
Sometimes "Segmentation fault (core dumped)" message is encountered in the ServerM 
For admin user: username-admin, password-admin
    
 Note:   
Reused socket creation, recv and send commands from Beej textbook

Included makefile.txt
To compile: make
To clean: make clean









