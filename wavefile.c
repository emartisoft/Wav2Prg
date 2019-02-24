/*
 * WAV to PRG converter for C64
 * (C)2019 emarti, Murat Ozdemir
*/

#include "wavefile.h"

unsigned int waveFileGetnFrames(FILE *wFile)
{
    fseek( wFile, 0x28, SEEK_SET );
    fread(buffer_data_size, sizeof(buffer_data_size), 1, wFile);
    wh.data_size = buffer_data_size[0] |
				(buffer_data_size[1] << 8) |
				(buffer_data_size[2] << 16) |
				(buffer_data_size[3] << 24 );

    return wh.data_size/2;
}

unsigned char findNibble(short s)
{
    return (unsigned char) ((s + 0x8000) * 0x10 / 0x10000);
}

unsigned char findByte(short s)
{
    return (unsigned char) ((s + 0x8000) * 0x100 / 0x10000);
}
