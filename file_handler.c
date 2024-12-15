#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include "file_handler.h"
#include "file_modifier.h"
#include "backup_manager.h"

#define HASH_TABLE_SIZE 4096
/*
unsigned int hash_md5(unsigned char *md5) {
    unsigned int hash = 0;
    for (int i = 0; i < EVP_MD_size(EVP_md5()); i++) {
        hash = (hash << 5) + hash + md5[i];
    }
    return hash % HASH_TABLE_SIZE;
}
*/

void reset_str(char *str){  // Vide la chaine de caractère
  while(*str != '\0'){
    *str = '\0';
    ++str;
  }
}

void compute_md5_file(const char *filename, unsigned char *md5, unsigned int *md5_len) { // genere un md5
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (!context) {
        fprintf(stderr, "Erreur : impossible d'initialiser EVP_MD_CTX\n");
        exit(1);
    }

    if (EVP_DigestInit_ex(context, EVP_md5(), NULL) != 1) {
        fprintf(stderr, "Erreur : EVP_DigestInit_ex a échoué\n");
        EVP_MD_CTX_free(context);
        exit(1);
    }

    EVP_DigestUpdate(context, filename, strlen(filename));

    if (EVP_DigestFinal_ex(context, md5, md5_len) != 1) {
        fprintf(stderr, "Erreur : EVP_DigestFinal_ex a échoué\n");
        EVP_MD_CTX_free(context);
        exit(1);
    }

    EVP_MD_CTX_free(context);
}

void compute_md5_chunk(char *data, size_t taille, char *md5_string) {
    EVP_MD_CTX *ctx;
    unsigned char md5_digest[EVP_MAX_MD_SIZE];
    unsigned int md5_length;

    ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Erreur : impossible d'initialiser EVP_MD_CTX\n");
        return;
    }

    if (EVP_DigestInit_ex(ctx, EVP_md5(), NULL) != 1) {
        fprintf(stderr, "Erreur : EVP_DigestInit_ex a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    if (EVP_DigestUpdate(ctx, data, taille) != 1) {
        fprintf(stderr, "Erreur : EVP_DigestUpdate a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    if (EVP_DigestFinal_ex(ctx, md5_digest, &md5_length) != 1) {
        fprintf(stderr, "Erreur : EVP_DigestFinal_ex a échoué\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    EVP_MD_CTX_free(ctx);

    for (unsigned int i = 0; i < md5_length; i++) {
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
        return-1;
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
    printf("Découpage terminé. %d chunks lus.\n", index_data + 1);

    return index_data + 1;
}

    

