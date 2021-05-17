#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#define NO_CLIENT -1
#define MAX_CLIENTS 64

const int server_listen_port = 5000;
const int MAX_MESSAGE_SIZE = 1024;

int server_socket_fd;
int client_sockets_fd[MAX_CLIENTS];
fd_set read_fd_set;

static void quit(int sig)
{
    if (sig == SIGINT || sig == 0 || sig == -1)
    {
        printf("Exiting...\n");

        FD_ZERO(&read_fd_set);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int socket_fd = client_sockets_fd[i];
            if (socket_fd > NO_CLIENT)
            {
                close(socket_fd);
            }
        }

        close(server_socket_fd);
        exit(sig);
    }
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int add_client_fd(int client_fd)
{

    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets_fd[i] == NO_CLIENT)
        {
            client_sockets_fd[i] = client_fd;
            break;
        }
    }

    if (i == MAX_CLIENTS)
    {
        return -1;
    }

    return i;
}

void handle_server()
{
    // creates client_address structure
    struct sockaddr_in client_address;
    unsigned int client_address_len = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));

    int client_socket_fd = accept(
        server_socket_fd,
        (struct sockaddr *)&client_address,
        &client_address_len);

    // add client_socket_fd to client_sockets_fd
    if (add_client_fd(client_socket_fd) == -1)
    {
        printf("Couldn't add another client\n");
        quit(-1);
    }

    printf("Added Client[%d]\n", client_socket_fd);
}

void handle_client(int client_socket, int index)
{

    char buffer[MAX_MESSAGE_SIZE * 2];

    int read_bytes = recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);

    if (read_bytes < 1)
    {
        printf("Client[%d] closed the connection...\n", client_socket);

        close(client_socket);
        client_sockets_fd[index] = NO_CLIENT;
    }

    buffer[read_bytes] = '\0';
    printf("<<<<Client[%d]: %s", client_socket, buffer);

    // relay messages to other clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        int other_client_fd = client_sockets_fd[i];

        if (other_client_fd == NO_CLIENT || other_client_fd == client_socket)
            continue;

        send(other_client_fd, buffer, read_bytes, 0);
    }
}

int main()
{
    signal(SIGINT, quit);

    // 1 . open socket
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {

        printf("socket creation failed (%s)\n", strerror(errno));
        quit(-1);
    }

    // 2 . setup server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;                 // setting only IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;         // accept any incoming address
    server_address.sin_port = htons(server_listen_port); // listen on port 'server_listen_port'

    // 3 . bin socket to address
    if (bind(server_socket_fd, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        printf("Error binding server (%s)\n", strerror(errno));
        quit(-1);
    }

    // 4 . setup select
    FD_ZERO(&read_fd_set);

    // set client_sockets_fd to default value
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets_fd[i] = NO_CLIENT;
    }

    // 5 . start listening for clients
    const int queue_size = 5;
    int is_listening = listen(server_socket_fd, queue_size);
    if (is_listening < 0)
    {
        printf("Error listening (%s)\n", strerror(errno));
        quit(-1);
    }

    // 6 . accept connections and process them
    printf("Server is open\n");

    while (1)
    {
        // clear read set and add server socket
        FD_ZERO(&read_fd_set);
        FD_SET(server_socket_fd, &read_fd_set);

        // adds valid clients to read_fd_set and finds max_fd
        int max_fd = 0;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets_fd[i] > 0)
            {
                FD_SET(client_sockets_fd[i], &read_fd_set);
                max_fd = max(max_fd, client_sockets_fd[i]);
            }
        }
        max_fd = max(max_fd, server_socket_fd) + 1;

        // use select to wait for a connection
        int has_selected = select(max_fd, &read_fd_set, NULL, NULL, NULL);

        if (has_selected < 0)
        {
            printf("Error on select (%s)\n", strerror(errno));
            quit(-1);
        }

        if (FD_ISSET(server_socket_fd, &read_fd_set))
        {
            // HANDLE SERVER
            handle_server();
        }

        // check clients for activity
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int client_socket_fd = client_sockets_fd[i];

            if (client_socket_fd == NO_CLIENT)
                continue;

            if (FD_ISSET(client_socket_fd, &read_fd_set))
            {
                // HANDLE CLIENT
                handle_client(client_socket_fd, i);
            }
        }
    }

    quit(0);
}