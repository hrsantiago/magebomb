#ifndef __CONSTTIBIA_H__
#define __CONSTTIBIA_H__

// 0 = Version 800
// 1 = Version 810
// 2 = Version 811
// 3 = Version 820
// 4 = Version 821
// 5 = Version 822

#define RSAKEY_CIP "124710459426827943004376449897985582167801707960697037164044904862948569380850421396904597686953877022394604239428185498284169068581802277612081027966724336319448537811441719076484340922854929273517308661370727105382899118999403808045846444647284499123164879035103627004668521005328367415259939915284902061793"
#define RSAKEY_OTS "109120132967399429278860960508995541528237502902798129123468757937266291492576446330739696001110603907230888610072655818825358503429057592827629436413108566029093628212635953836686562675849720620786279431090218017681061521755056710823876476444260558147179707119674283982419152118103759076030616683978566631413"

// Tibia signature (Used for Global Login)
#define DAT_SIGNATURE 0x489980A1
#define SPR_SIGNATURE 0x489980A5
#define PIC_SIGNATURE 0x48562106

const unsigned short currentTibiaVersion = 822;

// Tibia Addresses
const unsigned long battleBeginAddress[] = {0x60EB30, 0x613BD0, 0x613BD0, 0x621CD0, 0x624CD0, 0x626CD0};
const unsigned long battleEndAddress[]   = {0x6148F0, 0x619990, 0x619990, 0x627A90, 0x630A90, 0x632A90};
const unsigned long targetIdAddress[]    = {0x60EA9C, 0x613B3C, 0x613B3C, 0x621C3C, 0x624C3C, 0x626C3C};
const unsigned long playerIdAddress[]    = {0x60EAD0, 0x613B70, 0x613B70, 0x621C70, 0x624C70, 0x626C70};

// Runes
const unsigned short sd  = 3155;
const unsigned short uh  = 3160;
const unsigned short hmm = 3198;
const unsigned short lmm = 3174;
const unsigned short exP = 3200;
const unsigned short gfb = 3191;

// Potions
const unsigned short shp = 236;
const unsigned short smp = 237;
const unsigned short gmp = 238;
const unsigned short ghp = 239;
const unsigned short hp  = 266;
const unsigned short mp  = 268;

enum SpeakClasses {
	SPEAK_SAY           = 0x01,
	SPEAK_WHISPER       = 0x02,
	SPEAK_YELL          = 0x03,
	SPEAK_BROADCAST     = 0x09,
	SPEAK_PRIVATE       = 0x04,
	SPEAK_PRIVATE_RED   = 0x0B,
	SPEAK_CHANNEL_Y     = 0x05,
	SPEAK_CHANNEL_R1    = 0x0A,
	SPEAK_CHANNEL_R2    = 0x0E,
	SPEAK_CHANNEL_O     = 0x0C,
	SPEAK_MONSTER_SAY   = 0x10,
	SPEAK_MONSTER_YELL  = 0x11,
};

enum MessageClasses {
	MSG_STATUS_CONSOLE_YELLOW	= 0x01, //Yellow message in the console
	MSG_STATUS_CONSOLE_LBLUE	= 0x04, //Light blue message in the console
	MSG_STATUS_CONSOLE_ORANGE	= 0x11, //Orange message in the console
	MSG_STATUS_WARNING			= 0x12, //Red message in game window and in the console
	MSG_EVENT_ADVANCE			= 0x13, //White message in game window and in the console
	MSG_EVENT_DEFAULT			= 0x14, //White message at the bottom of the game window and in the console
	MSG_STATUS_DEFAULT			= 0x15, //White message at the bottom of the game window and in the console
	MSG_INFO_DESCR				= 0x16, //Green message in game window and in the console
	MSG_STATUS_SMALL			= 0x17, //White message at the bottom of the game window"
	MSG_STATUS_CONSOLE_BLUE		= 0x18, //Blue message in the console
	MSG_STATUS_CONSOLE_RED		= 0x19, //Red message in the console
};

enum Direction {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	SOUTHWEST = 4,
	SOUTHEAST = 5,
	NORTHWEST = 6,
	NORTHEAST = 7,
};

#endif
