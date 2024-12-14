#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <string.h>

#define HASH_TABLE_SIZE 4096

unsigned int hash_md5(unsigned char *md5) {
    unsigned int hash = 0;
    for (int i = 0; i < EVP_MD_size(EVP_md5()); i++) {
        hash = (hash << 5) + hash + md5[i];
    }
    return hash % HASH_TABLE_SIZE;
}

int main() {
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    char *filename = "fichier_test.txt";
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_md5(), NULL);

    unsigned char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(context, buffer, bytes);
    }

    EVP_DigestFinal_ex(context, md5, &md5_len);
    EVP_MD_CTX_free(context);
    fclose(file);

    printf("MD5 hash: ");
    for (unsigned int i = 0; i < md5_len; i++) {
        printf("%02x", md5[i]);
    }
    printf("\n");

    return 0;
}