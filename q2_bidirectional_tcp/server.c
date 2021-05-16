#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

const int server_listen_port = 5000;
const int MAX_MESSAGE_SIZE = 1024;

int server_socket_fd;

static void quit(int sig)
{
    if (sig == SIGINT)
    {
        printf("Exiting...\n");
        close(server_socket_fd);
        exit(sig);
    }
}

void handle_client(int client_socket)
{

    char buffer[MAX_MESSAGE_SIZE];

    while (1)
    {
        int read_bytes = read(client_socket, buffer, MAX_MESSAGE_SIZE);
        buffer[read_bytes] = '\0';
        printf("<<<<Client: %s\n", buffer);
        if (strncmp("quit", buffer, 4) == 0)
        {
            printf("Client quitting...\n");
            break;
        }

        // get response
        printf(">>>>Type answer (ENTER to send)> ");
        char c;
        int i = 0;
        do
        {
            c = getchar();
            buffer[i++] = c;
        } while (c != '\n');
        buffer[i++] = '\0';

        write(client_socket, buffer, i);
    }

    close(client_socket);
}

int main()
{
    signal(SIGINT, quit);

    // 1 . open socket
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {

        printf("socket creation failed (%s)\n", strerror(errno));
        exit(-1);
    }

    // 2 . setup server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;                 // setting only IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;         // accept any incoming address
    server_address.sin_port = htons(server_listen_port); // listen on port 'server_listen_port'

    // 3 . bin socket to address
    int bind_result = bind(
        server_socket_fd,
        (struct sockaddr *)&server_address,
        sizeof(server_address));

    if (bind_result < 0)
    {
        printf("Error binding server (%s)\n", strerror(errno));
        exit(-1);
    }

    // 4 . start listening for clients
    const int queue_size = 5;
    int is_listening = listen(server_socket_fd, queue_size);
    if (is_listening < 0)
    {
        printf("Error listening (%s)\n", strerror(errno));
        exit(-1);
    }

    // 5 . accept connections and process them
    struct sockaddr_in client_address;
    unsigned int client_address_len = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));

    int client_socket_fd;

    while (1)
    {

        printf("Waiting for client...\n");
        client_socket_fd = accept(
            server_socket_fd,
            (struct sockaddr *)&client_address,
            &client_address_len);

        if (client_socket_fd < 0)
        {
            printf("Error accepting connection (%s)\n", strerror(errno));
            exit(-1);
        }

        printf("Client connected\n");
        handle_client(client_socket_fd);
    }

    close(server_socket_fd);

    return 0;
}