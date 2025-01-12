#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include "menu.h"
#include "file_modifier.h"
#include "backup_manager.h"
#include "file_handler.h"

int verifier_fichier_ou_dossier(char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        return -1; // Chemin invalide
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        return 1; // dossier
    }
    if (S_ISREG(path_stat.st_mode))
    {
        return 0; // fichier
    }
    return -1; // ni fichier ni dossier
}

void vider_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_version()
{
    char reponse[256];
    int version;

    do
    {
        printf("-----------------------------------------------------------------\n");
        printf("- Entrez la versions du fichier que vous voulez                 -\n");
        printf("- Si vous voulez la derniere version entrez 0                   -\n");
        printf("-----------------------------------------------------------------\n");
        fgets(reponse, sizeof(reponse), stdin);
        reponse[strcspn(reponse, "\n")] = '\0';
        version = atoi(reponse);
    } while (version < 0); // Corrected loop condition to avoid infinite loop
    return version;
}

int get_network()
{
    char reponse[256];
    int temp;

    do
    {
        printf("-----------------------------------------------------------------\n");
        printf("- Entrez 0 si vous cherchez un fichier en local et 1 pour un    -\n");
        printf("- fichier sur le server                                         -\n");
        printf("-----------------------------------------------------------------\n");
        fgets(reponse, sizeof(reponse), stdin);
        reponse[strcspn(reponse, "\n")] = '\0';
        temp = atoi(reponse);
    } while (temp != 0 && temp != 1); // Corrected loop condition to avoid infinite loop
    return temp;
}

void Entry_sauvegarde(char *path_save)
{
    path_save[0] = '\0';
    do
    {
        printf("\n");
        printf("-----------------------------------------------------------------\n");
        printf("- Rentrez le chemin du fichier ou dossier (/home/linux/Bureau)  -\n");
        printf("-----------------------------------------------------------------\n");
        fgets(path_save, 256, stdin);
        path_save[strcspn(path_save, "\n")] = '\0';
    } while (path_save[0] == '\0');
}

void menu()
{
    printf("Bienvenue dans le menu de sauvegarde\n");
    char reponse;
    int choixmenu = 0;

    do
    {
        printf("\n");
        printf("-----------------------------------------------------------------\n");
        printf("- Actions disponibles :                                         -\n");
        printf("-----------------------------------------------------------------\n");
        printf("-[1] - Sauvegarder un fichier ou un dossier                     -\n");
        printf("-[2] - Recuperer une versions d'un fichier ou d'un dossier      -\n");
        printf("-[3] - test                                                     -\n");
        printf("-[4] - quitter                                                  -\n");
        printf("-----------------------------------------------------------------\n");
        printf("\nQue souhaitez vous faire ? [1/2/3]\n");
        scanf(" %c", &reponse);
        vider_buffer(); // Clear the input buffer
        choixmenu = strtol(&reponse, NULL, 10);
        switch (choixmenu)
        {
        case 1:
        {
            char path_save[256];
            int verif;
            Entry_sauvegarde(path_save);
            verif = verifier_fichier_ou_dossier(path_save);
            switch (verif)
            {
            case -1:
            {
                perror("La chaine que vous avez rentrez est vide");
                break;
            }
            case 0:
            {
                Chunk chunks[100];
                int Nchunk = compute_chunk(basename(path_save), path_save, chunks);
                sauvegarder(chunks, Nchunk, basename(path_save), path_save);
                printf("Fichier enregistré\n");
                break;
            }
            case 1:
            {
                parcourir_dossier(path_save, "./Save");
                break;
            }
            default:
                break;
            }
            break;
        }
        case 2:
        {
            char path_save[256];
            int verif;
            Entry_sauvegarde(path_save);
            printf("je suis la\n");
            int version = get_version();
            printf("Version %d\n", version);
            // verif = verifier_fichier_ou_dossier(path_save);
            int bool_network = get_network(); // 0 = local, 1 = network
            verif = 0;

            switch (verif)
            {
            case -1:
            {
                perror("La chaine que vous avez rentrez est vide");
                break;
            }
            case 0:
            {
                printf("En travaux");
                recup_save_content(basename(path_save), path_save, version, bool_network);
                printf("Version %d restaurée\n", version);
                break;
            }
            case 1:
            {
                // parcourir_dossier(path_save);
                printf("En travaux");
                break;
            }
            default:
                break;
            }
            break;
        }
        case 3:
        {
            // parcourir_dossier("/home/luna/Desktop/Test", "/home/luna/Desktop/LP25/Save");
            // recup_network("./test.txt", 1);
            char *test = "test.txt";
            char *test2 = "./test.txt";
            recup_save_content(test, test2, 1, 0);

            break;
        }
        case 4:
        {
            break;
        }
        default:
            printf("Erreur de saisie");
        }
    } while (choixmenu != 1 && choixmenu != 2 && choixmenu != 3 && choixmenu != 4);
}
