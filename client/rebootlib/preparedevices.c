/**********************************
  Copyright (C) Rick Wong (Lick)
  http://lickr.org
  http://licklick.wordpress.com

  See license in readme.txt
***********************************/
#include <nds.h>
#include <stdio.h>
#include <fat.h>


//====================================
#ifdef ARM9
//====================================

//=================================
void reboot_prepare_default() {
//=================================
    *(vu32*)0x27FFFF8 = 0x80000C0;
}


//=================================
void reboot_prepare_ez() {
//=================================
    *(vu16*)0x9FE0000 = 0xD200;
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9880000 = (1 << 15);
    *(vu16*)0x9FC0000 = 0x1500;

    *(vu32*)0x27FFFF8 = 0x80000C0;
}


//=================================
void reboot_prepare_m3() {
//=================================
    u32 mode = 0x00400004;
    vu16 tmp;
    tmp = *(vu16*)0x08E00002;
    tmp = *(vu16*)0x0800000E;
    tmp = *(vu16*)0x08801FFC;
    tmp = *(vu16*)0x0800104A;
    tmp = *(vu16*)0x08800612;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x08801B66;
    tmp = *(vu16*)(0x08000000 + (mode << 1));
    tmp = *(vu16*)0x0800080E;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x08000188;
    tmp = *(vu16*)0x08000188;
    
    *(vu32*)0x27FFFF8 = 0x80000C0;
}


//=================================
void reboot_prepare_sc() {
//=================================
    *(vu16*)0x09FFFFFE = 0xA55A;
    *(vu16*)0x09FFFFFE = 0xA55A;
    *(vu16*)0x09FFFFFE = 0;
    *(vu16*)0x09FFFFFE = 0;

    *(vu32*)0x27FFFF8 = 0x8000000;
}


void prepare_load_bin(const char *filename);

//=================================
void reboot_prepare_scds() {
//=================================
    prepare_load_bin("/msforsc.nds");

    *(vu32*)0x27FFFF8 = 0x6000000;
}


//=================================
void reboot_prepare_g6() {
//=================================
    u32 mode = 0;
    vu16 tmp;
    tmp = *(vu16*)0x09000000;
    tmp = *(vu16*)0x09FFFFE0;
    tmp = *(vu16*)0x09FFFFEC;
    tmp = *(vu16*)0x09FFFFEC;
    tmp = *(vu16*)0x09FFFFEC;
    tmp = *(vu16*)0x09FFFFFC;
    tmp = *(vu16*)0x09FFFFFC;
    tmp = *(vu16*)0x09FFFFFC;
    tmp = *(vu16*)0x09FFFF4A;
    tmp = *(vu16*)0x09FFFF4A;
    tmp = *(vu16*)0x09FFFF4A;
    tmp = *(vu16*)(0x09200000 + (mode << 1));
    tmp = *(vu16*)0x09FFFFF0;
    tmp = *(vu16*)0x09FFFFE8;
    
    *(vu32*)0x27FFFF8 = 0x80000C0;
}


//=================================
void reboot_prepare_dlms() {
//=================================
    asm(".ARM");
    asm("MOV R10, #0");
    asm("MCR p15, 0, R10,c7,c5,0");
    asm("MCR p15, 0, R10,c7,c6,0");
    asm("MCR p15, 0, R10,c7,c10,4");
    asm("MCR p15, 0, R10,c3,c0,0");
    asm("MCR p15, 0, R10,c1,c0");

    *((vu8 *)0x40001A1) = 0xC0;
    *((vu32*)0x40001A8) = 0xEDEDDEDE;
    *((vu32*)0x40001AC) = 2 << 24;
    CARD_CR2 = 0xA0000000;
    while (CARD_CR2 & CARD_BUSY);

    *((vu32*)0x40001A8) = 0x100200A4;
    *((vu8 *)0x40001AC) = 0;
    CARD_CR2 = 0xAA000000;

    u32 data;
    u32 *dest = (u32*)0x2100000;
    u32 *target = dest + 1024;
    do {
        if (CARD_CR2 & CARD_DATA_READY) {
            data = CARD_DATA_RD;
            if(dest < target) {
                *dest = data;
            }
            dest++;
        }
    } while (CARD_CR2 & CARD_BUSY);
    
    *(vu32*)0x27FFFF8 = 0x2100000;
}


#define BIT_AT(n, i) ((n >> i) & 1)
//=================================
void reboot_prepare_r4() {
//=================================
    FILE *in = fopen("/_DS_MENU.DAT", "rb");
    if (in == NULL) {
        return;
    }

    FILE *out = fopen("/_DS_MENU.NDS", "wb");
    if (out == NULL) {
        fclose(in);
        return;
    }

    unsigned char buf[512];
    int r, n = 0;
    int i, j;
    while ((r = fread(buf, 1, 512, in)) > 0)
    {
        unsigned short key = n ^ 0x484A;
        for (i = 0; i < 512; i ++)
        {
            unsigned char xor = 0;
            if (key & 0x4000) xor |= 0x80;
            if (key & 0x1000) xor |= 0x40;
            if (key & 0x0800) xor |= 0x20;
            if (key & 0x0200) xor |= 0x10;
            if (key & 0x0080) xor |= 0x08;
            if (key & 0x0040) xor |= 0x04;
            if (key & 0x0002) xor |= 0x02;
            if (key & 0x0001) xor |= 0x01;

            unsigned int k = ((buf[i] << 8) ^ key) << 16;
            unsigned int x = k;
            for (j = 1; j < 32; j ++)
                x ^= k >> j;
            key = 0x0000;
            if (BIT_AT(x, 23)) key |= 0x8000;
            if (BIT_AT(k, 22)) key |= 0x4000;
            if (BIT_AT(k, 21)) key |= 0x2000;
            if (BIT_AT(k, 20)) key |= 0x1000;
            if (BIT_AT(k, 19)) key |= 0x0800;
            if (BIT_AT(k, 18)) key |= 0x0400;
            if (BIT_AT(k, 17) != BIT_AT(x, 31)) key |= 0x0200;
            if (BIT_AT(k, 16) != BIT_AT(x, 30)) key |= 0x0100;
            if (BIT_AT(k, 30) != BIT_AT(k, 29)) key |= 0x0080;
            if (BIT_AT(k, 29) != BIT_AT(k, 28)) key |= 0x0040;
            if (BIT_AT(k, 28) != BIT_AT(k, 27)) key |= 0x0020;
            if (BIT_AT(k, 27) != BIT_AT(k, 26)) key |= 0x0010;
            if (BIT_AT(k, 26) != BIT_AT(k, 25)) key |= 0x0008;
            if (BIT_AT(k, 25) != BIT_AT(k, 24)) key |= 0x0004;
            if (BIT_AT(k, 25) != BIT_AT(x, 26)) key |= 0x0002;
            if (BIT_AT(k, 24) != BIT_AT(x, 25)) key |= 0x0001;

            buf[i] ^= xor;
        }
        fwrite(buf, 1, r, out);
        n ++;
    }
    fclose(out);
    fclose(in);

    prepare_load_bin("/_DS_MENU.NDS");
    remove("/_DS_MENU.NDS");

    *(vu32*)0x27FFFF8 = 0x6000000;
}


#endif
