#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include <openssl/md5.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "file_handler.h"
#include "file_modifier.h"
#include "backup_manager.h"

#define HASH_TABLE_SIZE 4096
#define MAX_PATH 1024

void reset_str(char *str)
{ // Vide la chaine de caractère
    while (*str != '\0')
    {
        *str = '\0';
        ++str;
    }
}

void compute_md5_file(const char *filename, unsigned char *md5, unsigned int *md5_len)
{ // genere un md5
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (!context)
    {
        fprintf(stderr, "Erreur : impossible d'initialiser EVP_MD_CTX\n");
        exit(1);
    }

    if (EVP_DigestInit_ex(context, EVP_md5(), NULL) != 1)
    {
        fprintf(stderr, "Erreur : EVP_DigestInit_ex a échoué\n");
        EVP_MD_CTX_free(context);
        exit(1);
    }

    EVP_DigestUpdate(context, filename, strlen(filename));

    if (EVP_DigestFinal_ex(context, md5, md5_len) != 1)
    {
        fprintf(stderr, "Erreur : EVP_DigestFinal_ex a échoué\n");
        EVP_MD_CTX_free(context);
        exit(1);
    }

    EVP_MD_CTX_free(context);
}

void compute_md5_chunk(char *data, size_t taille, char *md5_string)
{
    EVP_MD_CTX *ctx;
    unsigned char md5_digest[EVP_MAX_MD_SIZE];
    unsigned int md5_length;

    ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        fprintf(stderr, "Erreur : impossible d'initialiser EVP_MD_CTX\n");
        return;
    }

    if (EVP_DigestInit_ex(ctx, EVP_md5(), NULL) != 1)
    {
        fprintf(stderr, "Erreur : EVP_DigestInit_ex a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    if (EVP_DigestUpdate(ctx, data, taille) != 1)
    {
        fprintf(stderr, "Erreur : EVP_DigestUpdate a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    if (EVP_DigestFinal_ex(ctx, md5_digest, &md5_length) != 1)
    {
        fprintf(stderr, "Erreur : EVP_DigestFinal_ex a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    EVP_MD_CTX_free(ctx);

    for (unsigned int i = 0; i < md5_length; i++)
    {
        sprintf(&md5_string[i * 2], "%02x", md5_digest[i]);
    }
    md5_string[md5_length * 2] = '\0';
}

int compute_chunk(char *nom_fichier, char *path, Chunk *chunks)
{
    FILE *fichier_entree = fopen(nom_fichier, "rb");
    if (fichier_entree == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return -1;
    }

    char chunk_data[10][4096] = {0};
    int index_data = 0;
    int chunk_index = 0;
    char *contenu = read_file(nom_fichier);
    if (contenu == NULL)
    {
        perror("Erreur lors de la lecture du fichier");
        fclose(fichier_entree);
        return -1;
    }
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    compute_md5_file(path, md5, &md5_len);

    while (contenu[chunk_index] != '\0')
    {
        if (chunk_index % 4096 == 0 && chunk_index != 0)
        {
            chunks[index_data].index = index_data + 1;
            chunks[index_data].version = 1;
            chunks[index_data].data = strdup(chunk_data[index_data]);
            compute_md5_chunk(chunk_data[index_data], strlen(chunk_data[index_data]), chunks[index_data].MD5);
            memset(chunk_data[index_data], 0, sizeof(chunk_data[index_data]));
            index_data++;
        }
        strncat(chunk_data[index_data], &contenu[chunk_index], 1);
        chunk_index++;
    }

    if (chunk_index % 4096 != 0)
    {
        chunks[index_data].index = index_data + 1;
        chunks[index_data].version = 1;
        chunks[index_data].data = strdup(chunk_data[index_data]);
        compute_md5_chunk(chunk_data[index_data], strlen(chunk_data[index_data]), chunks[index_data].MD5);
    }

    fclose(fichier_entree);
    free(contenu);
    return index_data + 1;
}

void compute_md5_case(const char *dir_path, unsigned char *md5_out) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Erreur lors de l'ouverture du répertoire");
        return;
    }

    EVP_MD_CTX *md5_ctx = EVP_MD_CTX_new();
    if (!md5_ctx) {
        perror("Erreur lors de la création du contexte EVP");
        closedir(dir);
        return;
    }

    if (EVP_DigestInit_ex(md5_ctx, EVP_md5(), NULL) != 1) {
        perror("Erreur lors de l'initialisation du contexte MD5");
        EVP_MD_CTX_free(md5_ctx);
        closedir(dir);
        return;
    }

    struct dirent *entry;
    char full_path[MAX_PATH];
    char **file_names = malloc(sizeof(char*) * 1000);
    int file_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) == 0) {
            file_names[file_count] = strdup(entry->d_name);
            file_count++;
        }
    }

    qsort(file_names, file_count, sizeof(char*), (int (*)(const void*, const void*))strcmp);
    for (int i = 0; i < file_count; i++) {
        EVP_DigestUpdate(md5_ctx, file_names[i], strlen(file_names[i]));
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, file_names[i]);
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            compute_md5_case(full_path, md5_out); 
        }
        free(file_names[i]);
    }

    unsigned int len = EVP_MD_size(EVP_md5());
    EVP_DigestFinal_ex(md5_ctx, md5_out, &len);

    EVP_MD_CTX_free(md5_ctx);
    closedir(dir);
    free(file_names);
}