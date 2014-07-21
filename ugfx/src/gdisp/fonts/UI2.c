/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/*
 * This font is copyright Andrew Hannam and has been donated to uGFX as a default font.
 * It is licensed as per the rest of the uGFX library.
 * Other fonts are licensed as per their own licenses.
 *
 * UI1 contains a full character set from 0 to 255
 * UI2 contains just the printable characters (space to tilda) and is therefore much smaller
 *
 * UI1 and UI2 support the normal size, Narrow (double height), Double (double sized) versions.
 *
 */

#ifndef MF_RLEFONT_INTERNALS
#define MF_RLEFONT_INTERNALS
#endif
#include "mf_rlefont.h"

#ifndef MF_RLEFONT_VERSION_4_SUPPORTED
#error The font file is not compatible with this version of mcufont.
#endif

static const uint8_t mf_rlefont_UI2_dictionary_data[159] = {
    0x2c, 0x80, 0x06, 0x80, 0x03, 0x80, 0x06, 0x80, 0x03, 0x80, 0x01, 0x01, 0x80, 0x06, 0x80, 0x80, 
    0x0a, 0x80, 0x06, 0x21, 0x82, 0x01, 0x80, 0x07, 0x03, 0x80, 0x01, 0x80, 0x05, 0x02, 0x80, 0x07, 
    0x80, 0x0a, 0x80, 0x0a, 0x0b, 0x80, 0x01, 0x80, 0x05, 0x80, 0x02, 0x04, 0x80, 0x02, 0x09, 0x01, 
    0x80, 0x81, 0x01, 0x81, 0x04, 0x03, 0x80, 0x02, 0x08, 0x80, 0x80, 0x02, 0x0a, 0x80, 0x0a, 0x80, 
    0x80, 0x03, 0x06, 0x80, 0x07, 0xe7, 0x26, 0x86, 0x86, 0x28, 0xe0, 0xb4, 0x28, 0x2c, 0x86, 0x85, 
    0xa5, 0x1d, 0x33, 0x2e, 0x30, 0x23, 0x1c, 0x2b, 0x33, 0x2d, 0x2e, 0x2c, 0x85, 0xa5, 0xa4, 0x94, 
    0x94, 0x94, 0x88, 0x74, 0xfc, 0x74, 0x28, 0xdc, 0x33, 0x21, 0x33, 0xfd, 0x2e, 0x31, 0x74, 0x1e, 
    0xcc, 0x2e, 0xcb, 0x26, 0x1a, 0xcc, 0x26, 0x20, 0x2e, 0xf6, 0x2e, 0x28, 0x1b, 0x27, 0x30, 0x30, 
    0x23, 0x26, 0x1a, 0x2a, 0x32, 0x31, 0x19, 0x95, 0x26, 0x27, 0x2e, 0x32, 0xa8, 0x22, 0x28, 0x88, 
    0xc6, 0xa9, 0x25, 0x2e, 0x1e, 0xd6, 0x29, 0x1e, 0x25, 0x28, 0x1b, 0x1f, 0x22, 0x1b, 0x74, 
};

static const uint16_t mf_rlefont_UI2_dictionary_offsets[62] = {
    0x0000, 0x0001, 0x000b, 0x000f, 0x0013, 0x0014, 0x0015, 0x0018, 
    0x001c, 0x001d, 0x001f, 0x0021, 0x0024, 0x0025, 0x0028, 0x002b, 
    0x002e, 0x002f, 0x0031, 0x0032, 0x0034, 0x0035, 0x0038, 0x003a, 
    0x003c, 0x0040, 0x0042, 0x0043, 0x0045, 0x0049, 0x004b, 0x004d, 
    0x0051, 0x0053, 0x0056, 0x0059, 0x005b, 0x0064, 0x0066, 0x0069, 
    0x006b, 0x006d, 0x006f, 0x0072, 0x0077, 0x0079, 0x007c, 0x007e, 
    0x0081, 0x0083, 0x0085, 0x0087, 0x0089, 0x008b, 0x008e, 0x0090, 
    0x0092, 0x0095, 0x0096, 0x0098, 0x009d, 0x009f, 
};

static const uint8_t mf_rlefont_UI2_glyph_data_0[822] = {
    0x03, 0x00, 0x10, 0x03, 0x46, 0x39, 0x4e, 0x10, 0x05, 0x50, 0xee, 0x10, 0x08, 0xfc, 0x24, 0x50, 
    0xe3, 0x22, 0x86, 0x1f, 0x22, 0x38, 0x50, 0x10, 0x07, 0xfc, 0x1e, 0x92, 0x48, 0x28, 0xd0, 0x23, 
    0x86, 0x88, 0x41, 0x2e, 0x10, 0x0b, 0x28, 0xdc, 0x94, 0xa8, 0x88, 0x96, 0x85, 0x9a, 0x28, 0x91, 
    0x94, 0x8d, 0xa8, 0x88, 0x65, 0x10, 0x09, 0xf0, 0x4d, 0x21, 0x32, 0x3f, 0x8a, 0xa5, 0xa8, 0x27, 
    0x8a, 0x88, 0x49, 0xab, 0x10, 0x03, 0x46, 0x10, 0x04, 0x42, 0x47, 0xfd, 0x30, 0x10, 0x04, 0x1b, 
    0xa4, 0x47, 0x40, 0x23, 0x07, 0x24, 0x44, 0x2e, 0x8e, 0x64, 0x85, 0x58, 0x2e, 0x10, 0x07, 0xf4, 
    0x24, 0x23, 0x1b, 0x80, 0x2e, 0x30, 0x10, 0x04, 0x18, 0x18, 0x42, 0x10, 0x05, 0x18, 0x35, 0x10, 
    0x03, 0x18, 0x18, 0x0f, 0x10, 0x07, 0x4c, 0x20, 0x4c, 0x28, 0x46, 0x32, 0x42, 0x23, 0x07, 0x3e, 
    0xfd, 0x8a, 0x1f, 0x1a, 0x8e, 0x94, 0x89, 0x1f, 0x86, 0x90, 0x41, 0x10, 0x07, 0x00, 0x28, 0x3f, 
    0x8e, 0x39, 0x33, 0xc2, 0x10, 0x07, 0x3e, 0x31, 0x46, 0x32, 0x45, 0x33, 0x80, 0x10, 0x07, 0x3e, 
    0x31, 0x1b, 0xd2, 0x23, 0xc5, 0x88, 0x41, 0x10, 0x07, 0xfc, 0x24, 0x1e, 0x3f, 0x52, 0x8e, 0x26, 
    0x41, 0x49, 0x3b, 0x10, 0x07, 0x35, 0x49, 0x54, 0xfd, 0x23, 0xc5, 0x88, 0x41, 0x10, 0x07, 0x3e, 
    0x54, 0x33, 0x4a, 0x3d, 0x10, 0x07, 0x28, 0xc0, 0x1b, 0x3b, 0x28, 0x1b, 0x3b, 0x10, 0x07, 0x3e, 
    0x4b, 0x1e, 0x43, 0x00, 0x41, 0x10, 0x07, 0x3e, 0x4a, 0xf4, 0x7c, 0x23, 0x41, 0x10, 0x03, 0x1c, 
    0x2f, 0x18, 0x2e, 0x10, 0x04, 0x1c, 0x29, 0x28, 0x18, 0x42, 0x10, 0x07, 0xf4, 0x1c, 0x1e, 0x8a, 
    0x36, 0x3f, 0xd4, 0x10, 0x07, 0x1c, 0x1d, 0x87, 0x24, 0x24, 0x80, 0x10, 0x07, 0x1c, 0x33, 0x36, 
    0x90, 0x64, 0x2e, 0x10, 0x06, 0x8a, 0x26, 0x46, 0xfc, 0x36, 0x1b, 0xfc, 0x44, 0x10, 0x0a, 0x80, 
    0xa4, 0x26, 0x76, 0x4f, 0x4f, 0xf6, 0x58, 0x89, 0x6d, 0xa4, 0x28, 0x80, 0x10, 0x09, 0x2c, 0x30, 
    0x28, 0x50, 0xfd, 0x88, 0x7c, 0x85, 0x85, 0xa5, 0xa4, 0x10, 0x07, 0x35, 0x4d, 0x21, 0x22, 0x43, 
    0xc6, 0x7c, 0x10, 0x08, 0x00, 0x3e, 0x95, 0x4c, 0x30, 0x4e, 0x41, 0x10, 0x08, 0x24, 0x38, 0x95, 
    0x37, 0x37, 0x48, 0x1d, 0x10, 0x06, 0x24, 0x38, 0x54, 0x2e, 0x23, 0x33, 0x7c, 0x10, 0x06, 0x24, 
    0x38, 0x54, 0x39, 0x10, 0x08, 0x44, 0x49, 0xc6, 0x85, 0x46, 0x8b, 0x88, 0x86, 0x88, 0x41, 0x10, 
    0x08, 0x24, 0xa5, 0x37, 0x2c, 0x82, 0x37, 0x37, 0x10, 0x03, 0x00, 0x47, 0x46, 0x10, 0x05, 0x44, 
    0x47, 0x31, 0x64, 0x10, 0x07, 0x24, 0x4b, 0x33, 0xc9, 0x32, 0x49, 0x1f, 0x4d, 0x21, 0x32, 0xa6, 
    0x10, 0x06, 0x00, 0x30, 0x47, 0xf3, 0x10, 0x0b, 0x4e, 0x27, 0x87, 0xaa, 0x4f, 0x4f, 0xd8, 0xa8, 
    0x98, 0x96, 0x88, 0x8d, 0xa6, 0x10, 0x08, 0x4e, 0xc6, 0x90, 0x86, 0x8e, 0x85, 0xa9, 0x26, 0x89, 
    0xa8, 0x86, 0xa6, 0x85, 0xa5, 0x10, 0x09, 0x44, 0x34, 0x29, 0x3c, 0x10, 0x07, 0x24, 0x38, 0x4a, 
    0x20, 0x38, 0x46, 0x10, 0x0a, 0x44, 0x34, 0x29, 0x3c, 0x33, 0xb4, 0x10, 0x07, 0x24, 0x38, 0x4a, 
    0x20, 0x38, 0x8d, 0x48, 0xec, 0x10, 0x06, 0x28, 0xdc, 0x4d, 0x4e, 0x2e, 0x4e, 0xd4, 0x88, 0x64, 
    0x10, 0x07, 0x28, 0x80, 0x39, 0x46, 0x10, 0x08, 0x32, 0x37, 0x37, 0x37, 0x26, 0x88, 0x7c, 0x10, 
    0x09, 0x24, 0xc5, 0x86, 0x85, 0x85, 0x4b, 0x1e, 0x50, 0xc8, 0x2e, 0x10, 0x0d, 0x4e, 0xc6, 0x8a, 
    0x86, 0xa9, 0x96, 0x56, 0xa8, 0x4f, 0xc6, 0x89, 0x85, 0xa5, 0xa4, 0x10, 0x09, 0x4e, 0x21, 0x26, 
    0x1e, 0x53, 0xec, 0xa4, 0xa4, 0x10, 0x07, 0x24, 0x4b, 0x1e, 0x50, 0xc8, 0x2e, 0x30, 0x10, 0x08, 
    0x24, 0xc3, 0x28, 0x45, 0xc5, 0x26, 0x32, 0x45, 0x82, 0x10, 0x04, 0x49, 0x3b, 0x47, 0x1b, 0x36, 
    0x07, 0x33, 0xcc, 0x30, 0x24, 0x1b, 0x94, 0x30, 0x24, 0x1b, 0x04, 0x2a, 0x30, 0x47, 0x1b, 0xdc, 
    0x10, 0x07, 0x21, 0x28, 0x50, 0x4b, 0x29, 0x10, 0x08, 0x1c, 0x1c, 0x18, 0xc3, 0x10, 0x04, 0x31, 
    0x2e, 0x10, 0x07, 0x3a, 0x2c, 0x41, 0x19, 0xf4, 0x7c, 0x10, 0x07, 0x1b, 0x4c, 0x43, 0xf6, 0x48, 
    0x1d, 0x10, 0x06, 0x3a, 0x1b, 0x4c, 0x35, 0x10, 0x07, 0xd4, 0x23, 0x41, 0x19, 0x20, 0x4b, 0x86, 
    0x7c, 0x10, 0x06, 0x1c, 0x2b, 0x4d, 0x8b, 0x4c, 0x88, 0x64, 0x10, 0x06, 0x3f, 0x42, 0x1d, 0x39, 
    0x0f, 0x10, 0x07, 0x1c, 0x2b, 0x49, 0x4a, 0x3d, 0x46, 0x64, 0x33, 0x07, 0x46, 0x20, 0x1f, 0x85, 
    0x90, 0x19, 0x48, 0xec, 0x10, 0x03, 0x4e, 0x20, 0x44, 0x47, 0x10, 0x04, 0x28, 0x2d, 0x32, 0x44, 
    0x47, 0x40, 0x10, 0x06, 0x46, 0x4d, 0x86, 0x36, 0x50, 0xd6, 0x10, 0x03, 0x0f, 0x47, 0x46, 0x10, 
    0x0a, 0x1c, 0x25, 0x91, 0x90, 0x8d, 0xa6, 0x88, 0x95, 0xa6, 0x2d, 0x95, 0xa8, 0x10, 0x07, 0x1c, 
    0x25, 0x85, 0x90, 0x19, 0x48, 0xec, 0x10, 0x07, 0x3a, 0x4a, 0xf4, 0x88, 0x41, 0x10, 0x07, 0x1c, 
    0x38, 0x4a, 0x48, 0x8b, 0x4c, 0x10, 0x07, 0x1c, 0xf2, 0x19, 0x48, 0xf4, 0x41, 0x46, 0x10, 0x06, 
    0x1c, 0xfd, 0x87, 0x9c, 0x39, 0x10, 0x06, 0x3a, 0x0f, 0x32, 0x36, 0xd0, 0x28, 0x41, 0x10, 0x06, 
    0x24, 0x42, 0x1d, 0x39, 0x2b, 0x10, 0x07, 0x1c, 0x4a, 0x48, 0x3f, 0xef, 0x10, 0x07, 0x1c, 0x4b, 
    0x1e, 0x50, 0xc8, 0x2e, 0x10, 0x0b, 0x1c, 0x2f, 0xa6, 0x88, 0x95, 0x58, 0x86, 0x59, 0x48, 0xec, 
    0x10, 0x07, 0x1c, 0x2f, 0x1e, 0x53, 0xec, 0x10, 0x07, 0x1c, 0x4b, 0x1e, 0x50, 0xfc, 0x1b, 0x3b, 
    0x10, 0x05, 0x1c, 0x1d, 0x28, 0x45, 0x45, 0xf2, 0x10, 0x05, 0xfc, 0x40, 0x30, 0x28, 0x1e, 0x88, 
    0x39, 0x29, 0x10, 0x03, 0x1b, 0x4c, 0x30, 0x47, 0x05, 0x31, 0x39, 0x45, 0x29, 0x28, 0x42, 0x10, 
    0x07, 0x2c, 0x2e, 0x89, 0x6c, 0x10, 
};

static const uint16_t mf_rlefont_UI2_glyph_offsets_0[95] = {
    0x0000, 0x0003, 0x0008, 0x000c, 0x0018, 0x0025, 0x0036, 0x0045, 
    0x0048, 0x004e, 0x0054, 0x005e, 0x0067, 0x006c, 0x0070, 0x0075, 
    0x007e, 0x008c, 0x0095, 0x009e, 0x00a8, 0x00b4, 0x00be, 0x00c5, 
    0x00ce, 0x00d6, 0x00de, 0x00e4, 0x00eb, 0x00f4, 0x00fc, 0x0104, 
    0x010e, 0x011d, 0x012a, 0x0133, 0x013c, 0x0145, 0x014e, 0x0154, 
    0x0160, 0x0169, 0x016e, 0x0174, 0x0181, 0x0187, 0x0196, 0x01a6, 
    0x01ac, 0x01b4, 0x01bc, 0x01c6, 0x01d1, 0x01d7, 0x01e0, 0x01ec, 
    0x01fc, 0x0206, 0x020f, 0x021a, 0x0220, 0x022a, 0x0231, 0x0238, 
    0x023e, 0x0242, 0x024a, 0x0252, 0x0258, 0x0262, 0x026b, 0x0272, 
    0x027b, 0x0285, 0x028b, 0x0293, 0x029b, 0x02a0, 0x02ae, 0x02b7, 
    0x02be, 0x02c6, 0x02cf, 0x02d6, 0x02df, 0x02e6, 0x02ed, 0x02f5, 
    0x0301, 0x0308, 0x0311, 0x0319, 0x0323, 0x0328, 0x0330, 
};

static const struct mf_rlefont_char_range_s mf_rlefont_UI2_char_ranges[] = {
    {32, 95, mf_rlefont_UI2_glyph_offsets_0, mf_rlefont_UI2_glyph_data_0},
};

const struct mf_rlefont_s mf_rlefont_UI2 = {
    {
    "UI2",
    "UI2",
    11, /* width */
    11, /* height */
    3, /* min x advance */
    13, /* max x advance */
    0, /* baseline x */
    9, /* baseline y */
    11, /* line height */
    2, /* flags */
    32, /* fallback character */
    &mf_rlefont_character_width,
    &mf_rlefont_render_character,
    },
    4, /* version */
    mf_rlefont_UI2_dictionary_data,
    mf_rlefont_UI2_dictionary_offsets,
    28, /* rle dict count */
    61, /* total dict count */
    1, /* char range count */
    mf_rlefont_UI2_char_ranges,
};

#ifndef MF_SCALEDFONT_INTERNALS
#define MF_SCALEDFONT_INTERNALS
#endif
#include "mf_scaledfont.h"

const struct mf_scaledfont_s mf_rlefont_UI2_Narrow = {
	    {
			"UI2 Narrow",
			"UI2 Narrow",
			11, /* width */
			22, /* height */
			3, /* min x advance */
			13, /* max x advance */
			0, /* baseline x */
			18, /* baseline y */
			22, /* line height */
			2, /* flags */
			32, /* fallback character */
			&mf_scaled_character_width,
			&mf_scaled_render_character,
	    },
	    (struct mf_font_s*)&mf_rlefont_UI2,
		1, 2,
};

const struct mf_scaledfont_s mf_rlefont_UI2_Double = {
	    {
			"UI2 Double",
			"UI2 Double",
			22, /* width */
			22, /* height */
			6, /* min x advance */
			26, /* max x advance */
			0, /* baseline x */
			18, /* baseline y */
			22, /* line height */
			2, /* flags */
			32, /* fallback character */
			&mf_scaled_character_width,
			&mf_scaled_render_character,
	    },
	    (struct mf_font_s*)&mf_rlefont_UI2,
		2, 2,
};

#ifdef MF_INCLUDED_FONTS
/* List entry for searching fonts by name. */
static const struct mf_font_list_s mf_rlefont_UI2_Double_listentry = {
    MF_INCLUDED_FONTS,
    (struct mf_font_s*)&mf_rlefont_UI2_Double
};
static const struct mf_font_list_s mf_rlefont_UI2_Narrow_listentry = {
	&mf_rlefont_UI2_Double_listentry,
    (struct mf_font_s*)&mf_rlefont_UI2_Narrow
};
static const struct mf_font_list_s mf_rlefont_UI2_listentry = {
	&mf_rlefont_UI2_Narrow_listentry,
    (struct mf_font_s*)&mf_rlefont_UI2
};
#undef MF_INCLUDED_FONTS
#define MF_INCLUDED_FONTS (&mf_rlefont_UI2_listentry)
#endif
