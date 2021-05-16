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
    int socket_fd;

    // 1 . open socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // 4 . connect to server
    char message[] = "PING\0";
    printf(">>>>%s\n", message);
    int send_result = sendto(
        socket_fd,
        &message[0],
        strlen(message),
        MSG_CONFIRM,
        (struct sockaddr *)&server_address,
        sizeof(server_address));

    if (send_result < 0)
    {
        printf("Error sending message (%s)\n", strerror(errno));
    }

    // 5 . receive server answer

    // setup structures
    // allocates MAX_UDP_SIZE bytes for buffer of incoming messages
    char *message_buffer = malloc(MAX_UDP_SIZE * sizeof(char));
    // receive message (blocking)
    printf("Waiting server response...\n");

    unsigned int addr_len = sizeof(server_address);
    int bytes_received = recvfrom(
        socket_fd,
        message_buffer,
        MAX_UDP_SIZE,
        MSG_WAITALL,
        (struct sockaddr *)&server_address,
        &addr_len);

    // put a string termination at the end of received message
    message_buffer[bytes_received] = '\0';

    printf("<<<<%s\n", message_buffer);

    free(message_buffer);

    close(socket_fd);

    return 0;
}