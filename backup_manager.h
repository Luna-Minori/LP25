// backup_manager.h
#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

typedef struct Chunk {
    char *data;
    char MD5[33];
    int index;
    int version;
} Chunk;

void copier_fichier(FILE *source, FILE *dest);
int existe_deja_index(int index, FILE *file);
int existe_deja_version(int version, int index, FILE *file);
void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier);

#endif // BACKUP_MANAGER_H