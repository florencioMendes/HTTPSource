#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 9999
#define BACKLOG 3
#define BUFFER_SIZE 4096

void send_to_node(int pipe_fd, const char *data){
    write(pipe_fd, data, strlen(data));
};

char* receive_from_node(int pipe_fd){
    char buffer[BUFFER_SIZE] = {0};
    read(pipe_fd, buffer, BUFFER_SIZE);
    return strdup(buffer);
};

void handle_request(int client_socket, int *to_node, int *from_node) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);

    send_to_node(to_node[1], buffer);
    char *response = receive_from_node(from_node[0]);

    write(client_socket, response, strlen(response));   
    free(response);
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int MIN_VALUE_SUCCESS = 0;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int to_node[2];
    int from_node[2];
    pipe(to_node);
    pipe(from_node);

    pid_t pid = fork();
    if(pid == 0) {
        close(to_node[1]);
        dup2(to_node[0], STDIN_FILENO);
        close(from_node[0]);
        dup2(from_node[1], STDOUT_FILENO);
        execlp("node", "node", "processor.js", NULL);
        exit(EXIT_FAILURE);
    }; 

    close(to_node[0]);
    close(from_node[1]);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < MIN_VALUE_SUCCESS) {
        perror("faile to create socket");
        exit(EXIT_FAILURE);
    };

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < MIN_VALUE_SUCCESS) {
        perror("failed to bind socket");
        exit(EXIT_FAILURE);
    };

    if(listen(server_fd, BACKLOG) < MIN_VALUE_SUCCESS){
        perror("failed to listen on socket");
        exit(EXIT_FAILURE);
    };

    printf("server is listening on port %d\n", PORT);

    while(1){
        if((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < MIN_VALUE_SUCCESS) {
            perror("failed to accept connection");
            exit(EXIT_FAILURE);
        };

        handle_request(client_socket, to_node, from_node);
    };

    return 0;
};
