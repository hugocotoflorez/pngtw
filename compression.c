
#include "png.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

void get_IDAT_header(uint8_t* data, size_t chunk_length)
{
    uint8_t CM    = data[0] & 0xF;
    uint8_t CINFO = (data[0] >> 4) & 0xF;
    // bits 0 to 4  FCHECK  (check bits for CMF and FLG)
    // bit  5       FDICT   (preset dictionary)
    // bits 6 to 7  FLEVEL  (compression level)
    uint8_t FCHECK           = data[1] & 0x1F;
    uint8_t FDICT            = (data[1] >> 5) & 1;
    uint8_t FLEVEL           = (data[1] >> 6) & 0x3;
    uint16_t FLEVEL_CHECK    = (data[0] * 256 + data[1])%31;
    printf("| [CM] Compression method: %u\n", CM);
    printf("| [CINFO] Compression info: %.4b\n", CINFO);
    printf("| [FDICT] Preset dictionary: %.1b\n", FDICT);
    printf("| [FLEVEL] Compression level: %u\n", FLEVEL);
    printf("| [FCHECK] Must be zero: %u\n", FLEVEL_CHECK);
    // ALDER32
    // no se si siempre acaba al acabar el ALDER igual esto va mal
    uint32_t ALDER32 = data[chunk_length - 5];
    printf("| [ALDER32] : %#.8x\n", ALDER32);
}
