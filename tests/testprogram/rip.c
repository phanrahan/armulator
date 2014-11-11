
//Copyright (C) 2002, David Welch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;

FILE *sram;

unsigned int ra;
unsigned int rb;
unsigned int address;
unsigned int progoff;
unsigned int memsize;

struct
{
    unsigned char e_ident[16];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned int e_shoff;
    unsigned int e_flags;
    unsigned short e_ehsize;
    unsigned short e_phentsize;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
    unsigned short e_shstrndx;
} elfheader;

typedef struct
{
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_paddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
} PROGHEADER;
PROGHEADER progheader;
//unsigned int progheadersize;
unsigned short prog;

unsigned int *progdata;

unsigned char filename[250];


int main ( int argc, char *argv[] )
{
    if(argc<2)
    {
        printf(".ELF file not specified\n");
        return(1);
    }

    fp=fopen(argv[1],"rb");
    if(fp==NULL)
    {
        printf("Error opening file %s\n",argv[1]);
        return(1);
    }

    if(fread(&elfheader,1,sizeof(elfheader),fp)!=sizeof(elfheader))
    {
        printf("Error reading ELF header\n");
        fclose(fp);
        return(1);
    }
    if(   *(unsigned int *)elfheader.e_ident !=0x464C457F)
    {
        printf("Not an ELF format file\n");
        fclose(fp);
        return(1);
    }


    if(elfheader.e_ehsize!=sizeof(elfheader))
    {
        printf("e_ehsize is not %u\n",(unsigned int)sizeof(elfheader));
        fclose(fp);
        return(1);
    }

    if(elfheader.e_phentsize!=sizeof(PROGHEADER))
    {
        printf("e_phentsize is not %u\n",(unsigned int)sizeof(PROGHEADER));
        fclose(fp);
        return(1);
    }


    if(elfheader.e_phoff==0) return(1);

    progoff=elfheader.e_phoff;
    for(prog=0;prog<elfheader.e_phnum;prog++)
    {

        fseek(fp,progoff,0);
        fread(&progheader,1,sizeof(progheader),fp);
        progoff=ftell(fp);


        if(progheader.p_type==1)
        {
            printf("%08X %08X %08X ",progheader.p_vaddr,progheader.p_filesz,progheader.p_memsz);
            printf("(%u) (%u) (%u)\n",progheader.p_filesz,progheader.p_memsz,progheader.p_memsz-progheader.p_filesz);

            fseek(fp,progheader.p_offset,0);

            if(progheader.p_filesz>progheader.p_memsz)
            {
                printf("size problem\n");
                return(1);
            }

            if(progheader.p_memsz&3)
            {
                printf("data not word aligned\n");
                //return(1);
            }


            memsize=progheader.p_memsz;

            progdata=malloc(memsize);
            if(progdata==NULL)
            {
                printf("Error allocating memory\n");
                return(1);
            }
            else
            {
                memset(progdata,0,memsize);

                sprintf(filename,"%08X.bin",progheader.p_vaddr);
                sram=fopen(filename,"wb");
                if(sram==NULL)
                {
                    printf("Error creating file %s\n",filename);
                }
                else
                {
                    fread(progdata,1,progheader.p_filesz,fp);
                    fwrite(progdata,1,progheader.p_memsz,sram);
                    free(progdata);
                    fclose(sram);
                }
            }
        }
    }
    fclose(fp);
    return(0);
}
