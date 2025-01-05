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


int main()
{
    char choix[10] = "Network";
    pid_t pid;

    pid = fork(); // Créer un processus fils

    if (pid == -1) {
        // Gestion des erreurs de fork
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processus fils
        if (strcmp(choix, "Main") == 0) {
            printf("Processus fils : Lancement de Main...\n");
            execl("./Main", "Main", NULL); // Lancer Main si l'utilisateur choisit "Main"
            perror("Erreur lors de l'exécution de Main");
        } else if (strcmp(choix, "Network") == 0) {
            printf("Processus fils : Lancement de Network...\n");
            execl("./Network", "Network", NULL); // Lancer Network si l'utilisateur choisit "Network"
            perror("Erreur lors de l'exécution de Network");
        } else {
            // Si l'utilisateur ne fait pas un choix valide
            printf("Choix invalide : %s\n", choix);
            exit(EXIT_FAILURE);
        }
    } else {
        // Processus parent : attendre la fin du processus fils
        printf("Processus parent : L'exécutable a terminé.\n");
    }

    menu();
    
    return EXIT_SUCCESS;
}
