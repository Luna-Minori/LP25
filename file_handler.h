// file_handler.h
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "file_modifier.h"

void compute_md5_file(const char *filename, unsigned char *md5, unsigned int *md5_len);
int compute_chunk(char *nom_fichier,char *path , Chunk *chunks);
void compute_md5_chunk(char *data, size_t taille, char *md5_string);
void compute_md5_case(const char *dir_path, unsigned char *md5_out);

#endif // FILE_HANDLER_H