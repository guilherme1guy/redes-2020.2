#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

const int server_listen_port = 5000;
const int MAX_MESSAGE_SIZE = 1024;

void handle_connection(int connection_socket)
{

    char buffer[MAX_MESSAGE_SIZE];

    while (1)
    {
        // get message to send
        printf(">>>>Type a message (ENTER to send|'quit' to quit)> ");
        char c;
        int i = 0;
        do
        {
            c = getchar();
            buffer[i++] = c;
        } while (c != '\n');
        buffer[i++] = '\0';

        write(connection_socket, buffer, i);

        if (strncmp("quit", buffer, 4) == 0)
        {
            break;
        }

        int read_bytes = read(connection_socket, buffer, MAX_MESSAGE_SIZE);
        buffer[read_bytes] = '\0';
        printf("<<<<Server: %s\n", buffer);
    }
}

int main()
{
    int socket_fd;

    // 1 . open socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
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

    // 3 . connect to server and start messaging
    int is_connected = connect(
        socket_fd,
        (struct sockaddr *)&server_address,
        sizeof(server_address));

    if (is_connected != 0)
    {
        printf("Error connecting to server (%s)\n", strerror(errno));
        exit(-1);
    }

    // handle connection to server
    handle_connection(socket_fd);

    close(socket_fd);

    return 0;
}