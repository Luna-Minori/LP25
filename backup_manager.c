#include <string.h>
#include <openssl/md5.h>
#include <stdio.h>

typedef struct Chunk
{
	char *data;
	char MD5[33];
} Chunk;

int existe_deja(const char *md5, FILE *file)
{
	char ligne[4096];
	rewind(file);
	while (fgets(ligne, sizeof(ligne), file))
	{
        	if (strncmp(ligne, md5, 32) == 0)
        	{
            		return 1;
        	}
	}
	return 0;
}

void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier)
{
	char filename[256];
	snprintf(filename, sizeof(filename), "%s_sauvegarde.txt", nom_fichier);
    	FILE *file = fopen(filename, "a+");

    	if (file == NULL)
    	{
        	perror("Erreur d'ouverture du fichier");
        	return;
    	}

	char ligne[4096];
    	long pos;

    	for (int i = 0; i < nombre_de_chunks; i++)
    	{
        	if (!existe_deja(chunks[i].MD5, file))
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
                    			
					// Revenir à la position où la ligne commence et remplacer la ligne
                    			fseek(file, pos - strlen(ligne), SEEK_SET);
                    			fprintf(file, "%s;%d\n%s\n", chunks[i].MD5, i, chunks[i].data);
                   			break; // Sortir de la boucle une fois la ligne remplacée
                		}
            		}
        	}
    	}

	fclose(file);
}

int main()
{
	Chunk chunks[2] = {{"data10", "d41d8cd98f00b204e9800998ecf8427e"},{"data2", "d41d8cd98f00b204e9800998ecf8428e"}};
	char *fichier="test.txt";
	sauvegarder(chunks, 2, fichier);
	return 0;
}

