#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
/* Algorithm for TCP client
– Find the IP address and port number of server
– Create a TCP socket
– Connect the socket to server (Server must be up and listening for new
requests)
– Send/ receive data with server using the socket
– Close the connection */

#define MAX 1024           // Max size of buffer
#define PORT 8080          // The port number for the client to connect to
#define SA struct sockaddr // Create a shorthand for the type
#define MAX_FILENAME_SIZE 256

int main()
{
    int sockfd;
    struct sockaddr_in serveraddr;
    char buffer[MAX] = "";
    char *command;

    // Step 1: Initialization
    // 1.1 Establish Connection
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed…\n");
        exit(EXIT_SUCCESS);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address

    // Connect to server
    if (connect(sockfd, (SA *)&serveraddr, sizeof(serveraddr)) != 0)
    {
        printf("Connection to the server failed…\n");
        exit(EXIT_SUCCESS);
    }
    // printf("TESTING; Success!!!!Connection to the server Finish!!!\n"); // TESTING
    // ===================connection established =====================
   
   
    // Step 2: User Authentication
    char ServerMSG[25] = "";
    //-receive server's question (username)
    if (recv(sockfd, ServerMSG, sizeof(ServerMSG), 0) < 0)
    {
        perror("Error receiving Server question about ID\n");
        return -1;
    }
    printf("%s", ServerMSG);
    // 2.1 Send Credentials
    //-get username and send to server
    char username[50] = "";
    scanf("%49s", username);
    if (send(sockfd, username, strlen(username), 0) < 0)
    {
        perror("Faild send username\n");
        exit(-1);
    }
    memset(ServerMSG, 0, sizeof(ServerMSG));
    //-receive server's question (pwd)
    if (recv(sockfd, ServerMSG, sizeof(ServerMSG), 0) < 0)
    {
        perror("Error receiving Server question about Pwd\n");
        return -1;
    }
    printf("%s", ServerMSG);
    // 2.1 Send Credentials
    //-get pwd and send to server
    char userpwd[50] = "";
    scanf("%49s", userpwd);
    if (send(sockfd, userpwd, strlen(userpwd), 0) < 0)
    {
        perror("Faild send Pwd\n");
        exit(-1);
    }
    memset(ServerMSG, 0, sizeof(ServerMSG));
    // Waiting for response
    bzero(buffer, sizeof(buffer));
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    // printf("TESTING; byteread: %ld and TEST1;;;;;;;;;;%s", bytesRead, buffer); // testing
    // printf("\n");                                                              // testing
    if (bytesRead < 0)
    {
        perror("Error receiving Server response\n");
        return -1;
    }
    buffer[bytesRead] = '\0';
    // printf("TESTING; byteread: %ld and TEST2;;;;;;;;;;%s", bytesRead, buffer); // testing
    // printf("\n");                                                              // testing
    if (strcmp(buffer, "{\"STATUS\": \"success\", \"MESSAGE\": \"Authentication successful.\"}") == 0)
    {
        fflush(stdout);
        printf("%s\n", buffer);
    }
    else if (strcmp(buffer, "{\"STATUS\": \"error\", \"MESSAGE\": \"Invalid username or password.\"}") == 0)
    {
        fflush(stdout);
        printf("%s\n", buffer);
        close(sockfd);
        exit(0);
    }

    // Step 3: File Operations Command Loop
    while (1)
    {
        //- Take user input for command
        memset(buffer, 0, sizeof(buffer));
        printf("Enter command: ");
        scanf("%25s", buffer);
        // printf("Testing;; In while, 1st buff: %s", buffer);     // testing
        // printf("\n");                                           // testing
        command = strtok(buffer, "\n"); // Remove newline character from input
        // printf("Testing;; In while, 2nd command: %s", command); // testing
        // printf("\n");                                           // testing

        if (strcmp(command, "list_server") == 0)
        {
            // - If command is "list_server"
            // - Send "list_server" to server
            send(sockfd, command, strlen(command), 0);
            // - Receive and display file list from server
            bzero(buffer, sizeof(buffer));
            if (recv(sockfd, buffer, sizeof(buffer), 0) < 0)
            {
                perror("Error receiving list_server\n");
                return -1;
            }
            printf("Server files:\n%s\n", buffer);
        }
        else if (strcmp(command, "upload") == 0)
        {
            // - Else if command is "upload"
            // - Prompt user for filename and file content
            // - Send "upload" command, filename, and file content to server
            send(sockfd, command, strlen(command), 0);
            char filename[MAX_FILENAME_SIZE] = "";
            char filecontent[MAX] = "";
            printf("Enter filename: ");
            scanf("%250s", filename);
            strtok(filename, "\n"); // Remove newline character
            getchar();              // Consume the newline character left in the buffer
            send(sockfd, filename, strlen(filename), 0);
            printf("Enter file content: ");
            fgets(filecontent, sizeof(filecontent), stdin);
            strtok(filecontent, "\n"); // Remove newline character
            // printf("TESTING; filecontent = %s\n", filecontent); // TESTING
            send(sockfd, filecontent, strlen(filecontent), 0);

            // - Display server response
            bzero(buffer, sizeof(buffer));
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        }
        else if (strcmp(command, "download") == 0)
        {
            // - Else if command is "download"
            send(sockfd, command, strlen(command), 0);
            // - Prompt user for filename
            char dowfilename[MAX_FILENAME_SIZE];
            printf("Enter filename: ");
            scanf("%250s", dowfilename);
            strtok(dowfilename, "\n"); // Remove newline character
            // printf("TESTING; dowfilename = %s\n",dowfilename);//TESTING
            send(sockfd, dowfilename, strlen(dowfilename), 0);
            // - Receive and display file content or error message from server
            bzero(buffer, sizeof(buffer));
            recv(sockfd, buffer, sizeof(buffer), 0);
            if (strcmp(buffer, "{\"STATUS\": \"error\", \"MESSAGE\": \"File not found or not allowed to access\"}\n") == 0)
            {
                printf("%s", buffer);
                return -1;
            }
            printf("%s\n", buffer);
        }
        else if (strcmp(command, "exit") == 0)
        {
            send(sockfd, command, strlen(command), 0);
            break;
        }
    }

    // Closing the socket
    close(sockfd);

    return -1;
}
