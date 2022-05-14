#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "utils.h"

#define SERVER_IP "192.168.0.227"

int socket_desc;

void connect_to()
{
    struct sockaddr_in server_addr;

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Unable to create socket\n");
        exit(-1);
    }

    printf("Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Send connection request to server:
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Unable to connect\n");
        exit(-1);
    }
    printf("Connected with server successfully\n");
}

void connect_from()
{

    int client_sock;
    unsigned int client_size;
    struct sockaddr_in server_addr, client_addr;

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        exit(-1);
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        exit(-1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        exit(-1);
    }
    printf("\nListening for incoming connections.....\n");

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    if (client_sock < 0)
    {
        printf("Can't accept\n");
        exit(-1);
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    socket_desc = client_sock;
}

int send_coord(int curx, int cury)
{

    char *message = malloc(sizeof(char) * 10);
    char server_message[20];

    sprintf(message, "%d,%d;", curx, cury);

    if (send(socket_desc, message, sizeof(message), 0) < 0)
    {
        printf("Unable to send message\n");
        exit(-1);
    }
    if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
    {
        printf("Error while receiving server's msg\n");
        exit(-1);
    }
    printf("Server's response: %s\n", server_message);
    int extracted_message;
    sscanf(server_message, "%d;", &extracted_message);
    return extracted_message;
}

void send_ready()
{
    char client_message[6];

    if (send(socket_desc, "ready", 6, 0) < 0)
    {
        printf("Can't send\n");
        exit(-1);
    }

    while (strcmp(client_message, "ready"))
    {
        if (recv(socket_desc, client_message, sizeof(client_message), 0) < 0)
        {
            printf("Couldn't receive\n");
            exit(-1);
        }

        printf("Msg from client: %s\n", client_message);
    }
}

void send_response(int (*gameBoard)[10], short *shotx, short *shoty)
{

    char client_message[20];

    if (recv(socket_desc, client_message, sizeof(client_message), 0) < 0)
    {
        printf("Couldn't receive\n");
        exit(-1);
    }
    printf("Msg from client: %s\n", client_message);

    sscanf(client_message, "%hd,%hd;", shotx, shoty);

    if (gameBoard[*shoty][*shotx] == SHIP && flood_filled(gameBoard, *shotx, *shoty))
    {
        printf("Loď potopena!\n");
    }
    else if (gameBoard[*shoty][*shotx] == SHIP)
    {
        printf("Loď zasažena!\n");
    }
    else
    {
        printf("Vedle jak ta jedle!\n");
    }

    sprintf(client_message, "%d;", gameBoard[*shoty][*shotx]);

    if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
    {
        printf("Can't send\n");
        exit(-1);
    }
}