// guess_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1" // Change if server is on another machine
#define SERVER_PORT 12345
#define MAX_BUFFER 1024

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    socklen_t addr_len = sizeof(server_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid server IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Welcome to 'Guess the Number' game!\n");
    printf("Enter your guesses (numbers between 1 and 100).\n");

    while (1)
    {
        printf("Your guess: ");
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Error reading input.\n");
            break;
        }

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = 0;

        // Send guess to server
        int sent = sendto(sockfd, buffer, strlen(buffer), 0,
                          (struct sockaddr *)&server_addr, addr_len);
        if (sent < 0)
        {
            perror("sendto failed");
            break;
        }

        // Receive server response
        int n = recvfrom(sockfd, buffer, MAX_BUFFER - 1, 0,
                         NULL, NULL);
        if (n < 0)
        {
            perror("recvfrom failed");
            break;
        }

        buffer[n] = '\0';
        printf("Server says: %s\n", buffer);

        if (strcmp(buffer, "Correct") == 0)
        {
            printf("Congratulations! You guessed the number!\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
