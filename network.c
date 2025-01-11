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
#define SERVER_PATH "./SERVER/" // Dossier où les fichiers seront enregistrés

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};

    // Configuration
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("erreure socket");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("erreure PORT");
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(server_fd, 3) < 0)
    {
        perror("erreure ecoute");
        exit(EXIT_FAILURE);
    }

    // Accepter une connexion entrante
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("erreure acceptation de connexion");
        exit(EXIT_FAILURE);
    }

    size_t commande_taille;
    if (read(new_socket, &commande_taille, sizeof(commande_taille)) <= 0)
    {
        perror("erreur lors de la lecture de la taille de la commande");
        exit(EXIT_FAILURE);
    }

    // Lecture de la commande en fonction de la longueur reçue
    char commande[commande_taille + 1]; // +1 pour le caractère null de fin de chaîne
    if (read(new_socket, commande, commande_taille) <= 0)
    {
        perror("Erreur lors de la lecture de la commande");
        exit(EXIT_FAILURE);
    }

    commande[commande_taille] = '\0'; // Assurez-vous que la chaîne est correctement terminée
    printf("Commande reçue: %s\n", commande);
    // Traiter la commande
    if (strcmp(commande, "envoyer") == 0)
    {
        // Lecture du nom du fichier
        size_t fichier_taille;
        if (read(new_socket, &fichier_taille, sizeof(fichier_taille)) <= 0)
        {
            perror("erreur lors de la lecture de la taille du fichier");
            exit(EXIT_FAILURE);
        }

        char fichier[fichier_taille + 1];
        if (read(new_socket, fichier, fichier_taille) <= 0)
        {
            perror("Erreur lors de la lecture du nom du fichier");
            exit(EXIT_FAILURE);
        }
        fichier[fichier_taille] = '\0';

        int version; // lecture de la version
        if (read(new_socket, &version, sizeof(version)) <= 0)
        {
            perror("Erreur lors de la lecture de la version");
            exit(EXIT_FAILURE);
        }

        // Affichage des informations reçues
        printf("Fichier '%s' version '%d' traitée\n", fichier, version);
    }
    else if (strcmp(commande, "recevoir") == 0) // Si la commande est "recevoir" récupérer les données envoyer
    {
        mkdir(SERVER_PATH, 0755);
        // Recevoir la taille du nom du fichier
        size_t filename_length;
        if (read(new_socket, &filename_length, sizeof(filename_length)) <= 0)
        {
            perror("erreure lors de la reception de la taille du nom du fichier");
            exit(EXIT_FAILURE);
        }

        // Recevoir le nom du fichier
        char filename[filename_length + 1];
        if (read(new_socket, filename, filename_length) <= 0)
        {
            perror("erreure lors de la reception du nom du fichier");
            exit(EXIT_FAILURE);
        }
        filename[filename_length] = '\0';
        // Créer le chemin du fichier de destination
        char filepath[MAX_BUFFER_SIZE + sizeof(SERVER_PATH)];
        snprintf(filepath, sizeof(filepath), "%s%s", SERVER_PATH, filename);
        int file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0)
        {
            perror("erreur lors de la creation du fichier");
            exit(EXIT_FAILURE);
        }

        // Recevoir le fichier et l'enregistrer
        int bytes_received;
        while ((bytes_received = read(new_socket, buffer, MAX_BUFFER_SIZE)) > 0)
        {
            write(file_fd, buffer, bytes_received);
        }

        printf("Fichier '%s' traiter", filename);

        close(file_fd);
        close(new_socket);
        close(server_fd);
    }
    else
    {
        fprintf(stderr, "Commande inconnue: %s\n", commande);
        exit(EXIT_FAILURE);
    }

    return 0;
}