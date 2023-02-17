
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "mmu.h"
#include "rom.h"

const char *romTypeString[256] = {
	[ROM_PLAIN] = "ROM_PLAIN",
	[ROM_MBC1] = "ROM_MBC1",
	[ROM_MBC1_RAM] = "ROM_MBC1",
	[ROM_MBC1_RAM_BATT] = "ROM_MBC1_RAM_BATT",
	[ROM_MBC2] = "ROM_MBC2",
	[ROM_MBC2_BATTERY] = "ROM_MBC2_BATTERY",
	[ROM_RAM] = "ROM_RAM",
	[ROM_RAM_BATTERY] = "ROM_RAM_BATTERY",
	[ROM_MMM01] = "ROM_MMM01",
	[ROM_MMM01_SRAM] = "ROM_MMM01_SRAM",
	[ROM_MMM01_SRAM_BATT] = "ROM_MMM01_SRAM_BATT",
	[ROM_MBC3_TIMER_BATT] = "ROM_MBC3_TIMER_BATT",
	[ROM_MBC3_TIMER_RAM_BATT] = "ROM_MBC3_TIMER_RAM_BATT",
	[ROM_MBC3] = "ROM_MBC3",
	[ROM_MBC3_RAM] = "ROM_MBC3_RAM",
	[ROM_MBC3_RAM_BATT] = "ROM_MBC3_RAM_BATT",
	[ROM_MBC5] = "ROM_MBC5",
	[ROM_MBC5_RAM] = "ROM_MBC5_RAM",
	[ROM_MBC5_RAM_BATT] = "ROM_MBC5_RAM_BATT",
	[ROM_MBC5_RUMBLE] = "ROM_MBC5_RUMBLE",
	[ROM_MBC5_RUMBLE_SRAM] = "ROM_MBC5_RUMBLE_SRAM",
	[ROM_MBC5_RUMBLE_SRAM_BATT] = "ROM_MBC5_RUMBLE_SRAM_BATT",
	[ROM_POCKET_CAMERA] = "ROM_POCKET_CAMERA",
	[ROM_BANDAI_TAMA5] = "ROM_BANDAI_TAMA5",
	[ROM_HUDSON_HUC3] = "ROM_HUDSON_HUC3",
	[ROM_HUDSON_HUC1] = "ROM_HUDSON_HUC1",
};

const char *romOldLicenseeCode[256] = {
	[0x00] = "None",
	[0x01] = "Nintendo",
	[0x08] = "Capcom",
	[0x09] = "Hot-B",
	[0x0A] = "Jaleco",
	[0x0B] = "Coconuts Japan",
	[0x0C] = "Elite Systems",
	[0x13] = "EA (Electronic Arts)",
	[0x18] = "Hudsonsoft",
	[0x19] = "ITC Entertainment",
	[0x1A] = "Yanoman",
	[0x1D] = "Japan Clary",
	[0x1F] = "Virgin Interactive",
	[0x24] = "PCM Complete",
	[0x25] = "San-X",
	[0x28] = "Kotobuki Systems",
	[0x29] = "Seta",
	[0x30] = "Infogrames",
	[0x31] = "Nintendo",
	[0x32] = "Bandai",
	[0x33] = "Indicates that the New licensee code should be used instead.",
	[0x34] = "Konami",
	[0x35] = "HectorSoft",
	[0x38] = "Capcom",
	[0x39] = "Banpresto",
	[0x3C] = ".Entertainment i",
	[0x3E] = "Gremlin",
	[0x41] = "Ubisoft",
	[0x42] = "Atlus",
	[0x44] = "Malibu",
	[0x46] = "Angel",
	[0x47] = "Spectrum Holoby",
	[0x49] = "Irem",
	[0x4A] = "Virgin Interactive",
	[0x4D] = "Malibu",
	[0x4F] = "U.S. Gold",
	[0x50] = "Absolute",
	[0x51] = "Acclaim",
	[0x52] = "Activision",
	[0x53] = "American Sammy",
	[0x54] = "GameTek",
	[0x55] = "Park Place",
	[0x56] = "LJN",
	[0x57] = "Matchbox",
	[0x59] = "Milton Bradley",
	[0x5A] = "Mindscape",
	[0x5B] = "Romstar",
	[0x5C] = "Naxat Soft",
	[0x5D] = "Tradewest",
	[0x60] = "Titus",
	[0x61] = "Virgin Interactive",
	[0x67] = "Ocean Interactive",
	[0x69] = "EA (Electronic Arts)",
	[0x6E] = "Elite Systems",
	[0x6F] = "Electro Brain",
	[0x70] = "Infogrames",
	[0x71] = "Interplay",
	[0x72] = "Broderbund",
	[0x73] = "Sculptered Soft",
	[0x75] = "The Sales Curve",
	[0x78] = "t.hq",
	[0x79] = "Accolade",
	[0x7A] = "Triffix Entertainment",
	[0x7C] = "Microprose",
	[0x7F] = "Kemco",
	[0x80] = "Misawa Entertainment",
	[0x83] = "Lozc",
	[0x86] = "Tokuma Shoten Intermedia",
	[0x8B] = "Bullet-Proof Software",
	[0x8C] = "Vic Tokai",
	[0x8E] = "Ape",
	[0x8F] = "I’Max",
	[0x91] = "Chunsoft Co.",
	[0x92] = "Video System",
	[0x93] = "Tsubaraya Productions Co.",
	[0x95] = "Varie Corporation",
	[0x96] = "Yonezawa/S’Pal",
	[0x97] = "Kaneko",
	[0x99] = "Arc",
	[0x9A] = "Nihon Bussan",
	[0x9B] = "Tecmo",
	[0x9C] = "Imagineer",
	[0x9D] = "Banpresto",
	[0x9F] = "Nova",
	[0xA1] = "Hori Electric",
	[0xA2] = "Bandai",
	[0xA4] = "Konami",
	[0xA6] = "Kawada",
	[0xA7] = "Takara",
	[0xA9] = "Technos Japan",
	[0xAA] = "Broderbund",
	[0xAC] = "Toei Animation",
	[0xAD] = "Toho",
	[0xAF] = "Namco",
	[0xB0] = "acclaim",
	[0xB1] = "ASCII or Nexsoft",
	[0xB2] = "Bandai",
	[0xB4] = "Square Enix",
	[0xB6] = "HAL Laboratory",
	[0xB7] = "SNK",
	[0xB9] = "Pony Canyon",
	[0xBA] = "Culture Brain",
	[0xBB] = "Sunsoft",
	[0xBD] = "Sony Imagesoft",
	[0xBF] = "Sammy",
	[0xC0] = "Taito",
	[0xC2] = "Kemco",
	[0xC3] = "Squaresoft",
	[0xC4] = "Tokuma Shoten Intermedia",
	[0xC5] = "Data East",
	[0xC6] = "Tonkinhouse",
	[0xC8] = "Koei",
	[0xC9] = "UFL",
	[0xCA] = "Ultra",
	[0xCB] = "Vap",
	[0xCC] = "Use Corporation",
	[0xCD] = "Meldac",
	[0xCE] = "Pony Canyon",
	[0xCF] = "Angel",
	[0xD0] = "Taito",
	[0xD1] = "Sofel",
	[0xD2] = "Quest",
	[0xD3] = "Sigma Enterprises",
	[0xD4] = "ASK Kodansha Co.",
	[0xD6] = "Naxat Soft",
	[0xD7] = "Copya System",
	[0xD9] = "Banpresto",
	[0xDA] = "Tomy",
	[0xDB] = "LJN",
	[0xDD] = "NCS",
	[0xDE] = "Human",
	[0xDF] = "Altron",
	[0xE0] = "Jaleco",
	[0xE1] = "Towa Chiki",
	[0xE2] = "Yutaka",
	[0xE3] = "Varie",
	[0xE5] = "Epcoh",
	[0xE7] = "Athena",
	[0xE8] = "Asmik ACE Entertainment",
	[0xE9] = "Natsume",
	[0xEA] = "King Records",
	[0xEB] = "Atlus",
	[0xEC] = "Epic/Sony Records",
	[0xEE] = "IGS",
	[0xF0] = "A Wave",
	[0xF3] = "Extreme Entertainment",
	[0xFF] = "LJN",
};

const char *romNewLicenseeCode[256] = {
	[0x00] = "None",
	[0x01] = "Nintendo",
	[0x08] = "Capcom",
	[0x13] = "Electronic Arts",
	[0x18] = "Hudson Soft",
	[0x19] = "b-ai",
	[0x20] = "kss",
	[0x22] = "pow",
	[0x24] = "PCM Complete",
	[0x25] = "san-x",
	[0x28] = "Kemco Japan",
	[0x29] = "seta",
	[0x30] = "Viacom",
	[0x31] = "Nintendo",
	[0x32] = "Bandai",
	[0x33] = "Ocean/Acclaim",
	[0x34] = "Konami",
	[0x35] = "Hector",
	[0x37] = "Taito",
	[0x38] = "Hudson",
	[0x39] = "Banpresto",
	[0x41] = "Ubi Soft",
	[0x42] = "Atlus",
	[0x44] = "Malibu",
	[0x46] = "angel",
	[0x47] = "Bullet-Proof",
	[0x49] = "irem",
	[0x50] = "Absolute",
	[0x51] = "Acclaim",
	[0x52] = "Activision",
	[0x53] = "American Sammy",
	[0x54] = "Konami",
	[0x55] = "Hi Tech Entertainment",
	[0x56] = "LJN",
	[0x57] = "Matchbox",
	[0x58] = "Mattel",
	[0x59] = "Milton Bradley",
	[0x60] = "Titus",
	[0x61] = "Virgin",
	[0x64] = "LucasArts",
	[0x67] = "Ocean",
	[0x69] = "Electronic Arts",
	[0x70] = "Infogrames",
	[0x71] = "Interplay",
	[0x72] = "Broderbund",
	[0x73] = "sculptured",
	[0x75] = "sci",
	[0x78] = "THQ",
	[0x79] = "Accolade",
	[0x80] = "misawa",
	[0x83] = "lozc",
	[0x86] = "Tokuma Shoten Intermedia",
	[0x87] = "Tsukuda Original",
	[0x91] = "Chunsoft",
	[0x92] = "Video system",
	[0x93] = "Ocean/Acclaim",
	[0x95] = "Varie",
	[0x96] = "Yonezawa/s’pal",
	[0x97] = "Kaneko",
	[0x99] = "Pack in soft",
	[0xA4] = "Konami",
};

const char *romCGBCompatability[256] = {
	[0x80] = "Backward Compatible",
	[0xC0] = "CGB Only",
};

unsigned char ASCIILookup[256] = {
	[0x30] = (unsigned char)0x0,
	[0x31] = (unsigned char)0x1,
	[0x32] = (unsigned char)0x2,
	[0x33] = (unsigned char)0x3,
	[0x34] = (unsigned char)0x4,
	[0x35] = (unsigned char)0x5,
	[0x36] = (unsigned char)0x6,
	[0x37] = (unsigned char)0x7,
	[0x38] = (unsigned char)0x8,
	[0x39] = (unsigned char)0x9,
	[0x41] = (unsigned char)0xA,
	[0x42] = (unsigned char)0xB,
	[0x43] = (unsigned char)0xC,
	[0x44] = (unsigned char)0xD,
	[0x45] = (unsigned char)0xE,
	[0x46] = (unsigned char)0xF,
};

unsigned char rom_load(char *filename) {
	char name[17];
	enum romType type;
	int romSize;
	int ramSize;
	
	int i;
	
	FILE *f;
	size_t length;
	
	unsigned char header[0x180];
	
	f = fopen(filename, "rb");
	if(!f) return 0;
	
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	if(length < 0x180) {
		printf("ROM is too small!\n");
		fclose(f);
		return 0;
	}
	
	rewind(f);
	fread(header, 0x180, 1, f);
	
	memset(name, '\0', 17);
	for(i = 0; i < 16; i++) {
		if(header[i + ROM_OFFSET_NAME] == 0x80 || header[i + ROM_OFFSET_NAME] == 0xc0) name[i] = '\0';
		else name[i] = header[i + ROM_OFFSET_NAME];
	}
	
	printf("Title: %s\n", name);
	printf("old licensee code: 0x%02x", header[ROM_OFFSET_OLD_LICENSEE_CODE]);
	if(header[ROM_OFFSET_OLD_LICENSEE_CODE] == 0x33) {
		// use new licensee code
		unsigned char code1 = header[ROM_OFFSET_NEW_LICENSEE_CODE_1];
		unsigned char code2 = header[ROM_OFFSET_NEW_LICENSEE_CODE_2];
		unsigned char pub_code = (ASCIILookup[code1] << 4) | (ASCIILookup[code2]);
		printf("Publisher: %s\n", romNewLicenseeCode[pub_code]);
	}
	else {
		printf("Publisher: %s\n", romOldLicenseeCode[header[ROM_OFFSET_OLD_LICENSEE_CODE]]);
	}
	
	type = header[ROM_OFFSET_TYPE];
	
	if(!romTypeString[type]) {
		printf("Unknown ROM type: %#02x\n", type);
		fclose(f);
		return 0;
	}
	
	printf("ROM Type: %s\n", romTypeString[type]);
	printf("CGB Compatability: %s\n", romCGBCompatability[header[ROM_OFFSET_CGB_FLAG]]);
	
	romSize = header[ROM_OFFSET_ROM_SIZE];
	
	if((romSize & 0xF0) == 0x50) romSize = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
	else romSize = (int)pow(2.0, (double)(romSize + 1));
	
	printf("ROM Size: %dKB\n", romSize * 16);
	
	if(length != romSize * 16 * 1024) {
		printf("ROM filesize does not equal ROM size!\n");
	}
	
	ramSize = header[ROM_OFFSET_RAM_SIZE];
    ramSize = (int)pow(4.0, (double)(ramSize)) / 2;

	printf("RAM Size: %dKB\n", ramSize);
	
	ramSize = ceil(ramSize / 8.0f);	
	rewind(f);
	fread(mmu.cartridge, length, 1, f);	
	fclose(f);
	
	return 1;
}
