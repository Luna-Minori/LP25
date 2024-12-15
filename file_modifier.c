#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <libgen.h>
#include "file_modifier.h"
#include "file_handler.h"
#include "backup_manager.h"


#define CHUNK_SIZE 4096

int element_dans_liste(int element, int *liste, int taille) {
    for (int i = 0; i < taille; i++) {
        if (liste[i] == element) {
            return 1;
        }
    }
    return 0;
}


char **read_file_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur: Impossible d'ouvrir le fichier");
        return NULL;
    }

    char **lines = NULL;
    char buffer[1024];
    int num_lines = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        lines = realloc(lines, (num_lines + 1) * sizeof(char *));
        if (lines == NULL) {
            perror("Erreur: Allocation mémoire échouée");
            fclose(file);
            return NULL;
        }
        lines[num_lines] = strdup(buffer);
        if (lines[num_lines] == NULL) {
            perror("Erreur: Allocation mémoire échouée");
            fclose(file);
            return NULL;
        }
        num_lines++;
    }

    lines = realloc(lines, (num_lines + 1) * sizeof(char *));
    if (lines == NULL) {
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


int read_savefile_in_chunks(char *filename, Chunk *chunks)
{
    printf("%s\n", filename);
    char **file_content = read_file_lines(filename);
    if (file_content == NULL)
    {
        return 0;
    }

    int nombre_lignes = 0;
    while (file_content[nombre_lignes] != NULL) {
        nombre_lignes++;
    }

    int nombre_chunks = (nombre_lignes - 1) / 2;

    for (int i = 0; i < nombre_chunks; i++)
    {
        Chunk chunk;
        char ligne[4096] = {};
        strncpy(ligne, file_content[i * 2 + 1], sizeof(ligne) - 1); // Dereference file_content
        chunk.data = strdup(file_content[i * 2 + 2]); // Copy the data

        strncpy(chunk.MD5, ligne, 32);
        char temp_index[16] = {};
        int k = 33;
        while (ligne[k] != ';')
        {
            char temp[2] = {ligne[k], '\0'};
            strncat(temp_index, temp, 1);
            k++;
        }
        chunk.index = atoi(temp_index);
        char temp_version[16] = {};
        k++;
        while (ligne[k] != '\0')
        {
            char temp[2] = {ligne[k], '\0'};
            strncat(temp_version, temp, 1);
            k++;
        }
        chunk.version = atoi(temp_version);

        chunks[i] = chunk;
    }
    free(file_content);
    return 1;
}


void recup_save_content(char *nom_fichier, int version){
    unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
	compute_md5_file(nom_fichier, md5, &md5_len);
    
	char md5_fichier[33];
    for (int i = 0; i < 16; ++i) {
        snprintf(&md5_fichier[i*2], 3, "%02x", md5[i]);
    }
    char nom_fichier_sauvegarde[50];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", md5_fichier);
    
    if (access(nom_fichier_sauvegarde, F_OK) == -1){
        perror("Erreur: Le fichier de sauvegarde n'existe pas\n");
        printf("nom du fichier de sauvegarde :%s\n", nom_fichier_sauvegarde);
        return;
    }
    FILE *file = fopen(nom_fichier_sauvegarde, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1L) {
        perror("Erreur: Taille du fichier de sauvegarde inconnue");
        fclose(file);
        return;
    }
    rewind(file);
    printf("nom du fichier de sauvegarde :%s\n", nom_fichier_sauvegarde);
    printf("taille du fichier de sauvegarde : %ld\n", file_size);
    fclose(file);

    Chunk chunks[file_size];

    read_savefile_in_chunks(nom_fichier_sauvegarde, chunks);

    printf("%s\n", chunks[0].data);
    printf("passe1\n");

    char nom_fichier_temp[80];
    snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_temp.txt", nom_fichier_sauvegarde);

    FILE *temp_file = fopen(nom_fichier_temp, "wb");
    if (temp_file == NULL) {
        perror("Erreur: Impossible de créer le fichier temporaire");
        return;
    }
    printf("passe2\n");

    int i = 0;
    int index_max = 0;
    while (chunks[i].data != NULL) {      
        if (chunks[i].index > index_max) {
            index_max = chunks[i].index;
        }
        i++;
    }
    printf("passe3\n");
    int index_versions_proches[index_max+1];
    printf("passe35\n");
    for (int j = 0; j < index_max; j++) {
        index_versions_proches[j] = -1;
    }
    printf("passe4\n");

    i = 0;
    while (chunks[i].data != NULL) {
        
        if (index_versions_proches[(chunks[i].index-1)] == -1){
            index_versions_proches[(chunks[i].index-1)] = i;
        }

        else {
            if (chunks[index_versions_proches[(chunks[i].index)-1]].version < chunks[i].version && chunks[index_versions_proches[(chunks[i].index)-1]].version <= version) {
                index_versions_proches[(chunks[i].index)-1] = i;
            }
        }
        i++;
    }

    for (int k = 0; k < index_max; k++) {
        fprintf(temp_file, "%s",chunks[index_versions_proches[k]].data);
    }

    fclose(temp_file);

    // Remplacer l'ancien fichier par le fichier temporaire
    if (remove(nom_fichier) != 0) {
        perror("Erreur de suppression de l'ancien fichier");
        return;
    }

    if (rename(nom_fichier_temp, nom_fichier) != 0) {
        perror("Erreur de renommage du fichier temporaire");
        return;
    }

}
