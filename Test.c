#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

int verifier_fichier_ou_dossier(char *path)
{

    if (path == NULL)
    {
        return -1; // Chemin vide
    }

    int longueur = strlen(path);

    for (int i = longueur - 1; i >= 0; --i)
    {
        if (path[i] == '\\')
        {
            return 1; // dossier
        }
        if (path[i] == '.')
        {
            return 0; // Fichier
        }
    }

    return 1; // la racine
}

void parcourir_dossier(char *path)
{
    struct dirent *ent;
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        perror("Erreur lors de l'ouverture du dossier");
        return;
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        { // ignore le repertoire courant et parent
            continue;
        }

        char chemin_complet[1024];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", path, ent->d_name);

        struct stat st;
        if (stat(chemin_complet, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            { // cas d'un sous dossier appelle récursif
                parcourir_dossier(chemin_complet);
            }
            else
            {
                compute_chunk(chemin_complet); // fichier
            }
        }
    }
    closedir(dir);
}

void vider_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
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
        vider_buffer();
        fgets(path_save, 256, stdin);
        path_save[strcspn(path_save, "\n")] = '\0';
    } while (path_save[0] == '\0');
}

void Entry_recuperation(char *path_save, char *path_restore)
{

    Entry_sauvegarde(path_save);
    path_restore[0] = '\0';
    do
    {
        printf("\n");
        printf("-----------------------------------------------------------------\n");
        printf("- Rentrez le chemin du dossier ou vous voulez récupérer         -\n");
        printf("- la sauvegarde (/home/linux/Bureau)                            -\n");
        printf("-----------------------------------------------------------------\n");
        fgets(path_restore, 256, stdin);
        path_restore[strcspn(path_restore, "\n")] = '\0';
    } while (path_restore[0] == '\0');
}

void menu()
{
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
        printf("-[3] - quitter                                                  -\n");
        printf("-----------------------------------------------------------------\n");
        printf("\nQue souhaitez vous faire ? [1/2/3]\n");
        scanf("%c", &reponse);
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
                // compute_chunk(path_save);
                // sauvegarde(path_save);
                printf("debug 0");
                break;
            }
            case 1:
            {
                // parcourir_dossier(path_save);
                printf("debug 1");
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
            char path_restore[256];
            int verif;
            Entry_recuperation(path_save, path_restore);
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
                // compute_chunk(path_save);
                // sauvegarde(path_save);
                printf("debug 0");
                break;
            }
            case 1:
            {
                // parcourir_dossier(path_save);
                printf("debug 1");
                break;
            }
            default:
                break;
            }
            break;
        }
        case 3:
        {
            break;
        }
        default:
            printf("Erreur de saisie");
        }
    } while (choixmenu != 1 && choixmenu != 2 && choixmenu != 3);
}

int main()
{
    menu();
    return 0;
}