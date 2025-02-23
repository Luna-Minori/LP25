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

int send_network(char *filepath)
{
    int sock;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];
    int file_fd;
    int bytes_read;

    // Créer un socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Se connecter au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection à echoué");
        exit(EXIT_FAILURE);
    }

    // Envoi la taille
    char *commande = "recevoir";
    size_t commande_taille = strlen(commande);
    if (send(sock, &commande_taille, sizeof(commande_taille), 0) == -1)
    {
        perror("Commande taille envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Envoi "recevoir"
    if (send(sock, commande, commande_taille, 0) == -1)
    {
        perror("Commande envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier
    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0)
    {
        perror("File not found");
        exit(EXIT_FAILURE);
    }

    // Extraire le nom du fichier sans chemin
    char *filename = strrchr(filepath, '/');
    if (filename != NULL)
    {
        filename++; // Avancer après le '/'
    }
    else
    {
        filename = filepath; // Le fichier est déjà un nom sans chemin
    }

    // Envoyer la taille du nom du fichier
    size_t filename_length = strlen(filename);
    if (send(sock, &filename_length, sizeof(filename_length), 0) == -1)
    {
        perror("Filename length send failed");
        close(file_fd);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Envoyer le nom du fichier
    if (send(sock, filename, filename_length, 0) == -1)
    {
        perror("Filename send failed");
        close(file_fd);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Lire le fichier et l'envoyer au serveur
    while ((bytes_read = read(file_fd, buffer, MAX_BUFFER_SIZE)) > 0)
    {
        if (send(sock, buffer, bytes_read, 0) == -1)
        {
            perror("Send failed");
            close(file_fd);
            close(sock);
            exit(EXIT_FAILURE);
        }
    }

    printf("Fichier '%s' envoyé\n", filepath);

    // Fermer la connexion et le fichier
    close(file_fd);
    close(sock);

    return 0;
}

void recup_network(char *nom_fichier, int version)
{
    int sock;
    struct sockaddr_in server_address;
    int temp_att = 1000000;

    // Créer un socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Se connecter au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection à echoué");
        exit(EXIT_FAILURE);
    }

    // Envoi la taille
    char *commande = "envoyer";
    size_t commande_taille = strlen(commande);
    if (send(sock, &commande_taille, sizeof(commande_taille), 0) == -1)
    {
        perror("Commande taille envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Attendre un peu avant d'envoyer la prochaine donnée
    usleep(temp_att); // 100ms

    // Envoi "envoyer"
    if (send(sock, commande, commande_taille, 0) == -1)
    {
        perror("Commande envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Envoi la taille du nom du fichier
    size_t taille = strlen(nom_fichier);
    if (send(sock, &taille, sizeof(taille), 0) == -1)
    {
        perror("taille envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Envoi "le nom du fichier"
    if (send(sock, nom_fichier, taille, 0) == -1)
    {
        perror("nom du fichier l'envoi à échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Envoi de la version
    if (send(sock, &version, sizeof(version), 0) == -1)
    {
        perror("Version envoi échoué");
        close(sock);
        exit(EXIT_FAILURE);
    }

    close(sock);
    printf("tout envoyer");
}