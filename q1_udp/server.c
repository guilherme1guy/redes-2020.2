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
const int MAX_UDP_SIZE = 65507;

int main()
{
    int server_socket_fd;

    // 1 . open socket
    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // 4 . wait for client connection

    // setup structures
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(server_address));
    unsigned int client_address_len = sizeof(client_address);

    // allocates MAX_UDP_SIZE bytes for buffer of incoming messages
    char *message_buffer = malloc(MAX_UDP_SIZE * sizeof(char));
    // receive message (blocking)
    printf("Waiting connection\n");
    int bytes_received = recvfrom(
        server_socket_fd,
        message_buffer,
        MAX_UDP_SIZE,
        MSG_WAITALL,
        (struct sockaddr *)&client_address,
        &client_address_len);

    // put a string termination at the end of received message
    message_buffer[bytes_received] = '\0';

    printf("<<<<%s\n", message_buffer);

    // 5 . answer client
    char response_message[] = "PONG\0";
    int send_result = sendto(
        server_socket_fd,
        &response_message[0],
        strlen(response_message),
        MSG_CONFIRM,
        (struct sockaddr *)&client_address,
        client_address_len);

    if (send_result < 0)
    {
        printf("Error sending response (%s)\n", strerror(errno));
    }
    else
    {
        printf("Response sent to client! Ending...\n");
    }

    free(message_buffer);

    close(server_socket_fd);

    return 0;
}