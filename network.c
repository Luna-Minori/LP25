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
#define SERVER_PATH "./SERVER/"  // Dossier où les fichiers seront enregistrés

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};

    // Créer un socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse et le port du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Lier le socket à l'adresse et au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accepter les connexions des clients
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection established with client\n");

    // Créer un dossier pour recevoir les fichiers si ce n'est pas déjà fait
    mkdir(SERVER_PATH, 0755);

    // Recevoir la taille du nom du fichier
    size_t filename_length;
    if (read(new_socket, &filename_length, sizeof(filename_length)) <= 0) {
        perror("Failed to receive filename length");
        exit(EXIT_FAILURE);
    }

    // Recevoir le nom du fichier
    char filename[filename_length + 1];
    if (read(new_socket, filename, filename_length) <= 0) {
        perror("Failed to receive filename");
        exit(EXIT_FAILURE);
    }
    filename[filename_length] = '\0';  // Assurez-vous que le nom du fichier est terminé par un caractère nul

    // Créer le chemin du fichier de destination
    char filepath[MAX_BUFFER_SIZE + sizeof(SERVER_PATH)];
    snprintf(filepath, sizeof(filepath), "%s%s", SERVER_PATH, filename);
    int file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        perror("File creation failed");
        exit(EXIT_FAILURE);
    }

    // Recevoir le fichier et l'enregistrer
    int bytes_received;
    while ((bytes_received = read(new_socket, buffer, MAX_BUFFER_SIZE)) > 0) {
        write(file_fd, buffer, bytes_received);
    }

    printf("File '%s' received and stored at '%s'\n", filename, filepath);

    // Fermer le fichier et la connexion
    close(file_fd);
    close(new_socket);
    close(server_fd);

    return 0;
}
