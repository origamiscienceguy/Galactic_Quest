#include "tonc.h"

//equal to 2/x^2, 8.24 fixed point
const u32 inverseTimeSquared[128] = {
	80000000, 0x20000000, 0xE38E38E, 0x8000000, 0x51EB852, 0x38E38E4, 0x29CBC15, 0x2000000, 
	0x1948B10, 0x147AE14, 0x10ECF57, 0xE38E39, 0xC1E4BC, 0xA72F05, 0x91A2B4, 0x800000, 
	0x716253, 0x6522C4, 0x5AC524, 0x51EB85, 0x4A4DC9, 0x43B3D6, 0x3DF17B, 0x38E38E, 0x346DC6, 
	0x30792F, 0x2CF302, 0x29CBC1, 0x26F690, 0x2468AD, 0x22190A, 0x200000, 0x1E170A, 0x1C5895, 
	0x1ABFD8, 0x1948B1, 0x17EF8B, 0x16B149, 0x158B31, 0x147AE1, 0x137E40, 0x129372, 0x11B8D6, 
	0x10ECF5, 0x102E86, 0xF7C5F, 0xED578, 0xE38E4, 0xDA5CC, 0xD1B71, 0xC9926, 0xC1E4C, 0xBAA55, 
	0xB3CC0, 0xAD518, 0xA72F0, 0xA15E8, 0x9BDA4, 0x969D4, 0x91A2B, 0x8CE65, 0x88643, 0x84188, 
	0x80000, 0x7C178, 0x785C2, 0x74CB4, 0x71625, 0x6E1F1, 0x6AFF6, 0x68014, 0x6522C, 0x62625, 
	0x5FBE3, 0x5D34F, 0x5AC52, 0x586D8, 0x562CC, 0x5401D, 0x51EB8, 0x4FE8E, 0x4DF90, 0x4C1AE, 
	0x4A4DD, 0x4890E, 0x46E35, 0x45449, 0x43B3D, 0x42309, 0x40BA1, 0x3F4FF, 0x3DF18, 0x3C9E5, 
	0x3B55E, 0x3A17C, 0x38E39, 0x37B8D, 0x36973, 0x357E5, 0x346DC, 0x33655, 0x32649, 0x316B5, 
	0x30793, 0x2F8DF, 0x2EA95, 0x2DCB2, 0x2CF30, 0x2C20D, 0x2B546, 0x2A8D7, 0x29CBC, 0x290F3, 
	0x2857A, 0x27A4D, 0x26F69, 0x264CD, 0x25A75, 0x25060, 0x2468B, 0x23CF4, 0x23399, 0x22A79, 
	0x22191, 0x218DF, 0x21062, 0x20818, 0x20000
};

const u16 inverseTime[256] = {
	0x8000, 0x4000, 0x2AAB, 0x2000, 0x199A, 0x1555, 0x1249, 0x1000, 0xE39, 0xCCD, 0xBA3, 0xAAB,
	0x9D9, 0x925, 0x889, 0x800, 0x788, 0x71C, 0x6BD, 0x666, 0x618, 0x5D1, 0x591, 0x555, 0x51F,
	0x4EC, 0x4BE, 0x492, 0x46A, 0x444, 0x421, 0x400, 0x3E1, 0x3C4, 0x3A8, 0x38E, 0x376, 0x35E,
	0x348, 0x333, 0x31F, 0x30C, 0x2FA, 0x2E9, 0x2D8, 0x2C8, 0x2B9, 0x2AB, 0x29D, 0x28F, 0x283,
	0x276, 0x26A, 0x25F, 0x254, 0x249, 0x23F, 0x235, 0x22B, 0x222, 0x219, 0x211, 0x208, 0x200,
	0x1F8, 0x1F0, 0x1E9, 0x1E2, 0x1DB, 0x1D4, 0x1CE, 0x1C7, 0x1C1, 0x1BB, 0x1B5, 0x1AF, 0x1AA,
	0x1A4, 0x19F, 0x19A, 0x195, 0x190, 0x18B, 0x186, 0x182, 0x17D, 0x179, 0x174, 0x170, 0x16C,
	0x168, 0x164, 0x160, 0x15D, 0x159, 0x155, 0x152, 0x14E, 0x14B, 0x148, 0x144, 0x141, 0x13E,
	0x13B, 0x138, 0x135, 0x132, 0x12F, 0x12D, 0x12A, 0x127, 0x125, 0x122, 0x11F, 0x11D, 0x11A,
	0x118, 0x116, 0x113, 0x111, 0x10F, 0x10D, 0x10A, 0x108, 0x106, 0x104, 0x102, 0x100, 0xFE,
	0xFC, 0xFA, 0xF8, 0xF6, 0xF5, 0xF3, 0xF1, 0xEF, 0xED, 0xEC, 0xEA, 0xE8, 0xE7, 0xE5, 0xE4,
	0xE2, 0xE0, 0xDF, 0xDD, 0xDC, 0xDA, 0xD9, 0xD8, 0xD6, 0xD5, 0xD3, 0xD2, 0xD1, 0xCF, 0xCE,
	0xCD, 0xCC, 0xCA, 0xC9, 0xC8, 0xC7, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1, 0xC0, 0xBF, 0xBD, 0xBC,
	0xBB, 0xBA, 0xB9, 0xB8, 0xB7, 0xB6, 0xB5, 0xB4, 0xB3, 0xB2, 0xB1, 0xB0, 0xAF, 0xAE, 0xAD,
	0xAC, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8, 0xA7, 0xA6, 0xA5, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA1,
	0xA0, 0x9F, 0x9E, 0x9E, 0x9D, 0x9C, 0x9B, 0x9B, 0x9A, 0x99, 0x98, 0x98, 0x97, 0x96, 0x96,
	0x95, 0x94, 0x94, 0x93, 0x92, 0x92, 0x91, 0x90, 0x90, 0x8F, 0x8E, 0x8E, 0x8D, 0x8D, 0x8C,
	0x8B, 0x8B, 0x8A, 0x8A, 0x89, 0x89, 0x88, 0x87, 0x87, 0x86, 0x86, 0x85, 0x85, 0x84, 0x84,
	0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80
};

const s16 sinTable[256] = {
	0, 0x3, 0x6, 0x9, 0xD, 0x10, 0x13, 0x16, 0x19, 0x1C, 0x1F, 0x22, 0x25, 0x28, 0x2B, 0x2E, 0x31,
	0x34, 0x37, 0x3A, 0x3C, 0x3F, 0x42, 0x44, 0x47, 0x4A, 0x4C, 0x4F, 0x51, 0x54, 0x56, 0x58, 0x5B,
	0x5D, 0x5F, 0x61, 0x63, 0x65, 0x67, 0x69, 0x6A, 0x6C, 0x6E, 0x6F, 0x71, 0x72, 0x74, 0x75, 0x76,
	0x77, 0x79, 0x7A, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7E, 0x7F, 0x7F, 0x7F, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x7F, 0x7F, 0x7F, 0x7E, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x7A, 0x79, 0x77, 0x76,
	0x75, 0x74, 0x72, 0x71, 0x6F, 0x6E, 0x6C, 0x6A, 0x69, 0x67, 0x65, 0x63, 0x61, 0x5F, 0x5D, 0x5B,
	0x58, 0x56, 0x54, 0x51, 0x4F, 0x4C, 0x4A, 0x47, 0x44, 0x42, 0x3F, 0x3C, 0x3A, 0x37, 0x34, 0x31,
	0x2E, 0x2B, 0x28, 0x25, 0x22, 0x1F, 0x1C, 0x19, 0x16, 0x13, 0x10, 0xD, 0x9, 0x6, 0x3, 0x0, 0xFFFD,
	0xFFFA, 0xFFF7, 0xFFF3, 0xFFF0, 0xFFED, 0xFFEA, 0xFFE7, 0xFFE4, 0xFFE1, 0xFFDE, 0xFFDB, 0xFFD8, 0xFFD5, 0xFFD2, 0xFFCF, 0xFFCC,
	0xFFC9, 0xFFC6, 0xFFC4, 0xFFC1, 0xFFBE, 0xFFBC, 0xFFB9, 0xFFB6, 0xFFB4, 0xFFB1, 0xFFAF, 0xFFAC, 0xFFAA, 0xFFA8, 0xFFA5, 0xFFA3,
	0xFFA1, 0xFF9F, 0xFF9D, 0xFF9B, 0xFF99, 0xFF97, 0xFF96, 0xFF94, 0xFF92, 0xFF91, 0xFF8F, 0xFF8E, 0xFF8C, 0xFF8B, 0xFF8A, 0xFF89,
	0xFF87, 0xFF86, 0xFF86, 0xFF85, 0xFF84, 0xFF83, 0xFF82, 0xFF82, 0xFF81, 0xFF81, 0xFF81, 0xFF80, 0xFF80, 0xFF80, 0xFF80, 0xFF80,
	0xFF80, 0xFF80, 0xFF81, 0xFF81, 0xFF81, 0xFF82, 0xFF82, 0xFF83, 0xFF84, 0xFF85, 0xFF86, 0xFF86, 0xFF87, 0xFF89, 0xFF8A, 0xFF8B,
	0xFF8C, 0xFF8E, 0xFF8F, 0xFF91, 0xFF92, 0xFF94, 0xFF96, 0xFF97, 0xFF99, 0xFF9B, 0xFF9D, 0xFF9F, 0xFFA1, 0xFFA3, 0xFFA5, 0xFFA8,
	0xFFAA, 0xFFAC, 0xFFAF, 0xFFB1, 0xFFB4, 0xFFB6, 0xFFB9, 0xFFBC, 0xFFBE, 0xFFC1, 0xFFC4, 0xFFC6, 0xFFC9, 0xFFCC, 0xFFCF, 0xFFD2,
	0xFFD5, 0xFFD8, 0xFFDB, 0xFFDE, 0xFFE1, 0xFFE4, 0xFFE7, 0xFFEA, 0xFFED, 0xFFF0, 0xFFF3, 0xFFF7, 0xFFFA, 0xFFFD
};

const u8 arctanTable1[256] = {
	0x0, 0x1, 0x2, 0x3, 0x5, 0x6, 0x7, 0x8, 0xA, 0xB, 0xC, 0xD, 0xF, 0x10, 0x11, 0x13, 0x14, 0x15, 
	0x16, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26, 0x27, 0x29, 
	0x2A, 0x2B, 0x2C, 0x2D, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x37, 0x38, 0x39, 0x3A, 0x3B, 
	0x3C, 0x3D, 0x3E, 0x3F, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 
	0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5B, 
	0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x68, 0x69, 
	0x6A, 0x6B, 0x6C, 0x6C, 0x6D, 0x6E, 0x6F, 0x6F, 0x70, 0x71, 0x72, 0x72, 0x73, 0x74, 0x75, 0x75, 
	0x76, 0x77, 0x77, 0x78, 0x79, 0x7A, 0x7A, 0x7B, 0x7C, 0x7C, 0x7D, 0x7E, 0x7E, 0x7F, 0x80, 0x80, 
	0x81, 0x81, 0x82, 0x83, 0x83, 0x84, 0x84, 0x85, 0x86, 0x86, 0x87, 0x87, 0x88, 0x89, 0x89, 0x8A, 
	0x8A, 0x8B, 0x8B, 0x8C, 0x8C, 0x8D, 0x8D, 0x8E, 0x8E, 0x8F, 0x90, 0x90, 0x91, 0x91, 0x92, 0x92, 
	0x93, 0x93, 0x93, 0x94, 0x94, 0x95, 0x95, 0x96, 0x96, 0x97, 0x97, 0x98, 0x98, 0x99, 0x99, 0x99, 
	0x9A, 0x9A, 0x9B, 0x9B, 0x9C, 0x9C, 0x9C, 0x9D, 0x9D, 0x9E, 0x9E, 0x9E, 0x9F, 0x9F, 0xA0, 0xA0, 
	0xA0, 0xA1, 0xA1, 0xA2, 0xA2, 0xA2, 0xA3, 0xA3, 0xA3, 0xA4, 0xA4, 0xA5, 0xA5, 0xA5, 0xA6, 0xA6, 
	0xA6, 0xA7, 0xA7, 0xA7, 0xA8, 0xA8, 0xA8, 0xA9, 0xA9, 0xA9, 0xAA, 0xAA, 0xAA, 0xAB, 0xAB, 0xAB, 
	0xAC, 0xAC, 0xAC, 0xAC, 0xAD, 0xAD, 0xAD, 0xAE, 0xAE, 0xAE, 0xAF, 0xAF, 0xAF, 0xAF, 0xB0, 0xB0, 
	0xB0, 0xB0, 0xB1, 0xB1, 0xB1, 0xB2, 0xB2, 0xB2, 0xB2, 0xB3, 0xB3, 0xB3, 0xB3, 0xB4
};

const u8 arctanTable2[256] = {
	0x0, 0xA, 0x14, 0x1E, 0x27, 0x31, 0x3A, 0x43, 0x4B, 0x53, 0x5B, 0x62, 0x68, 0x6F, 0x75, 0x7A, 
	0x80, 0x84, 0x89, 0x8D, 0x92, 0x95, 0x99, 0x9C, 0xA0, 0xA3, 0xA6, 0xA8, 0xAB, 0xAD, 0xB0, 0xB2, 
	0xB4, 0xB6, 0xB8, 0xBA, 0xBB, 0xBD, 0xBF, 0xC0, 0xC1, 0xC3, 0xC4, 0xC5, 0xC7, 0xC8, 0xC9, 0xCA, 
	0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD1, 0xD2, 0xD3, 0xD4, 0xD4, 0xD5, 0xD6, 0xD6, 0xD7, 
	0xD8, 0xD8, 0xD9, 0xD9, 0xDA, 0xDA, 0xDB, 0xDB, 0xDC, 0xDC, 0xDD, 0xDD, 0xDE, 0xDE, 0xDF, 0xDF, 
	0xDF, 0xE0, 0xE0, 0xE0, 0xE1, 0xE1, 0xE2, 0xE2, 0xE2, 0xE3, 0xE3, 0xE3, 0xE3, 0xE4, 0xE4, 0xE4, 
	0xE5, 0xE5, 0xE5, 0xE5, 0xE6, 0xE6, 0xE6, 0xE6, 0xE7, 0xE7, 0xE7, 0xE7, 0xE8, 0xE8, 0xE8, 0xE8, 
	0xE8, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEB, 0xEB, 0xEB, 0xEB, 
	0xEB, 0xEB, 0xEC, 0xEC, 0xEC, 0xEC, 0xEC, 0xEC, 0xEC, 0xED, 0xED, 0xED, 0xED, 0xED, 0xED, 0xED, 
	0xED, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0xEF, 0xEF, 0xEF, 0xEF, 0xEF, 0xEF, 
	0xEF, 0xEF, 0xEF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF1, 0xF1, 
	0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 
	0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 
	0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 
	0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF5, 0xF5, 0xF5, 
	0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
};

const u8 minimapPositions[] = {
	31, 49, 61, 67
};

//the y position of the select-a-ship menu based on how many entries there are
const u8 selectAShipYPos[] = {
	0, 0, 40, 32, 24, 16, 8, 0, 0, 0
};

//the x position of the top-left corner of the select-a-ship menu based on what frame of emergence it is at
const u8 selectAShipXPos[] = {
	28, 52, 70, 82, 88
};

//the x position of the action menu
const u8 actionMenuXPos[] = {
	28, 46, 58, 64
};

//the y position of the action menu
const u8 actionMenuYPos[] = {
	0, 72, 64, 56, 48
};

const u8 leftBattleAngle[] = {
	0, 1, 2, 3, 3, 4, 4, 5, 5, 4, 4, 3, 3, 2, 1, 0, 255, 254, 253, 253, 252, 252, 251, 251, 252, 252, 253, 253, 254, 255,
};

const u8 leftBattleYPos[] = {
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0,
};
