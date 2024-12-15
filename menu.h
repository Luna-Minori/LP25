#ifndef MENU_H
#define MENU_H



// Function prototypes
int verifier_fichier_ou_dossier(char *path);
void creer_dossier(const char *chemin_dossier);
void parcourir_dossier(const char *dossier, const char *dossier_save);
void vider_buffer();
void Entry_sauvegarde(char *path_save);
int Entry_recuperation(char *path_save, char *path_restore);
void menu();
int get_version();

#endif // MENU_H