#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <libgen.h>
#include "file_handler.h"
#include "backup_manager.h"
#include "file_modifier.h"



/*
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
*/
/*
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
*/

/*
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
*/

/*
void recup_save_content(char nom_fichier[256], int version){

}
*/
/*

// Fonction principale pour tester
int main() {
    const char *input_txt = "chunks.txt";   // Fichier contenant les chunks
    const char *output_file = "output.txt"; // Fichier reconstruit
    const char *target_md5 = "123456789abcdef123456789abcdef12"; // MD5 cible
    int target_version = 1;                 // Version cible

    reconstruct_file(input_txt, output_file, target_md5, target_version);

    return;
}

*/

int main()
{
    
    Chunk chunks[100];
    
    char *nom_fichier = "fichier_test.txt";
    char *path = "./fichier_test.txt";
    
    //int nombre_chunk = compute_chunk(nom_fichier, path, chunks);

    int nombre_chunk = read_savefile_in_chunks(nom_fichier, path, chunks);

    printf("nombre de chunks: %d\n", nombre_chunk);

    /*
    int i = 0;
    while (chunks[i].data != NULL) {
        printf("chunk index: %d\n", i);
        printf("data: %s\n", chunks[i].data);
        printf("md5: %s\n", chunks[i].MD5);
        printf("index: %d\n", chunks[i].index);
        printf("version: %d\n", chunks[i].version);
        i++;
    }
    */

    
    
    //sauvegarder(chunks, nombre_chunk, nom_fichier, path);
    
    //recup_save_content(nom_fichier, 1);


    /*

    printf("nombre de chunks: %d\n", nombre_chunk);
    printf("data: %s\n", chunks[0].data);
    printf("data: %s\n", chunks[1].data);
    printf("data: %s\n", chunks[2].data);

    FILE *file = fopen("e6889244e245c304fd9ab8fad96f4a90_sauvegarde.txt", "rb");
    char *md5 ="cd8c1d3b592a9520a56aeb9663987aca";

    printf("index: %d\n",existe_deja_index(3,file));  
    printf("version: %d\n",existe_deja_version(1,3,file));  
    printf("md5: %d\n",existe_deja_md5(md5,1,3,file));   

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
    */
    
    //read_file_in_chunks("fichier_test.txt");
    
    return 0;
}
