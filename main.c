/*
 * WAV to PRG converter for C64
 * (C)2019 emarti, Murat Ozdemir
 *
 * Reference: Python Script, http://www.frank-buss.de/c64/sampler/index.html
*/

#include "main.h"
#include "wavefile.h"

int main(int argc, char **argv)
{
    if(argc!=4)
    {
        printHelp();
        return 0;
    }

    char* cmd = argv[1];
    bool h1 = strequal(cmd,"-h1");
    bool h2 = strequal(cmd,"-h2");

    // argv[2] must be wave filename
    if(!IsThisWaveFile(argv[2]) || !(h1 || h2))
    {
        printHelp();
        return 0;
    }

    // length
    unsigned int length = waveFileGetnFrames(waveFile);
    //printf("length: %u \n", length);

    int datasize = 0;
    if (h1) datasize = (length/2 + 0x0870);
    if (h2) datasize = length + 0x200 + 0x801;

    //printf("%u\n", datasize);

    if( datasize >= 0xd000)
    {
        printf("Error: The WAV file is too big size! :(\n");
        fclose(waveFile);
        return 0;
    }
    else
    {
        prgFile = fopen(argv[3], "wb");

        if(h1)
        {
            if(prgFile){
                for(int i=0;i<sizeof(prgheader4bit);i++)
                {
                   fputc(prgheader4bit[i], prgFile);
                }
            }
            // add sample size
            fputc((unsigned char) ((length/2)&0xff), prgFile);
            fputc((unsigned char) (length/2/0x100), prgFile);

            // add samples
            fseek(waveFile, 0x2c, SEEK_SET);
            unsigned char data[4];
            for(int i=0;i<length/2;i++)
            {
                fread(data, sizeof(data), 1, waveFile);
                fputc(findNibble(data[0]|(data[1]<<8)) | ((findNibble(data[2]|(data[3]<<8)))<<4) , prgFile);
            }
        }

        if(h2)
        {
            if(prgFile){
                for(int i=0;i<sizeof(prgheader8bit);i++)
                {
                   fputc(prgheader8bit[i], prgFile);
                }
            }
            // add sample size
            unsigned char block = length / 0x100;
            if ((block * 0x100)<length) block++;
            fputc((unsigned char) block, prgFile);

            // add samples
            fseek(waveFile, 0x2c, SEEK_SET);
            unsigned char data[2];
            for(int i=0;i<length;i++)
            {
                fread(data, sizeof(data), 1, waveFile);
                fputc(findByte(data[0]|(data[1]<<8)) , prgFile);
            }

            // fill last block
            for(int i=0;i<(length-block*0x100);i++)
            {
                fputc((unsigned char)0x80, prgFile);
            }

        }
    }
    fclose(prgFile);
    fclose(waveFile);

    printf("%s PRG file was created successfully.\n", argv[3]);

    return 1;
}

bool IsThisWaveFile(char* filename)
{
    waveFile = fopen(filename, "rb");
    if(waveFile==NULL)
    {
        //fprintf(stderr, "Cannot open input file %s\n", filename);
        return false;
    }

    if(fread(&wh, sizeof(wh), 1, waveFile)<1)
    {
        fprintf(stderr, "Cannot read file header.\n");
        return false;
    }

    if(wh.riff[0] != 'R' ||
       wh.riff[1] != 'I' ||
       wh.riff[2] != 'F' ||
       wh.riff[3] != 'F')
    {
        fprintf(stderr, "File is not WAV format.\n");
        return false;
    }

    return true;
}

bool strequal(char *str1, char *str2)
{
    while( ( *str1 != '\0' && *str2 != '\0' ) && *str1 == *str2 )
    {
        str1++;
        str2++;
    }

    if(*str1 == *str2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void printHelp()
{
    // help
    printf("\n                      WAV to PRG converter for C64\n");
    printf("                           Copyright (C) 2019\n");
    printf("          Wav2Prg %s   emarti, Murat Ozdemir   %s\n\n", VERSION, DATE);
    printf("Usage: Wav2Prg -hx wavefile prgfile\n\n");
    printf("Options:\n");
    printf("  -hx           -h1 for 4 bit PRG header\n");
    printf("                -h2 for 8 bit PRG header\n");
    printf("  wavefile      Mono wave file properties:\n");
    printf("                For 4 bit, 8000Hz sample rate, max wave file length is about 12 seconds\n");
    printf("                For 8 bit, 11025Hz sample rate, max wave file length is about 4.5 seconds\n");
    printf("  prgfile       C64 PRG filename\n\n");
    printf("Examples:\n");
    printf("  Wav2Prg -h1 mysound8kHz.wav playonc64.prg\n");
    printf("  Wav2Prg -h2 mysound11kHz.wav playonc64.prg\n\n");
    printf("\nWav2Prg comes with absolutely no warranty for details visit https://github.com/emartisoft\n\n");
}
