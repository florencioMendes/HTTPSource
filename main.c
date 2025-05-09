#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9999
#define BACKLOG 3
#define BUFFER_SIZE 1024

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    char response[] = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "Hello world!\n";

    read(client_socket, buffer, BUFFER_SIZE);

    printf("client request: \n%s\n", buffer);

    write(client_socket, response, strlen(response));
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int response_ok = 0;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < response_ok) {
        perror("faile to create socket");
        exit(EXIT_FAILURE);
    };

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < response_ok) {
        perror("failed to bind socket");
        exit(EXIT_FAILURE);
    };

    if(listen(server_fd, BACKLOG) < response_ok){
        perror("failed to listen on socket");
        exit(EXIT_FAILURE);
    };

    printf("server is listening on port %d\n", PORT);

    while(1){
        if((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < response_ok) {
            perror("failed to accept connection");
            exit(EXIT_FAILURE);
        };

        handle_request(client_socket);
    };

    return 0;
};
