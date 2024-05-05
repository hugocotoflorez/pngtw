#include "png.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define HASH_STR(CHK_STR) (((CHK_STR[0] << 8) + CHK_STR[1]) % 84)
#define HASH_UINT(CHK_UINT) (((CHK_UINT) >> 16) % 84)


// if error: print error, set errno to 0 and return true
bool check_failture() // i think misspeak
{
    if(errno)
    {
        perror("Error");
        errno = 0;
        return true;
    }
    return false;
}

static void* chunk_lookup[] = {
    [60] = (void*)read_IDHR,
    [14] = (void*)read_IEND,
    [65] = (void*)read_IDAT,
};

uint32_t swap_endian(uint32_t num)
{
    return ((num >> 24) & 0xff) | // move byte 3 to byte 0
    ((num << 8) & 0xff0000) |     // move byte 1 to byte 2
    ((num >> 8) & 0xff00) |       // move byte 2 to byte 1
    ((num << 24) & 0xff000000);   // byte 0 to byte 3
}


bool png_open(const char* filename, int* fd)
{
    errno = 0;
    *fd   = open(filename, O_RDONLY);
    return check_failture();
}


int get_size(int fd)
{
    uint32_t size;
    read(fd, &size, 4);
    size = swap_endian(size);
    if(PNG_VERBOSE > 1)
        printf("Next chunck size: %u bytes\n", size);
    return size;
}


bool read_header(int fd, struct file_header* header)
{
    read(fd, header, 8);
    if(PNG_VERBOSE)
    {
        puts("\e[1m[File Header]\e[0m");
        printf("| Transmission System: %.2x\n", header->transmission_system);
        printf("| Identifier: %.6x\n", header->identifier);
        printf("| Line ending: %.4x\n", header->line_ending);
        printf("| Eof character: %.2x\n", header->eof_character);
        printf("| Eol character: %.2x\n", header->eol_character);
    }
    return check_failture();
}


bool read_IDAT(int fd, struct CHUNK* chunk)
{
    chunk->chunk_type = IDAT;
    struct IDAT* idat= &chunk->chunk_data.idat;
    uint8_t buff[chunk->length];
    read(fd, &buff, chunk->length);
    if(PNG_VERBOSE)
    {
        puts("\e[1m[IDAT Chunk]\e[0m");
        printf("| Size: %u bytes\n", chunk->length);
    }
    return check_failture();
}


bool read_IEND(int fd, struct CHUNK* chunk)
{
    chunk->chunk_type = IEND;
    struct IEND* iend= &chunk->chunk_data.iend;
    if(PNG_VERBOSE)
    {
        puts("\e[1m[IEND Chunk]\e[0m");
    }
    return check_failture();
}

bool read_IDHR(int fd, struct CHUNK* chunk)
{
    struct IHDR* ihdr = &chunk->chunk_data.ihdr;
    read(fd, ihdr, chunk->length);
    if(PNG_VERBOSE)
    {
        puts("\e[1m[IHDR Chunk]\e[0m");
        printf("| Width: %u \n", swap_endian(ihdr->width));
        printf("| Height: %u \n", swap_endian(ihdr->height));
        printf("| Bit depth: %.1x\n", ihdr->bit_depth);
        printf("| Color type: %.1x\n", ihdr->color_type);
        printf("| Compression method: %.1x\n", ihdr->compression_method);
        printf("| Filter method: %.1x\n", ihdr->filter_method);
        printf("| Interlace method: %.1x\n", ihdr->interlace_method);
    }
    return check_failture();
}


bool read_chunk(int fd)
{
    struct CHUNK chunk;
    uint32_t name;
    uint32_t crc;
    uint16_t key;
    bool (*f)(int, struct CHUNK*);
    chunk.length = get_size(fd);
    read(fd, &name, 4);
    key = HASH_UINT(name);
    if(PNG_VERBOSE > 1)
        printf("Next -> %.4s (KEY:%u)\n", (char*)&name, key);
    if((f = chunk_lookup[key]))
    {
        f(fd, &chunk);
    }
    else
    {
        lseek(fd, chunk.length, SEEK_CUR);
        if(PNG_VERBOSE > 1)
            puts("Chunk type not found");
        // return 0;
    }
    // CRC
    read(fd, &crc, 4);
    if(PNG_VERBOSE > 1)
        printf("CRC-32: %#x\n", swap_endian(crc));
    return (chunk.length > 0);
    /*
     * add chunk to chunck array ?
     */
}


int main()
{
    int fd;
    struct file_header header;
    struct IHDR ihdr;
    if(png_open("file.png", &fd))
        goto exit;
    if(read_header(fd, &header))
        goto exit;
    while(read_chunk(fd))
        ; // call funcion until it return 0: end of file

exit:
    close(fd);
    return 0;
}
