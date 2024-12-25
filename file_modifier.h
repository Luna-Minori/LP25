#ifndef FILE_MODIFIER_H
#define FILE_MODIFIER_H


#define CHUNK_SIZE 4096

typedef struct Chunk{
    char *data;
    char MD5[33];
    int index;
    int version;
} Chunk;

int element_dans_liste(int element, int *liste, int taille);
char **read_file_lines(const char *filename);
char *read_file(char *filename);
int read_savefile_in_chunks(char *filename, char *path, Chunk *chunks);
void recup_save_content(char *nom_fichier, char *path, int version);
void parcourir_dossier(char *dossier, char *dossier_save);
void creer_dossier(const char *chemin_dossier);

#endif // FILE_MODIFIER_H