#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Erreur: Impossible d'ouvrir le fichier");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1L)
    {
        perror("Erreur: Taille du fichier inconnue");
        fclose(file);
        return NULL;
    }
    rewind(file);

    char *content = (char *)malloc(file_size + 1);
    if (content == NULL)
    {
        printf("Erreur: Allocation mémoire échouée.\n");
        fclose(file);
        return NULL;
    }

    size_t total_read = 0;
    size_t chunk_size = 1024;
    while (total_read < file_size)
    {
        size_t bytes_to_read = (file_size - total_read < chunk_size) ? file_size - total_read : chunk_size;
        size_t read_size = fread(content + total_read, 1, bytes_to_read, file);
        if (read_size != bytes_to_read)
        {
            perror("Erreur lors de la lecture du fichier");
            free(content);
            fclose(file);
            return NULL;
        }
        total_read += read_size;
    }

    content[file_size] = '\0';

    if (ferror(file))
    {
        perror("Erreur lors de la lecture du fichier");
        free(content);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return content;
}

void read_file_in_chunks(const char *filename)
{
    char *file_content = read_file(filename);
    if (file_content == NULL)
    {
        return;
    }

    size_t file_size = strlen(file_content);

    size_t total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (size_t i = 0; i < total_chunks; ++i)
    {
        size_t chunk_start = i * CHUNK_SIZE;
        size_t chunk_end = (i + 1) * CHUNK_SIZE;
        if (chunk_end > file_size)
        {
            chunk_end = file_size;
        }

        printf("Chunk %zu (%zu octets) :\n", i + 1, chunk_end - chunk_start);
        fwrite(file_content + chunk_start, 1, chunk_end - chunk_start, stdout);
        printf("\n---\n");
    }
    free(file_content);
}

int main()
{
    const char *filename = "test.txt";
    read_file_in_chunks(filename);
    return 0;
}
