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

/*
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

int Entry_recuperation(char *path_save, char *path_restore)
{
    int version = -1;
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

    do
    {
        printf("\n");
        printf("-----------------------------------------------------------------\n");
        printf("- Entrez la versions du fichier que vous voulez                 -\n");
        printf("- Si vous voulez la derniere version entrez 0                   -\n");
        printf("-----------------------------------------------------------------\n");
        fgets(path_restore, 256, stdin);
        path_restore[strcspn(path_restore, "\n")] = '\0';
        version = atoi(path_restore);
    } while (version >= 0);
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
                Chunk chunks[10000000];
                int Nchunk = compute_chunk(basename(path_save), path_save, chunks);
                sauvegarde(chunks, Nchunk, basename(path_save), path_save);
                printf("debug 0");
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
        case 2:
        {
            char path_save[256];
            char path_restore[256];
            int verif;
            int version = Entry_recuperation(path_save, path_restore);
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
                recup_save_content(basename(path_save), version);
                printf("debug 0");
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
typedef struct Chunk
{
    char *data;
    char MD5[33];
    int index;
    int version;
} Chunk;

#define CHUNK_SIZE 4096
char *read_file(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Erreur: Impossible d'ouvrir le fichier");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1L)
    {
        perror("Erreur: Taille du fichier inconnue");
        fclose(file);
        return NULL;
    }
    rewind(file);

    char *content = (char *)malloc(file_size + 1);
    if (content == NULL)
    {
        printf("Erreur: Allocation mémoire échouée.\n");
        fclose(file);
        return NULL;
    }

    size_t total_read = 0;
    while (total_read < file_size)
    {
        size_t bytes_to_read = (file_size - total_read < CHUNK_SIZE) ? file_size - total_read : CHUNK_SIZE;
        size_t read_size = fread(content + total_read, 1, bytes_to_read, file);
        if (read_size != bytes_to_read)
        {
            perror("Erreur lors de la lecture du fichier");
            free(content);
            fclose(file);
            return NULL;
        }
        total_read += read_size;
    }

    content[file_size] = '\0';

    if (ferror(file))
    {
        perror("Erreur lors de la lecture du fichier");
        free(content);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return content;
}

void compute_chunk(char *nom_fichier, Chunk *chunks)
{ // découpe un fichier en chunk
    FILE *fichier_entree = fopen(nom_fichier, "rb");
    if (fichier_entree == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return;
    }

    char **chunk = (char **)malloc(sizeof(char *));
    int index_data = 0;
    int chunk_index = 0;
    // unsigned char md5[EVP_MAX_MD_SIZE];
    // unsigned int md5_len;
    // compute_md5_file(nom_fichier, md5, &md5_len);
    char *contenu = read_file(nom_fichier);
    // printf("contenu : %s\n", contenu);

    while (contenu[chunk_index] != '\0')
    {

        if (chunk_index % 4096 == 0)
        {

            chunks[index_data].index = index_data + 1;
            chunks[index_data].version = 1;
            // compute_md5_chunk(chunk, strlen(chunk), chunks[index_data].MD5);
            chunks[index_data].data = chunk;
            memset(chunk, 0, sizeof(chunk));
            index_data++;
        }
        strncat(chunk, &contenu[chunk_index], 1);
        chunk_index++;
    }

    /*
    while ((bytes_lus = fread(chunk, 1, HASH_TABLE_SIZE, fichier_entree)) > 0) { // découpe le fichier en chunk
        chunks->data = chunk;
        chunks->index = chunk_index;
        chunks->version = 1;
        compute_md5_chunk(chunk, strlen(chunk), chunks->MD5);
        chunk_index++;
    }
fclose(fichier_entree);
printf("Découpage terminé. %d chunks lus.\n", chunk_index);
}

int main()
{
    Chunk chunks[100];

    char *path = "C:\\Users\\Luna\\Desktop\\board_game_tournament";

    parcourir_dossier(path);
    /*
    compute_chunk(nom_fichier, chunks);
    printf("data 1 : %s\n\n\n", chunks[0].data);
    printf("data 2 : %s\n\n\n", chunks[1].data);
    printf("data 3 : %s\n\n\n", chunks[2].data);
    printf("index 1 : %d\n", chunks[0].index);
    printf("index 2 : %d\n", chunks[1].index);
    printf("index 3 : %d\n", chunks[2].index);
    printf("version 1 : %d\n", chunks[0].version);
    printf("version 2 : %d\n", chunks[1].version);
    printf("version 3 : %d\n", chunks[2].version);
    printf("MD5 1 : %s\n", chunks[0].MD5);
    printf("MD5 2 : %s\n", chunks[1].MD5);
    printf("MD5 3 : %s\n", chunks[2].MD5);

    Chunk un = {"data1", "d41d8cd98f00b204e9800998ecf8427e", 1, 1}; //oui
    Chunk deux = {"data2", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // oui
    Chunk trois = {"data3", "d41d8cd98f00b204e9800998ecf8427e", 1, 2}; // oui
    Chunk quatre = {"data4", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // non
    Chunk cinq = {"data5", "d41d8cd98f00b204e9800998ecf8427e", 2, 2}; // oui
    Chunk six = {"data6", "d41d8cd98f00b204e9800998ecf8428e", 3, 1}; // oui

    Chunk chunks1[5] = {un, deux, trois, cinq, six};
    Chunk chunks2[3] = {un, six, quatre};


    sauvegarder(chunks1, 5, nom_fichier);
    printf("ça c'eST FAIT\n");
    sauvegarder(chunks2, 3, nom_fichier);

    recup_save_content(nom_fichier, 2);
    printf("sortie\n");
    */
/*
Chunk un = {"data1", "d41d8cd98f00b204e9800998ecf8427e", 1, 1}; //oui
Chunk deux = {"data2", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // oui
Chunk trois = {"data3", "d41d8cd98f00b204e9800998ecf8427e", 1, 2}; // oui
Chunk quatre = {"data4", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // non
Chunk cinq = {"data5", "d41d8cd98f00b204e9800998ecf8427e", 2, 2}; // oui
Chunk six = {"data6", "d41d8cd98f00b204e9800998ecf8428e", 3, 1}; // oui

Chunk chunks1[5] = {un, deux, trois, cinq, six};
Chunk chunks2[3] = {un, six, quatre};

char *nom_fichier = "fichier_test";

sauvegarder(chunks1, 5, nom_fichier);
printf("ça c'eST FAIT\n");
sauvegarder(chunks2, 3, nom_fichier);

// read_file_in_chunks("fichier_test.txt");

return 0;
}
*/