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

int child_pid = 0;

int check_server_for_message(int connection_socket)
{
    char buffer[MAX_MESSAGE_SIZE];

    int read_bytes = recv(connection_socket, buffer, MAX_MESSAGE_SIZE, 0);

    if (read_bytes == 0)
    {
        // NO DATA / EOF
    }
    else if (read_bytes < 0)
    {
        printf("Connection error (%s)\n", strerror(errno));
    }
    else
    {
        buffer[read_bytes] = '\0';
        printf("\n<<<<Server: %s\n", buffer);
    }

    return read_bytes;
}

void handle_connection(int connection_socket)
{

    char buffer[MAX_MESSAGE_SIZE];

    printf("\n(ENTER to send|'CTRL + C' to quit)\n");

    while (1)
    {
        // get message to send
        printf(">>>>Type a message:  ");

        char c;
        int i = 0;
        do
        {
            c = getchar();
            buffer[i++] = c;
        } while (c != '\n');
        buffer[i++] = '\0';

        int bytes_written = write(connection_socket, buffer, i);

        if (bytes_written < 1)
        {
            break;
        }
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

    // splitting program into two processes, one will handle incoming server messages
    // the other will handle user ui and send messages to server
    child_pid = fork();

    if (child_pid == 0)
    {
        // CHILDREN

        handle_connection(socket_fd);
    }
    else
    {
        // FATHER

        while (1)
        {
            int read_bytes = check_server_for_message(socket_fd);

            if (read_bytes < 1)
            {
                break;
            }
        }

        kill(child_pid, SIGINT);
    }

    close(socket_fd);

    return 0;
}