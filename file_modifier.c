#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <dirent.h>
#include "file_modifier.h"
#include "file_handler.h"
#include "backup_manager.h"

#define CHUNK_SIZE 4096

int element_dans_liste(int element, int *liste, int taille)
{// vérifie si un élément est dans une liste
    for (int i = 0; i < taille; i++)
    {
        if (liste[i] == element)
        {
            return 1;
        }
    }
    return 0;
}

char **read_file_lines(const char *filename)
{// lit un fichier ligne par ligne et retourne un tableau de chaînes de caractères
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Erreur: Impossible d'ouvrir le fichier");
        return NULL;
    }

    char **lines = NULL;
    char buffer[1024];
    int num_lines = 0;

    while (fgets(buffer, sizeof(buffer), file))
    {
        lines = realloc(lines, (num_lines + 1) * sizeof(char *));
        if (lines == NULL)
        {
            perror("Erreur: Allocation mémoire échouée");
            fclose(file);
            return NULL;
        }
        lines[num_lines] = strdup(buffer);
        if (lines[num_lines] == NULL)
        {
            perror("Erreur: Allocation mémoire échouée");
            fclose(file);
            return NULL;
        }
        num_lines++;
    }

    lines = realloc(lines, (num_lines + 1) * sizeof(char *));
    if (lines == NULL)
    {
        perror("Erreur: Allocation mémoire échouée");
        fclose(file);
        return NULL;
    }
    lines[num_lines] = NULL;

    fclose(file);
    return lines;
}

char *read_file(char *filename)
{// lit un fichier et retourne son contenu sous forme de chaîne de caractères
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

int read_savefile_in_chunks(char *filename, char *path, Chunk *chunks, int network)
{// lit un fichier de sauvegarde et stocke les chunks dans un tableau les uns a la suite des autres
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);
    char md5_fichier[33];
    for (int i = 0; i < 16; ++i)
    {
        snprintf(&md5_fichier[i * 2], 3, "%02x", md5[i]);
    }
    char nom_fichier_sauvegarde[57];
    if (network == 0)
    {
        snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "./Save/%s_sauvegarde.txt", md5_fichier);
    }
    else
    {
        snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "./SERVER/%s_sauvegarde.txt", md5_fichier);
    }
    char **file_content = read_file_lines(nom_fichier_sauvegarde);
    if (file_content == NULL)
    {
        return 0;
    }
    FILE *file = fopen(nom_fichier_sauvegarde, "rb");
    char ligne[4096];

    int nombre_chunks = 0;
    while (fgets(ligne, sizeof(ligne), file))
    {// compte le nombre de chunks dans le fichier en recherchant le nombre de lignes avec un point-virgule au 33ème caractère
        if (ligne[32] == ';' && strlen(ligne) > 32)
        {
            nombre_chunks++;
        }
    }

    fclose(file);
    int nombre_lignes = 0;
    while (file_content[nombre_lignes] != NULL)
    {
        nombre_lignes++;
    }
    int k = 1;
    int pointeur = 0;
    int i = 0;
    while (i < nombre_chunks)
    {
        Chunk chunk;

        char lignes[4096] = {};
        if (file_content[k][32] == ';') // recup l'index et la version du chunk et les insère dans la structure
        {
            strncpy(chunk.MD5, file_content[k], 32);
            char temp_index[16] = {};
            int h = 33;
            while (file_content[k][h] != ';')
            {
                strncat(temp_index, &file_content[k][h], 1);
                h++;
            }
            int taille = strlen(temp_index);
            temp_index[taille] = '\0';
            chunk.index = atoi(temp_index);
            char temp_version[16] = {};
            h++;
            while (file_content[k][h] != '\0')
            {
                char temp[2] = {file_content[k][h], '\0'};
                strncat(temp_version, temp, 1);
                h++;
            }
            chunk.version = atoi(temp_version);
        }
        k++;
        while (k < nombre_lignes && pointeur < 4096 && file_content[k][32] != ';') // recup la data du chunk et l'insère dans la structure
        {
            pointeur = 0;

            while (pointeur < strlen(file_content[k]))
            {
                strncat(lignes, &file_content[k][pointeur], 1);
                pointeur++;
            }
            k++;
        }
        if (pointeur > 4096) // si on est ai milieu d'un chunk, on recule d'une ligne
        {
            pointeur = 0;
            k--;
        }

        chunk.data = strdup(lignes);
        chunks[i] = chunk;
        i++;
    }
    free(file_content);
    return nombre_chunks;
}

void recup_save_content(char *nom_fichier, char *path, int version, int network)
{// récupère le contenu d'un fichier sauvegardé afin d'en restaurer le contenu à une version donnée
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);

    char md5_fichier[33];
    for (int i = 0; i < 16; ++i)
    {
        snprintf(&md5_fichier[i * 2], 3, "%02x", md5[i]);
    }
    char nom_fichier_sauvegarde[57];
    if (network == 0)
    {
        snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "./Save/%s_sauvegarde.txt", md5_fichier);
    }
    else
    {
        snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "./SERVER/%s_sauvegarde.txt", md5_fichier);
    }
    if (access(nom_fichier_sauvegarde, F_OK) == -1)
    {
        perror("Erreur: Le fichier de sauvegarde n'existe pas\n");
        printf("nom du fichier de sauvegarde :%s\n", nom_fichier_sauvegarde);
        return;
    }
    FILE *file = fopen(nom_fichier_sauvegarde, "rb");
    if (file == NULL)
    {
        perror("Erreur: Impossible d'ouvrir le fichier de sauvegarde");
        return;
    }
    // Déplacez le curseur à la fin du fichier
    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("Erreur lors de la position du curseur dans le fichier");
        fclose(file);
        return;
    }

    long file_size = ftell(file); // Récupère la taille du fichier
    if (file_size == -1L)
    {
        perror("Erreur: Taille du fichier de sauvegarde inconnue");
        fclose(file);
        return;
    }

    rewind(file);
    fclose(file);
    Chunk chunks[file_size];
    read_savefile_in_chunks(nom_fichier_sauvegarde, path, chunks, network);

    if (chunks->data == NULL)
    {
        perror("Erreur: Impossible de lire les chunks du fichier de sauvegarde");
        return;
    }
    char nom_fichier_temp[256 + 9];
    int compteur = 0;
    char temp_nom[256] = {0};
    while (compteur < strlen(nom_fichier) - 4)
    {
        strncat(temp_nom, &nom_fichier[compteur], 1);
        compteur++;
    }

    if (network == 0)
    {
        snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_temp.txt", temp_nom);
    }
    else
    {
        snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_temp.txt", temp_nom);
    }

    FILE *temp_file = fopen(nom_fichier_temp, "wb");
    if (temp_file == NULL)
    {
        perror("Erreur: Impossible de créer le fichier temporaire");
        return;
    }

    int i = 0;
    int index_max = 0;

    while (chunks[i].data != NULL)
    {// On cherche l'index maximal dans les chunks
        if (chunks[i].index > index_max)
        {
            index_max = chunks[i].index;
        }
        i++;
    }

    int index_versions_proches[index_max + 1];
    for (int j = 0; j < index_max; j++)
    {// J'initalise le tableau des index des versions les plus proches à -1
        index_versions_proches[j] = -1;
    }

    i = 0;
    while (chunks[i].data != NULL)
    {// On cherche les index des chunks des versions les plus proches de la version demandée (au bon index)
        if (index_versions_proches[(chunks[i].index - 1)] == -1)
        {// si l'index vaut moins 1 c'est qu'il n'y a pas de version à cet index, on l'initialise
            index_versions_proches[(chunks[i].index - 1)] = i;
        }

        else
        {
            if (chunks[index_versions_proches[(chunks[i].index) - 1]].version < chunks[i].version && chunks[i].version <= version)
            {// Si la version est plus proche de celle demandée sans la dépasser, on la remplace
                index_versions_proches[(chunks[i].index) - 1] = i;
            }
        }
        i++;
    }
    for (int k = 0; k < index_max; k++)
    {
        fprintf(temp_file, "%s", chunks[index_versions_proches[k]].data);
    }
    fclose(temp_file);
    
    //Remplacer l'ancien fichier par le fichier temporaire
    if (remove(nom_fichier) != 0)
    {
        perror("Erreur de suppression de l'ancien fichier");
        return;
    }
    if (rename(nom_fichier_temp, nom_fichier) != 0)
    {
        perror("Erreur de renommage du fichier temporaire");
        return;
    }
}

void creer_dossier(const char *chemin_dossier)
{// crée un dossier a un path demandé
    if (mkdir(chemin_dossier, 0777) == 0) // linux mkdir(chemin_dossier, 0755) pour les permissons
    {
        printf("Création du dossier : %s\n", chemin_dossier);
    }
    else
    {
        perror("Erreur lors de la création du dossier");
    }
}

void parcourir_dossier(char *dossier, char *dossier_save)
{ // parcours un dossier et sauvegarde les fichiers et dossiers dans un dossier de sauvegarde
    struct dirent *fichieroudossier;
    DIR *dir = opendir(dossier);

    if (dir == NULL)
    {
        perror("Erreur lors de l'ouverture du dossier");
        return;
    }

    while ((fichieroudossier = readdir(dir)) != NULL)
    {
        if (strcmp(fichieroudossier->d_name, ".") == 0 || strcmp(fichieroudossier->d_name, "..") == 0) // Ignore les entrées spéciales "." et ".."
        {
            continue;
        }
        char chemin_complet[1024];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", dossier, fichieroudossier->d_name);

        // Crée le chemin complet du dossier dans /Save
        char chemin_save[1024];
        unsigned char md5[EVP_MAX_MD_SIZE];
        unsigned int md5_len;

        struct stat st;
        if (stat(chemin_complet, &st) == 0 && S_ISDIR(st.st_mode))
        {
            char chemin_save_case[1024];
            char *md5 = get_md5_of_directory_name(chemin_complet);
            FILE *l = fopen("testmd5.txt", "w");
            fprintf(l, "%s", md5);
            fclose(l);
            snprintf(chemin_save_case, sizeof(chemin_save), "%s/%s", dossier_save, md5);

            printf("chemin save case : %s\n", chemin_save_case);
            printf("chemin complet : %s\n", chemin_complet);
            printf("nom du fichier : %s\n", fichieroudossier->d_name);
            printf("md5 : %s\n", md5);
            printf("md5_len : %d\n", md5_len);

            creer_dossier(chemin_save_case);
            parcourir_dossier(chemin_complet, chemin_save_case);
        }
        else
        {
            compute_md5_file(fichieroudossier->d_name, md5, &md5_len);
            snprintf(chemin_save, sizeof(chemin_save), "%s/%s", dossier_save, md5);

            printf("chemin save : %s\n", chemin_save);
            printf("chemin complet : %s\n", chemin_complet);
            printf("nom du fichier : %s\n", fichieroudossier->d_name);
            printf("md5 : %s\n", md5);
            printf("md5_len : %d\n", md5_len);
            Chunk chunks[100]; // faut le faire dynamiquement
            int Nchunk = compute_chunk(fichieroudossier->d_name, chemin_complet, chunks);
            sauvegarder(chunks, Nchunk, fichieroudossier->d_name, chemin_save);
        }
    }
    closedir(dir);
}