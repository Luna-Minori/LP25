#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <libgen.h>
#include "file_modifier.h"
#include "file_handler.h"
#include "backup_manager.h"
#include "menu.h"
#include "network_transmission.h"
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char path_save[40] = "/home/Max/LP25/testmax/CACA.txt";
    Chunk chunks[100];
    printf("Erreur de création du fichier la \n");
    int Nchunk = compute_chunk("CACA.txt", path_save, chunks);
    printf("Erreur de création du fichier la \n");
    sauvegarder(chunks, Nchunk, "CACA.txt", path_save);
    printf("Nchunk = %d\n", Nchunk);
    return 0;
}
