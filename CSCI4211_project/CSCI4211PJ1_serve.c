#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h> // For threading, link with lpthread (phase 2)

#define MAX 1024
#define PORT 8080
#define SA struct sockaddr
#define MAX_LINE_LENGTH 25
#define MAX_FILENAME_SIZE 256

// Simple structure for username and password
struct user
{
    char username[50]; // Max length is 50 byte
    char password[50];
};

// Global variable for user data
struct user users[10]; // assume: only 10 users allowed for this example
int user_count = 0;

// read data file and store each user's data at User global structure
void read_user_data()
{
    char line[MAX_LINE_LENGTH];
    // Open the 'user_data.txt' file in read mode
    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    // Read each line from the file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Remove newline character at the end of the line
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        // printf("Testing data read::: %s \n", line); // TESTING
        // line points after first ":" which is pwd
        char *colonPosition = strchr(line, ':');
        // If colon is found
        if (colonPosition != NULL)
        {
            // Calculate the length of the username
            // printf("colonpos = %s\n", colonPosition); // TESTING
            size_t usernameLength = colonPosition - line;
            // printf("usernameLength = %ld\n", usernameLength); // TESTING
            strncpy(users[user_count].username, line, usernameLength);
            // Calculate the length of the password and allocate memory
            size_t passwordLength = strlen(colonPosition + 1);
            // Copy the password string
            strcpy(users[user_count].password, colonPosition + 1);
            // printf("Testing;; Usernumber:%d ==> ID: %s---pwd: %s \n",user_count, users[user_count].username, users[user_count].password); // TESTING
        }
        user_count++;
        if (user_count >= 10)
        {
            printf("Too many users, only the first 10 are loaded.\n");
            break;
        }
    }
    // Close the file
    fclose(file);
}

/*  ==========================  command functions ============================== */
void list_server_files(int client_socket, char *username)
{
    DIR *dir;
    struct dirent *entry;
    char fileList[MAX] = "";

    // Open the current directory
    dir = opendir("ServerDir");
    if (dir == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_SUCCESS);
    }

    // Read files from the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Check if the file name starts with the username
        // function finds the first occurrence of the substring
        // e.g.) strstr(entry->d_name, alice) = alice_file1.txt and alice_file2.txt
        if (strstr(entry->d_name, username) == entry->d_name)
        {
            // Allocate memory for the file name
            char *fileName = malloc(strlen(entry->d_name) + 1);
            if (fileName == NULL)
            {
                perror("Error allocating memory for fileName");
                exit(EXIT_SUCCESS);
            }
            strcpy(fileName, entry->d_name);
            // printf("TESTING>>> fileName: %s \n", fileName); // TESTING
            char files[25];
            sprintf(files, "%s  \n", fileName); // current file name
            strcat(fileList, files);            // make the final fileList
            free(fileName);
        }
    }

    // Close the directory
    closedir(dir);
    // printf("TESTING>>> FINAL fileList= %s \n", fileList); // TESTING
    send(client_socket, fileList, sizeof(fileList), 0);
}

void upload_file(int client_socket, char *username)
{
    char fileName[MAX_FILENAME_SIZE] = "";
    char filecont[MAX] = "";
    memset(filecont, 0, MAX);
    char FinalFilepath[400] = "";
    // Receive file name from client
    if (recv(client_socket, fileName, sizeof(fileName), 0) < 0)
    {
        perror("Error receiving filename\n");
        exit(EXIT_SUCCESS);
    }
    // Receive file content from client
    if (recv(client_socket, filecont, sizeof(filecont), 0) < 0)
    {
        perror("Error receiving file content\n");
        exit(EXIT_SUCCESS);
    }
    // printf("TESTING; filecont = %s\n", filecont); // TESTING
    //  - Associate file with username
    sprintf(FinalFilepath, "ServerDir/%s_%s.txt", username, fileName);
    // Save file to server's directory
    FILE *file = fopen(FinalFilepath, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing\n");
        exit(EXIT_SUCCESS);
    }
    // write the content in the file
    fwrite(filecont, 1, strlen(filecont), file);
    fclose(file);
    // Send success message to client
    char success_message[] = "{\"MESSAGE\": \"File uploaded successfully.\"} \n";
    send(client_socket, success_message, sizeof(success_message), 0);
}

void download_file(int client_socket, char *username)
{

    char filename[MAX_FILENAME_SIZE] = "";
    char filepath[MAX_FILENAME_SIZE + 50] = ""; // filename + rest of path
    char filecontent[MAX] = "";
    char error_message[] = "{\"STATUS\": \"error\", \"MESSAGE\": \"File not found or not allowed to access\"}\n";

    // Receive filename from client
    if (recv(client_socket, filename, sizeof(filename), 0) < 0)
    {
        perror("Error receiving filename for download\n");
        exit(EXIT_SUCCESS);
    }
    // printf("TESTING; filename = %s\n",filename);//TESTING
    //  Check if file exists and is owned by the specified username
    sprintf(filepath, "ServerDir/%s_%s.txt", username, filename);
    // printf("TESTING; filepath = %s\n",filepath);//TESTING

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    { // - Else:
        // File not found or not allowed to access
        // - Send {\"STATUS\": \"error\", \"MESSAGE\": \"File not found or not allowed to access\"}
        // to client
        send(client_socket, error_message, sizeof(error_message), 0);
        exit(EXIT_SUCCESS);
    }

    // - If file exists and is owned by username:
    // - Send file content to client
    // Read file content
    fread(filecontent, 1, sizeof(filecontent), file);
    char sendingData[sizeof(filecontent) + sizeof(filename) + 30];
    sprintf(sendingData, "In %s_%s.txt >>> %s", username, filename, filecontent);
    fclose(file);

    // Send file content to client
    send(client_socket, sendingData, strlen(sendingData), 0);
}
/* ============================================  handle client request ===========================================*/
// Function to send and receive data for authentication and response client request
void handle_client_request(int client_socket)
{
    char buffer[MAX] = "";
    char userName[50] = "";
    char passWord[50] = "";

    // Authentication process
    strcpy(buffer, "Enter username:");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, userName, 50, 0);

    strcpy(buffer, "Enter password:");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, passWord, 50, 0);

    // 2.1 Authentication
    int auth_success = 0;
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, userName) == 0 && strcmp(users[i].password, passWord) == 0)
        {
            auth_success = 1;
            break;
        }
    }

    memset(buffer, 0, MAX); // clean the buff
    // if user pass to authorization
    if (auth_success)
    {
        strcpy(buffer, "{\"STATUS\": \"success\", \"MESSAGE\": \"Authentication successful.\"}");
    }
    else
    {
        // if user fail to authorization
        strcpy(buffer, "{\"STATUS\": \"error\", \"MESSAGE\": \"Invalid username or password.\"}");
    }
    // send to client
    send(client_socket, buffer, strlen(buffer), 0);
    if (!auth_success)
    {
        close(client_socket);
        exit(0);
    }

    // Command loop: simplified to only receive a command and not actually handle files
    while (1)
    {
        bzero(buffer, MAX);

        // Receive command from client
        // buff = command buffer
        recv(client_socket, buffer, sizeof(buffer), 0);

        if (strcmp(buffer, "list_server") == 0)
        {
            // Call list_server_files(username) and send result to client
            list_server_files(client_socket, userName);
        }
        else if (strcmp(buffer, "upload") == 0)
        {
            // Call upload_file(client_socket, username)
            upload_file(client_socket, userName);
        }
        else if (strcmp(buffer, "download") == 0)
        {
            // Call download_file(client_socket, username)
            download_file(client_socket, userName);
        }
        else if (strcmp(buffer, "exit") == 0)
        {
            break; // Exit the loop
        }
        else // any other case
        {
            // For other commands, just send back a placeholder message
            char *message = "Command received but we do not have that function\n";
            send(client_socket, message, strlen(message), 0);
        }
    }

    close(client_socket);
}

// Thread function to handle connection
void *client_handler(void *arg)
{
    int client_socket = *((int *)arg);
    handle_client_request(client_socket);
    // this is phase 2
    // pthread_exit(NULL); // Exit the thread
}

//========================================= main ==================================================================
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in serveraddr, cli;

    // Step 1: Initialization
    // 1.1 Read User Data
    read_user_data();

    // 1.2 Start Server and Listen for Connections
    /* AF_INET = IP protocol family.  */
    /*SOCK_STREAM =  Sequenced, reliable, connection-based byte streams.  */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Socket creation failed...\n");
        return -1;
    }
    // Setting the serveraddr===================
    // IP protocol
    serveraddr.sin_family = AF_INET;
    // port
    serveraddr.sin_port = htons(PORT);
    // Host address (IP)
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Done for setting the serveraddr===================
    // Bind to a specific IP address and Port
    if ((bind(server_socket, (SA *)&serveraddr, sizeof(serveraddr))) != 0)
    {
        printf("Socket bind failed...\n");
        return -1;
    }
    // Listen for incoming connection
    /* # in listen() =  the maximum length to which the queue of pending connections for sockfd may grow. */
    if ((listen(server_socket, 30)) != 0)
    {
        printf("Listen failed...\n");
        return -1;
    }
    // ===================connection established =====================
   
   
    // Step 2: Handling Client Requests
    while (1)
    {
        int len = sizeof(cli);

        // Accept the data packet from client
        client_socket = accept(server_socket, (SA *)&cli, &len);
        if (client_socket < 0)
        {
            printf("Server accept failed...\n");
            continue; // Skip the failed connection
        }
        // this is phase 1
        // client_handler((void *)&client_socket);

        // This is phase 2
        pthread_t tid;
        // // Create a new thread for each client
        if (pthread_create(&tid, NULL, client_handler, (void *)&client_socket) != 0)
        {
            printf("Failed to create thread...\n");
            return -1;
        }
    }

    // if (pthread_join(tid, NULL) != 0)
    // {
    //     perror("fail pthread_join for worker_threads\n");
    //     return -1;
    // }
    close(server_socket);
}
