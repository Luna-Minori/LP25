#ifndef NETWORK_TRANSMISSION_H
#define NETWORK_TRANSMISSION_H

#include <stddef.h>

int send_network(char *filepath);
void recup_network(char *nom_fichier, int version);

#endif // NETWORK_TRANSMISSION_H