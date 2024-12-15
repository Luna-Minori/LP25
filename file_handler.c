#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <string.h>
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
void compute_md5_file(const char *filename, unsigned char *md5, unsigned int *md5_len) { // genere un md5
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        exit(1);
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_md5(), NULL);

    unsigned char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(context, buffer, bytes);
    }

    EVP_DigestFinal_ex(context, md5, md5_len);
    EVP_MD_CTX_free(context);
    fclose(file);
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


void compute_chunk(const char *nom_fichier, Chunk *chunks) { // découpe un fichier en chunk
    FILE *fichier_entree = fopen(nom_fichier, "rb");
    if (fichier_entree == NULL) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return;
    }

    char chunk[HASH_TABLE_SIZE];
    size_t bytes_lus;
    int chunk_index = 0;
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
	compute_md5_file(nom_fichier, md5, &md5_len);

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

