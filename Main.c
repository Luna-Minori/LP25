#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <libgen.h>
#include "file_modifier.h"
#include "file_handler.h"
#include "backup_manager.h"
#include "menu.h"
#include "network_transmission.h"
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char choix[10] = "Network";
    pid_t pid;

    pid = fork(); // Créer un processus fils

    if (pid == -1)
    {
        // Gestion des erreurs de fork
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        if (strcmp(choix, "Network") == 0)
        {
            printf("Processus fils : Lancement de Network...\n");
            execl("./Network", "Network", NULL); // Lancer Network si l'utilisateur choisit "Network"
            perror("Erreur lors de l'exécution de Network");
        }
        else
        {
            printf("Choix invalide : %s\n", choix);
            exit(EXIT_FAILURE);
        }
    }

    if (argc == 1)
    {
        menu();
    }
    else if (argc == 2)
    {
        Chunk chunks[100];
        int Nchunk = compute_chunk(basename(argv[1]), argv[1], chunks);
        sauvegarder(chunks, Nchunk, basename(argv[1]), argv[1]);
    }
    else if (argc == 3)
    {
        int version = get_version();
        recup_save_content(basename(argv[1]), argv[1], version);
        printf("Version %d restaurée\n", version);
    }
    else
    {
        // Nombre d'arguments invalide
        printf("Nombre d'arguments invalide\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
