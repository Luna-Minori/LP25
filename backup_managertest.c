#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Chunk
{
    char *data;
    char MD5[33];
    int index;
    int version;
} Chunk;

int existe_deja(const char *md5, FILE *file)
{
    char ligne[1024];
    rewind(file); // Rewind pour lire depuis le début du fichier
    while (fgets(ligne, sizeof(ligne), file))
    {
        // Si une ligne commence par le MD5 du chunk, il existe déjà
        if (strncmp(ligne, md5, 32) == 0)
        {
            return 1; // Trouvé, il existe déjà
        }
    }
    return 0; // Pas trouvé
}

void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier)
{
    char filename[256];

    if (!nom_fichier)
    {
        snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", "default");
        FILE *file = fopen(filename, "w"); // Créer un nouveau fichier en mode écriture
        
        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }
        fclose(file);
        return;
    }

    snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", nom_fichier);
    FILE *file = fopen(filename, "a+"); // Ouvrir le fichier en mode lecture-écriture

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char ligne[1024];
    long pos;  // Position de la ligne à remplacer

    for (int i = 0; i < nombre_de_chunks; i++)
    {
        if (!existe_deja(chunks[i].MD5, file)) // Si le MD5 n'existe pas déjà
        {
            fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
        }
        else
        {
            rewind(file); // Rewind pour lire depuis le début du fichier
            while (fgets(ligne, sizeof(ligne), file))
            {
                pos = ftell(file); 

                // Si on trouve le MD5 du chunk, on remplace la ligne
                if (strncmp(ligne, chunks[i].MD5, 32) == 0)
                {
                	fgets(ligne, sizeof(ligne), file);
                    if (strncmp(ligne, chunks[i].data, 32) != 0){
                        fseek(file, pos, SEEK_SET); 
                        fprintf(file, "%s\n", chunks[i].data);
                        fflush(file); 
                        break;
                    }
                }
            }
        }
    }

    fclose(file);
}

void write_backup(char *nom_fichier){

    if (!nom_fichier)
    {
        snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", "default");
        FILE *file = fopen(filename, "w"); // Créer un nouveau fichier en mode écriture
        
        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }
        fclose(file);
        return;
    }

    snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", nom_fichier);
    FILE *file = fopen(filename, "a+"); // Ouvrir le fichier en mode lecture-écriture

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char ligne[1024];
    long pos;  // Position de la ligne à remplacer

    for (int i = 0; i < nombre_de_chunks; i++)
    {
        if (!existe_deja(chunks[i].MD5, file)) // Si le MD5 n'existe pas déjà
        {
            fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
        }
        else
        {
            rewind(file); // Rewind pour lire depuis le début du fichier
            while (fgets(ligne, sizeof(ligne), file))
            {
                pos = ftell(file); // Enregistrer la position actuelle

                // Si on trouve le MD5 du chunk, on remplace la ligne
                if (strncmp(ligne, chunks[i].MD5, 32) == 0)
                {
                	fgets(ligne, sizeof(ligne), file);
                    if (strncmp(ligne, chunks[i].data, 32) != 0){
                        fseek(file, pos, SEEK_SET); // Revenir à la position de la ligne à remplacer
                        fprintf(file, "%s\n", chunks[i].data); // Remplacer la ligne avec les nouvelles données
                        fflush(file); // S'assurer que les données sont écrites immédiatement
                        break;
                    }
                }
            }
        }
    }

    fclose(file);
}

void write_backup(char *nom_fichier){

    if (!nom_fichier)
    {
        snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", "default");
        FILE *file = fopen(filename, "w"); // Créer un nouveau fichier en mode écriture
        
        if (file == NULL)
        {
            perror("Erreur de création du fichier");
            return;
        }
        fclose(file);
        return;
    }

    snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", nom_fichier);
    FILE *file = fopen(filename, "a+"); // Ouvrir le fichier en mode lecture-écriture

    if (file == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char ligne[1024];
    long pos;  // Position de la ligne à remplacer

    for (int i = 0; i < nombre_de_chunks; i++)
    {
        if (!existe_deja(chunks[i].MD5, file)) // Si le MD5 n'existe pas déjà
        {
            fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
        }
        else
        {
            rewind(file); // Rewind pour lire depuis le début du fichier
            while (fgets(ligne, sizeof(ligne), file))
            {
                pos = ftell(file); // Enregistrer la position actuelle

                // Si on trouve le MD5 du chunk, on remplace la ligne
                if (strncmp(ligne, chunks[i].MD5, 32) == 0)
                {
                	fgets(ligne, sizeof(ligne), file);
                    if (strncmp(ligne, chunks[i].data, 32) != 0){
                        fseek(file, pos, SEEK_SET); // Revenir à la position de la ligne à remplacer
                        fprintf(file, "%s\n", chunks[i].data); // Remplacer la ligne avec les nouvelles données
                        fflush(file); // S'assurer que les données sont écrites immédiatement
                        break;
                    }
                }
            }
        }
    }

    fclose(file);
}

int main()
{
    Chunk chunks[2] = {
        {"data5", "d41d8cd98f00b204e9800998ecf8427e"},
        {"data6", "d41d8cd98f00b204e9800998ecf8428e"}};

    // Exemple de nom de fichier de base
    char *nom_fichier = "fichier_test";

    sauvegarder(chunks, 2, nom_fichier);
    return 0;
}

