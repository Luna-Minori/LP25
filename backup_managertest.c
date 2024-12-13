#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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

void copier_fichier(FILE *source, FILE *dest) {
    
    rewind(source);
    rewind(dest);

    char buffer[1024];
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
    while (fgets(ligne, sizeof(ligne), file))
    {
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
    return 0; // Pas trouvé
}

int existe_deja_version(int version, int index, FILE *file)
{
    char ligne[4096];
    int version_ligne=-1;
    int index_ligne=-1;
    rewind(file); // Rewind pour lire depuis le début du fichier
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
    char nom_fichier_sauvegarde[256];
    snprintf(nom_fichier_sauvegarde, sizeof(nom_fichier_sauvegarde), "%s_sauvegarde.txt", nom_fichier);

    if (!nom_fichier)
    {
        FILE *file = fopen(nom_fichier_sauvegarde, "w");
        
        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }
        fclose(file);
        return;
    }

    FILE *file = fopen(nom_fichier_sauvegarde, "a+"); // Ouvrir le fichier en mode lecture-écriture

    printf("Ouverture du fichier %s\n", nom_fichier_sauvegarde);

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char ligne[4096];
    long pos;  // Position de la ligne à remplacer

    char nom_fichier_temp[256];
    snprintf(nom_fichier_temp, sizeof(nom_fichier_temp), "%s_sauvegarde_tmp.txt", nom_fichier);

    FILE *temp_file = fopen(nom_fichier_temp, "w");
    if (temp_file == NULL) {
        perror("Erreur de création du fichier temporaire");
        fclose(file);
        return;
    }

    copier_fichier(file, temp_file);

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



int main()
{
    Chunk un = {"data5", "d41d8cd98f00b204e9800998ecf8427e", 2, 1};
    Chunk deux = {"data6", "d41d8cd98f00b204e9800998ecf8428e", 2, 3};

    Chunk chunks[2] = {un, deux};


    char *nom_fichier = "fichier_test";

    sauvegarder(chunks, 2, nom_fichier);
    return 0;
}

