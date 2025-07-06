// guess_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_BUFFER 1024
#define MAX_NUMBER 100

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER];
    socklen_t addr_len = sizeof(client_addr);
    int target_number;

    // Seed the random number generator
    srand(time(NULL));
    target_number = rand() % MAX_NUMBER + 1;

    printf("Server started. Target number is: %d\n", target_number);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any local IP
    server_addr.sin_port = htons(PORT);       // Server port

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        memset(buffer, 0, MAX_BUFFER);

        // Receive message from client
        int n = recvfrom(sockfd, buffer, MAX_BUFFER - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }

        int guess = atoi(buffer);
        printf("Received guess %d from %s:%d\n", guess,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Prepare response
        char response[MAX_BUFFER];
        if (guess < target_number)
        {
            strcpy(response, "Higher");
        }
        else if (guess > target_number)
        {
            strcpy(response, "Lower");
        }
        else
        {
            strcpy(response, "Correct");
        }

        // Send response
        sendto(sockfd, response, strlen(response), 0,
               (const struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}
