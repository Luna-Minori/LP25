#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define CHUNK_SIZE 4096

int read_case_backup(char *path, char *file_md5) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("Erreur d'ouverture du dossier");
        return -1;
    }
    file_md5 = strcat(file_md5, ".txt");
    while ((entry = readdir(dp)) != NULL) {
        // Comparer le nom du fichier avec le fichier recherché
        if (strcmp(entry->d_name, file_md5) == 0) {
            return 1;
        }
    }
    closedir(dp);
    return 0; // Fichier non trouvé
}

void md5_chunk(const unsigned char *data, size_t length, unsigned char *output) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erreur d'initialisation de EVP_MD_CTX");
        return;
    }

    // Initialisation de l'algorithme MD5
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erreur d'initialisation de MD5");
        EVP_MD_CTX_free(mdctx);
        return;
    }

    // Mise à jour du MD5 avec les données
    if (EVP_DigestUpdate(mdctx, data, length) != 1) {
        perror("Erreur de mise à jour du MD5");
        EVP_MD_CTX_free(mdctx);
        return;
    }

    // Finalisation du calcul du MD5
    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, output, &md_len) != 1) {
        perror("Erreur de finalisation du MD5");
    }

    EVP_MD_CTX_free(mdctx);  // Libérer le contexte
}

void md5_file(const char *filename, unsigned char *output) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    unsigned char buffer[1024];  // Tampon de lecture (taille de chunk)
    size_t bytes_read;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erreur d'initialisation de EVP_MD_CTX");
        fclose(file);
        return;
    }

    // Initialisation de l'algorithme MD5
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erreur d'initialisation de MD5");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return;
    }

    // Lire le fichier par morceaux et mettre à jour le MD5
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            perror("Erreur de mise à jour du MD5");
            break;
        }
    }

    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, output, &md_len) != 1) {
        perror("Erreur de finalisation du MD5");
    }

    EVP_MD_CTX_free(mdctx);  // Libérer le contexte
    fclose(file);  // Fermer le fichier
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
    size_t chunk_size = 1024;
    while (total_read < file_size)
    {
        size_t bytes_to_read = (file_size - total_read < chunk_size) ? file_size - total_read : chunk_size;
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

void read_file_in_chunks(char *filename)
{
    char *file_content = read_file(filename);
    if (file_content == NULL)
    {
        return;
    }

    size_t file_size = strlen(file_content);

    size_t total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (size_t i = 0; i < total_chunks; ++i)
    {
        size_t chunk_start = i * CHUNK_SIZE;
        size_t chunk_end = (i + 1) * CHUNK_SIZE;
        if (chunk_end > file_size)
        {
            chunk_end = file_size;
        }

        printf("Chunk %zu (%zu octets) :\n", i + 1, chunk_end - chunk_start);
        fwrite(file_content + chunk_start, 1, chunk_end - chunk_start, stdout);
        printf("\n---\n");
    }
    free(file_content);
}


// Fonction pour reconstruire le fichier
void recup_save_content(char *path)
{
    char *file_name;
    FILE file;
    int test_file = 0;
    unsigned char hash[4096];
    char *filename = "/home/luna/Bureau/test.txt";
    md5_file(filename, hash);
    printf("MD5 du fichier : ");
    test_file = read_case_backup(path, hash);
    if (test_file == 1)
    {
        file_name = strcat("Save/", path);
        file_name = strcat(file_name, ".txt");
        FILE *file = fopen(file_name, "rb");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);
        char *content = (char *)malloc(file_size + 1);
        content = read_file(file_name);
        printf("%s", content);
    }
    /*
    char line[MAX_LINE_LENGTH];
    char md5[33];
    int index, version;
    char data[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        // Lire les métadonnées : md5;index;version
        if (sscanf(line, "%32[^;];%d;%d", md5, &index, &version) == 3) {
            // Vérifier si le MD5 et la version correspondent
            if (strcmp(md5, target_md5) == 0 && version == target_version) {
                // Lire la ligne suivante pour obtenir le chunk
                if (fgets(data, sizeof(data), fp)) {
                    data[strcspn(data, "\n")] = '\0'; // Retirer le saut de ligne
                    add_chunk(list, index, data); // Ajouter le chunk à la liste
                }
            }
        }
    }
    // Trier les chunks par index
    qsort(chunks, chunk_count, sizeof(Chunk), compare_chunks);

    // Écrire les chunks triés dans le fichier de sortie
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        return;
    }

    for (int i = 0; i < chunk_count; i++) {
        fputs(chunks[i].data, output);
        free(chunks[i].data); // Libérer la mémoire utilisée
    }

    fclose(output);
    printf("Fichier reconstruit avec succes : %s\n", output_file);
}

// Fonction principale pour tester
int main() {
    const char *input_txt = "chunks.txt";   // Fichier contenant les chunks
    const char *output_file = "output.txt"; // Fichier reconstruit
    const char *target_md5 = "123456789abcdef123456789abcdef12"; // MD5 cible
    int target_version = 1;                 // Version cible

    reconstruct_file(input_txt, output_file, target_md5, target_version);
*/
    return;
}


int main()
{
    //printf("Lecture du fichier en chunks de %d octets\n", CHUNK_SIZE);
    char *filename = "test.txt";
    recup_save_content(filename);
    //read_file_in_chunks(filename);
    return 0;
}
