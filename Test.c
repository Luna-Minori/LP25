#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

int verifier_fichier_ou_dossier(char *path) {
    
    if (path == NULL) {
        return -1; // Chemin vide
    }

    int longueur = strlen(path);

    for (int i = longueur - 1; i >= 0; --i) {
        if (path[i] == '\\') { 
            return 1; // dossier
        }
        if (path[i] == '.') {
            return 0; // Fichier 
        }
    }

    return 1; // la racine
}

void parcourir_dossier(char *path) {
    struct dirent *ent;
    DIR *dir = opendir(dossier);

    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) { // ignore le repertoire courant et parent
            continue;
        }

        char chemin_complet[1024];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", dossier, ent->d_name);

        struct stat st;
        if (stat(chemin_complet, &st) == 0) {
            if (S_ISDIR(st.st_mode)) { // cas d'un sous dossier appelle récursif 
                parcourir_dossier(chemin_complet);
            } 
            else {
                compute_chunk(chemin_complet); // fichier
            }
        }
    }
    closedir(dir);
}

int main(){
    char *test = "C:\\Users\\Luna\\Documents\\GitHub\\LP25";
    printf("%d", verifier_fichier_ou_dossier(test));
    return 0;
}