// file_handler.h
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "file_modifier.h"


void compute_md5_file(const char *filename, unsigned char *md5, unsigned int *md5_len);
void compute_chunk(const char *nom_fichier, Chunk *chunks);
void compute_md5_chunk(char *data, size_t taille, char *md5_string);

#endif // FILE_HANDLER_H