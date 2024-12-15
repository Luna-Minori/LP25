#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <libgen.h>
#include "file_modifier.h"
#include "file_handler.h"
#include "backup_manager.h"

#define CHUNK_SIZE 4096

int element_dans_liste(int element, int *liste, int taille)
{
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
{
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
    lines[num_lines] = NULL; // Add null terminator

    fclose(file);
    return lines;
}

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

int read_savefile_in_chunks(char *filename, char *path, Chunk *chunks)
{
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);

    char md5_fichier[33];
    for (int i = 0; i < 16; ++i)
    {
        snprintf(&md5_fichier[i * 2], 3, "%02x", md5[i]);
    }
    char nom_fichier_sauvegarde[50];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", md5_fichier);

    char **file_content = read_file_lines(nom_fichier_sauvegarde);
    if (file_content == NULL)
    {
        return 0;
    }
    /*
    int nombre_caractere = strlen(read_file(nom_fichier_sauvegarde));
    int nombre_chunks = (floor(nombre_caractere/4096))+1;
    */
    FILE *file = fopen(nom_fichier_sauvegarde, "rb");
    char ligne[4096];
    int nombre_chunks = 0;

    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[32] == ';')
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
    for (int i = 0; i < nombre_chunks; i++)
    {
        Chunk chunk;

        char ligne[4096] = {};

        if (file_content[k][32] == ';')
        {

            strncpy(chunk.MD5, file_content[k], 32);
            char temp_index[16] = {};
            int h = 33;
            while (file_content[k][h] != ';')
            {
                char temp[2] = {file_content[k][h], '\0'};
                strncat(temp_index, temp, 1);
                h++;
            }
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

        while (k < nombre_lignes && file_content[k][32] != ';')
        {
            pointeur = 0;
            while (pointeur < strlen(file_content[k]))
            {
                strncat(ligne, &file_content[k][pointeur], 1);
                pointeur++;
            }
            k++;
        }
        if (pointeur > 4096)
        {
            pointeur = 0;
            k--;
        }

        chunk.data = strdup(ligne);
        chunks[i] = chunk;
    }
    free(file_content);
    return nombre_chunks;
}

void recup_save_content(char *nom_fichier, char *path, int version)
{
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);

    char md5_fichier[33];
    for (int i = 0; i < 16; ++i)
    {
        snprintf(&md5_fichier[i * 2], 3, "%02x", md5[i]);
    }
    char nom_fichier_sauvegarde[50];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", md5_fichier);

    if (access(nom_fichier_sauvegarde, F_OK) == -1)
    {
        perror("Erreur: Le fichier de sauvegarde n'existe pas\n");
        printf("nom du fichier de sauvegarde :%s\n", nom_fichier_sauvegarde);
        return;
    }
    FILE *file = fopen(nom_fichier_sauvegarde, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1L)
    {
        perror("Erreur: Taille du fichier de sauvegarde inconnue");
        fclose(file);
        return;
    }
    rewind(file);
    fclose(file);

    Chunk chunks[file_size];
    read_savefile_in_chunks(nom_fichier_sauvegarde, path, chunks);
    char nom_fichier_temp[80];
    snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_temp.txt", nom_fichier_sauvegarde);

    FILE *temp_file = fopen(nom_fichier_temp, "wb");
    if (temp_file == NULL)
    {
        perror("Erreur: Impossible de créer le fichier temporaire");
        return;
    }

    int i = 0;
    int index_max = 0;
    while (chunks[i].data != NULL)
    {
        if (chunks[i].index > index_max)
        {
            index_max = chunks[i].index;
        }
        i++;
    }

    int index_versions_proches[index_max + 1];
    for (int j = 0; j < index_max; j++)
    {
        index_versions_proches[j] = -1;
    }

    i = 0;
    while (chunks[i].data != NULL)
    {
        if (index_versions_proches[(chunks[i].index - 1)] == -1)
        {
            index_versions_proches[(chunks[i].index - 1)] = i;
        }

        else
        {
            if (chunks[index_versions_proches[(chunks[i].index) - 1]].version < chunks[i].version && chunks[i].version <= version)
            {
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

    // Remplacer l'ancien fichier par le fichier temporaire
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
