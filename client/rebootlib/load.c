/**********************************
  Copyright (C) Rick Wong (Lick)
  http://lickr.org
  http://licklick.wordpress.com
  
  Copyright (C) Michael Chisholm
  http://chishm.drunkencoders.com
  
  See license in readme.txt
***********************************/

//====================================
#ifdef ARM9
//====================================
#include <nds.h>
#include <string.h>
#include <sys/stat.h>
#include "loadnds_bin.h"


#define LCDC_BANK_C                 ((u16*)0x06840000)
#define STORED_FILE_CLUSTER_OFFSET  (4)
#define INIT_DISC_OFFSET            (8)
#define WANT_TO_PATCH_DLDI_OFFSET   (12)
#define FIX_ALL                     (0x01)
#define FIX_GLUE                    (0x02)
#define FIX_GOT                     (0x04)
#define FIX_BSS	                    (0x08)

typedef signed int addr_t;
typedef unsigned char data_t;

enum DldiOffsets {
	DO_magicString = 0x00,
	DO_magicToken = 0x00,
	DO_magicShortString = 0x04,
	DO_version = 0x0C,
	DO_driverSize = 0x0D,
	DO_fixSections = 0x0E,
	DO_allocatedSpace = 0x0F,
	DO_friendlyName = 0x10,
	DO_text_start = 0x40,
	DO_data_end = 0x44,
	DO_glue_start = 0x48,
	DO_glue_end = 0x4C,
	DO_got_start = 0x50,
	DO_got_end = 0x54,
	DO_bss_start = 0x58,
	DO_bss_end = 0x5C,
	DO_ioType = 0x60,
	DO_features = 0x64,
	DO_startup = 0x68,
	DO_isInserted = 0x6C,
	DO_readSectors = 0x70,
	DO_writeSectors = 0x74,
	DO_clearStatus = 0x78,
	DO_shutdown = 0x7C,
	DO_code = 0x80
};

static const data_t dldiMagicString[] = "\xED\xA5\x8D\xBF Chishm";
static const data_t dldiMagicLoaderString[] = "\xEE\xA5\x8D\xBF Chishm";
#define DEVICE_TYPE_DLDI 0x49444C44
extern const u32* _io_dldi;


//=================================
int getSize(u8 *source, u16 *dest, u32 r2) {
//=================================
	return *(u32*)source;
}

//=================================
u8 readByte(u8 *source) {
//=================================
	return *source;
}

TDecompressionStream decompressCallback = {
  getSize, (void*)0, readByte
};


//=================================
static addr_t readAddr (data_t *mem, addr_t offset) {
//=================================
	return ((addr_t*)mem)[offset/sizeof(addr_t)];
}

//=================================
static void writeAddr (data_t *mem, addr_t offset, addr_t value) {
//=================================
	((addr_t*)mem)[offset/sizeof(addr_t)] = value;
}

//=================================
static addr_t quickFind (const data_t* data, const data_t* search, size_t dataLen, size_t searchLen) {
//=================================
	const int* dataChunk = (const int*) data;
	int searchChunk = ((const int*)search)[0];
	addr_t i;
	addr_t dataChunkEnd = (addr_t)(dataLen / sizeof(int));

	for ( i = 0; i < dataChunkEnd; i++) {
		if (dataChunk[i] == searchChunk) {
			if ((i*sizeof(int) + searchLen) > dataLen) {
				return -1;
			}
			if (memcmp (&data[i*sizeof(int)], search, searchLen) == 0) {
				return i*sizeof(int);
			}
		}
	}

	return -1;
}


//=================================
static bool dldiPatchLoader (data_t *binData, u32 binSize, bool clearBSS)
//=================================
{
	addr_t memOffset;			// Offset of DLDI after the file is loaded into memory
	addr_t patchOffset;			// Position of patch destination in the file
	addr_t relocationOffset;	// Value added to all offsets within the patch to fix it properly
	addr_t ddmemOffset;			// Original offset used in the DLDI file
	addr_t ddmemStart;			// Start of range that offsets can be in the DLDI file
	addr_t ddmemEnd;			// End of range that offsets can be in the DLDI file
	addr_t ddmemSize;			// Size of range that offsets can be in the DLDI file

	addr_t addrIter;

	data_t *pDH;
	data_t *pAH;

	size_t dldiFileSize = 0;

	// Find the DLDI reserved space in the file
	patchOffset = quickFind (binData, dldiMagicLoaderString, binSize, sizeof(dldiMagicLoaderString));

	if (patchOffset < 0) {
		// does not have a DLDI section
		return false;
	}

	pDH = (data_t*)(((u32*)(&_io_dldi)) -24);
	pAH = &(binData[patchOffset]);

	if (*((u32*)(pDH + DO_ioType)) == DEVICE_TYPE_DLDI) {
		// No DLDI patch
		return false;
	}

	if (pDH[DO_driverSize] > pAH[DO_allocatedSpace]) {
		// Not enough space for patch
		return false;
	}

	dldiFileSize = 1 << pDH[DO_driverSize];

	memOffset = readAddr (pAH, DO_text_start);
	if (memOffset == 0) {
			memOffset = readAddr (pAH, DO_startup) - DO_code;
	}
	ddmemOffset = readAddr (pDH, DO_text_start);
	relocationOffset = memOffset - ddmemOffset;

	ddmemStart = readAddr (pDH, DO_text_start);
	ddmemSize = (1 << pDH[DO_driverSize]);
	ddmemEnd = ddmemStart + ddmemSize;

	// Remember how much space is actually reserved
	pDH[DO_allocatedSpace] = pAH[DO_allocatedSpace];
	// Copy the DLDI patch into the application
	memcpy (pAH, pDH, dldiFileSize);

	// Fix the section pointers in the header
	writeAddr (pAH, DO_text_start, readAddr (pAH, DO_text_start) + relocationOffset);
	writeAddr (pAH, DO_data_end, readAddr (pAH, DO_data_end) + relocationOffset);
	writeAddr (pAH, DO_glue_start, readAddr (pAH, DO_glue_start) + relocationOffset);
	writeAddr (pAH, DO_glue_end, readAddr (pAH, DO_glue_end) + relocationOffset);
	writeAddr (pAH, DO_got_start, readAddr (pAH, DO_got_start) + relocationOffset);
	writeAddr (pAH, DO_got_end, readAddr (pAH, DO_got_end) + relocationOffset);
	writeAddr (pAH, DO_bss_start, readAddr (pAH, DO_bss_start) + relocationOffset);
	writeAddr (pAH, DO_bss_end, readAddr (pAH, DO_bss_end) + relocationOffset);
	// Fix the function pointers in the header
	writeAddr (pAH, DO_startup, readAddr (pAH, DO_startup) + relocationOffset);
	writeAddr (pAH, DO_isInserted, readAddr (pAH, DO_isInserted) + relocationOffset);
	writeAddr (pAH, DO_readSectors, readAddr (pAH, DO_readSectors) + relocationOffset);
	writeAddr (pAH, DO_writeSectors, readAddr (pAH, DO_writeSectors) + relocationOffset);
	writeAddr (pAH, DO_clearStatus, readAddr (pAH, DO_clearStatus) + relocationOffset);
	writeAddr (pAH, DO_shutdown, readAddr (pAH, DO_shutdown) + relocationOffset);

	if (pDH[DO_fixSections] & FIX_ALL) {
		// Search through and fix pointers within the data section of the file
		for (addrIter = (readAddr(pDH, DO_text_start) - ddmemStart); addrIter < (readAddr(pDH, DO_data_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GLUE) {
		// Search through and fix pointers within the glue section of the file
		for (addrIter = (readAddr(pDH, DO_glue_start) - ddmemStart); addrIter < (readAddr(pDH, DO_glue_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GOT) {
		// Search through and fix pointers within the Global Offset Table section of the file
		for (addrIter = (readAddr(pDH, DO_got_start) - ddmemStart); addrIter < (readAddr(pDH, DO_got_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (clearBSS && (pDH[DO_fixSections] & FIX_BSS)) {
		// Initialise the BSS to 0, only if the disc is being re-inited
		memset (&pAH[readAddr(pDH, DO_bss_start) - ddmemStart] , 0, readAddr(pDH, DO_bss_end) - readAddr(pDH, DO_bss_start));
	}

	return true;
}


//=================================
void prepare_load_bin(const char *filename) {
//=================================
	struct stat st;
	stat(filename, &st);

	VRAM_C_CR = VRAM_ENABLE | VRAM_C_LCD;
	
	memset(LCDC_BANK_C, 0, 128*1024);
	memcpy(LCDC_BANK_C, loadnds_bin, loadnds_bin_size);

	if (!dldiPatchLoader((data_t*)LCDC_BANK_C, loadnds_bin_size, false)) {
		return; 
	}

	writeAddr((data_t*)LCDC_BANK_C, STORED_FILE_CLUSTER_OFFSET, st.st_ino);
	writeAddr((data_t*)LCDC_BANK_C, INIT_DISC_OFFSET, false);
	writeAddr((data_t*)LCDC_BANK_C, WANT_TO_PATCH_DLDI_OFFSET, true);

	VRAM_C_CR = VRAM_ENABLE | VRAM_C_ARM7_0x06000000;
}

#endif
