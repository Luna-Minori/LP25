#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include "menu.h"
#include "file_modifier.h"
#include "backup_manager.h"
#include "file_handler.h"

int verifier_fichier_ou_dossier(char *path)
{

    if (path == NULL)
    {
        return -1; // Chemin vide
    }

    int longueur = strlen(path);

    for (int i = longueur - 1; i >= 0; --i)
    {
        if (path[i] == '/')
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
/*
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void creer_dossier(const char *chemin_dossier)
{
    if (mkdir(chemin_dossier) == 0) // linux mkdir(chemin_dossier, 755) pour les permissons
    {
        printf("Création du dossier : %s\n", chemin_dossier);
    }
    else
    {
        perror("Erreur lors de la création du dossier");
    }
}

void parcourir_dossier(const char *dossier, const char *dossier_save)
{
    struct dirent *ent;
    DIR *dir = opendir(dossier);

    if (dir == NULL)
    {
        perror("Erreur lors de l'ouverture du dossier");
        return;
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) // Ignore les entrées spéciales "." et ".."
        {
            continue;
        }
        char chemin_complet[1024];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", dossier, ent->d_name);

        // Crée le chemin complet du dossier dans /Save
        char chemin_save[1024];
        // unsigned char md5[EVP_MAX_MD_SIZE];
        // unsigned int md5_len;
        // compute_md5_file(nom_fichier, md5, &md5_len);
        // snprintf(chemin_save, sizeof(chemin_save), "%s/%s", md5, ent->d_name);
        printf("Trouvé : %s\n", chemin_complet);

        struct stat st;
        if (stat(chemin_complet, &st) == 0 && S_ISDIR(st.st_mode))
        {
            creer_dossier(chemin_save);
            parcourir_dossier(chemin_complet, chemin_save);
        }
    }
    closedir(dir);
}

int main() // int argc, char *argv[]
{
    char *path = "C:\\Users\\Luna\\Desktop\\board_game_tournament";
    // Définit le dossier de sauvegarde de base (/Save)
    const char *racine_save = "./Save";
    // Appelle la fonction pour parcourir le dossier
    parcourir_dossier(path, racine_save);

    return 0;
}
*/

void vider_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_version() {
    char reponse[256];
    int version;

    do {
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
/*
void Entry_recuperation(char *path_save)
{
    //int version = -1;
    //char reponse[256];
    Entry_sauvegarde(path_save);
    
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
*/
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
                Chunk chunks[100];
                int Nchunk = compute_chunk(basename(path_save), path_save, chunks);
                sauvegarder(chunks, Nchunk, basename(path_save), path_save);
                printf("Fichier enregistré\n");
                break;
            }
            case 1:
            {
                
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
            int version = get_version();
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
                recup_save_content(basename(path_save), path_save, version);
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
            break;
        }
        default:
            printf("Erreur de saisie");
        }
    } while (choixmenu != 1 && choixmenu != 2 && choixmenu != 3);
    
}
