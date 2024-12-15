// backup_manager.h
#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

#include "file_modifier.h"


void copier_fichier(FILE *source, FILE *dest, int ligne_debut);
int existe_deja_index(int index, FILE *file);
int existe_deja_version(int version, int index, FILE *file);
int existe_deja_md5(char *md5, int version, int index, FILE *file);
void sauvegarder(Chunk *chunks, int nombre_de_chunks, char *nom_fichier, char *path);

#endif // BACKUP_MANAGER_H