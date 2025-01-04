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



char* get_md5_of_directory_name(const char *dir_path) {
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
    char *md5_string = malloc(33);

    if (!md5_string) {
        perror("Erreur lors de l'allocation de mémoire pour md5_string");
        return NULL;
    }

    compute_md5_file(dir_path, md5, &md5_len);

    for (unsigned int i = 0; i < md5_len; i++) {
        sprintf(&md5_string[i * 2], "%02x", md5[i]);
    }
    md5_string[md5_len * 2] = '\0';

    return md5_string;
}

int main(){
    
    char *md5 = get_md5_of_directory_name("/home/Max/LP25/Save/");

    if (md5) {
        printf("MD5 du dossier: %s\n", md5);
        free(md5);
    }
    else {
        fprintf(stderr, "Erreur lors du calcul du MD5 du dossier\n");
    }

    return 0;
}