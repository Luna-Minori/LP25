#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 1024
#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define SERVER_PATH "./SERVER/"  // Dossier où les fichiers seront enregistrés


int send_network(char *filepath) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];
    int file_fd;
    int bytes_read;

    // Créer un socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Se connecter au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier
    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        perror("File not found");
        exit(EXIT_FAILURE);
    }

    // Envoyer le nom du fichier
    send(sock, filepath, strlen(filepath), 0);

    // Lire le fichier et l'envoyer au serveur
    while ((bytes_read = read(file_fd, buffer, MAX_BUFFER_SIZE)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }

    printf("File '%s' sent successfully\n", filepath);

    // Fermer la connexion et le fichier
    close(file_fd);
    close(sock);

    return 0;
}
