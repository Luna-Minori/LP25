#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <unistd.h>
#include "file_handler.h"


#define PRINT_TYPE(var) _Generic((var), \
    int: "int", \
    float: "float", \
    double: "double", \
    char: "char", \
    char*: "char*", \
    default: "unknown")

typedef struct Chunk
{
    char *data;
    char MD5[33];
    int index;
    int version;
} Chunk;

void copier_fichier(FILE *source, FILE *dest, int ligne_debut) {
    
    rewind(source);
    //rewind(dest);

    for (int i = 0; i < ligne_debut; i++)
    {
        char ligne[4096];
        fgets(ligne, sizeof(ligne), source);
        printf("Ligne %d : %s\n", i, ligne);
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
}

int existe_deja_index(int index, FILE *file)
{
    char ligne[4096];
    int index_ligne=-1;
    rewind(file); // Rewind pour lire depuis le début du fichier
    fgets(ligne, sizeof(ligne), file);
    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[0] != '\n' || strlen(ligne) > 33) {
            if (ligne[32]==';'){
                char temp_index[16]={};
                int i = 33;
                while (ligne[i] != ';'){
                    char temp[2] = {ligne[i]};
                    strncat(temp_index, temp, 1);
                    i++;
                }
                index_ligne = atoi(temp_index);
            }
            if (index == index_ligne)
            {
                return 1; // Trouvé
            }
        }
    }
    return 0; // Pas trouvé
}

int existe_deja_version(int version, int index, FILE *file)
{
    char ligne[4096];
    int version_ligne=-1;
    int index_ligne=-1;
    rewind(file); // Rewind pour lire depuis le début du fichier
    fgets(ligne, sizeof(ligne), file);  
    while (fgets(ligne, sizeof(ligne), file))
    {
        if (ligne[0] != '\n' || strlen(ligne) > 33) {
            
            if (ligne[32]==';'){
                char temp_version[16]={};
                char temp_index[16]={};
                int i = 33;
                while (ligne[i] != ';'){
                    char temp[2] = {ligne[i]};
                    strncat(temp_index, temp, 1);
                    i++;
                }
                index_ligne = atoi(temp_index);

                if (index_ligne == index){
                    
                
                    i++;
                    while (ligne[i] != '\0'){
                        char temp[2] = {ligne[i]};
                        strncat(temp_version, temp, 1);
                        i++;
                    }
                    version_ligne = atoi(temp_version);
                }
            }
            if (version == version_ligne)
            {
                return 1; // Trouvé
            }
        }
    }
    return 0; // Pas trouvé
}

void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier)
{
	unsigned char md5[EVP_MAX_MD_SIZE];
    unsigned int md5_len;
	compute_md5(nom_fichier, md5, &md5_len);
	char md5_fichier[33];
    for (int i = 0; i < 16; ++i) {
        snprintf(&md5_fichier[i*2], 3, "%02x", md5[i]);
    }
	
	char nom_fichier_sauvegarde[50];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", md5_fichier);

	printf("quoicoubeh\n");

    if (access(nom_fichier_sauvegarde, F_OK) == -1)
    {
        printf("Le fichier n'existe pas, on le crée\n");
        FILE *file = fopen(nom_fichier_sauvegarde, "w");
        
        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }

        fprintf(file, "%d\n",0);
        fclose(file);
        //return;
    }

    FILE *file = fopen(nom_fichier_sauvegarde, "a+"); // Ouvrir le fichier en mode lecture-écriture

    printf("Ouverture du fichier %s\n", nom_fichier_sauvegarde);

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }


    char nom_fichier_temp[60];
    snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_sauvegarde_tmp.txt", md5_fichier);

    FILE *temp_file = fopen(nom_fichier_temp, "w");
    if (temp_file == NULL) {
        perror("Erreur de création du fichier temporaire");
        fclose(file);
        return;
    }

    
    char version_temp[16]={};
    char ligne_version[16]={};
    fgets(ligne_version, sizeof(ligne_version), file);
    int k = 0;
    while (ligne_version[k] != '\0'){
        char tempV[2] = {ligne_version[k]};
        strncat(version_temp, tempV, 1);
        k++;
    }
    

    fprintf(temp_file, "%d\n",atoi(version_temp)+1);

    

    copier_fichier(file, temp_file, 1);

    for (int i = 0; i < nombre_de_chunks; i++)
    {   
        if (!existe_deja_index(chunks[i].index, file)) // Si le chunk n'existe pas, on l'ajoute
        {
            printf("l'index %d n'existe pas\n", chunks[i].index);
            fprintf(temp_file, "%s;%d;%d\n%s\n", chunks[i].MD5, chunks[i].index, chunks[i].version, chunks[i].data);
        }
        else
        {
            if (!existe_deja_version(chunks[i].version, chunks[i].index, file)) // Si la version n'existe pas, on l'ajoute
            {
                printf("la version %d n'existe pas\n", chunks[i].version);
                fprintf(temp_file, "%s;%d;%d\n%s\n", chunks[i].MD5, chunks[i].index, chunks[i].version, chunks[i].data);
            }
            
        }
    }

    fclose(file);
    fclose(temp_file);

    // Remplacer l'ancien fichier par le fichier temporaire
    if (remove(nom_fichier_sauvegarde) != 0) {
        perror("Erreur de suppression de l'ancien fichier");
        return;
    }

    if (rename(nom_fichier_temp, nom_fichier_sauvegarde) != 0) {
        perror("Erreur de renommage du fichier temporaire");
        return;
    }

    printf("Le fichier a été modifié avec succès.\n");
}


/*
int main()
{
    Chunk un = {"data1", "d41d8cd98f00b204e9800998ecf8427e", 1, 1}; //oui
    Chunk deux = {"data2", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // oui
	Chunk trois = {"data3", "d41d8cd98f00b204e9800998ecf8427e", 1, 2}; // oui
    Chunk quatre = {"data4", "d41d8cd98f00b204e9800998ecf8428e", 2, 1}; // non
	Chunk cinq = {"data5", "d41d8cd98f00b204e9800998ecf8427e", 2, 2}; // oui
    Chunk six = {"data6", "d41d8cd98f00b204e9800998ecf8428e", 3, 1}; // oui

    Chunk chunks1[5] = {un, deux, trois , cinq, six};
	Chunk chunks2[2] = {un, six};

    char *nom_fichier = "fichier_test";

    sauvegarder(chunks1, 5, nom_fichier);
	printf("ça c'eST FAIT\n");
	sauvegarder(chunks2, 2, nom_fichier);
    return 0;
}
*/

