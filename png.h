#ifndef _PNG_WRAPPER
#define _PNG_WRAPPER

#include <stdbool.h>
#include <stdint.h>

/*
 * 0: silent mode
 * 1: show known chunks data
 * 2: debug mode
 */
#define PNG_VERBOSE 1

/*
 * Portable Network Graphics Tiny Wrapper
 * Author: Hugo Coto Florez
 * Src: github.com/hugocotoflorez/pngtw
 */

struct file_header
{
    uint8_t transmission_system : 8;
    uint32_t identifier : 24;
    uint16_t line_ending : 16;
    uint8_t eof_character : 8;
    uint8_t eol_character : 8;
};

// ---- chunks ----

enum chunk_t
{
    IHDR = (1 << 0),
    PLTE = (1 << 1),
    IDAT = (1 << 2),
    IEND = (1 << 3),
};
// IHDR must be the fist chunk
struct IHDR
{
    uint32_t width : 32;
    uint32_t height : 32;
    uint8_t bit_depth : 8;
    uint8_t color_type : 8;
    uint8_t compression_method : 8;
    uint8_t filter_method : 8;
    uint8_t interlace_method : 8;
};

// PLTE
struct PLTE
{
    uint32_t data : 32;
};

// IDAT
struct IDAT
{
    uint32_t data : 32;
};

// IEND
struct IEND
{
    uint32_t data : 32;
};

struct CHUNK
{
    uint32_t length;
    enum chunk_t chunk_type;
    union
    {
        struct IHDR ihdr;
        struct PLTE plte;
        struct IDAT idat;
        struct IEND iend;
    } chunk_data;
    uint32_t CRC;
};


// FUNCTIONS
bool read_IDHR(int fd, struct CHUNK* chunk);
bool read_IEND(int fd, struct CHUNK* chunk);
bool read_IDAT(int fd, struct CHUNK* chunk);


#endif // _PNG_WRAPPER
