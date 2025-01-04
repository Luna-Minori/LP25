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



int main()
{
    send_network("test.txt");
    menu();
    
    return 0;
}
