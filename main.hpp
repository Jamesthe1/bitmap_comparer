#ifndef MAIN_HPP_
#define MAIN_HPP_

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int
#define ulong unsigned long
#define llong long long

#define BUF_SIZE 10

using namespace std;					// Entire namespace used because we have ensured nothing will clash

ushort hexStrToUShort (char *str, uchar len);
ulong hexStrToULong (char *str, uchar len);
void uLongToHexStr (ulong data, uchar *out);

// Numbers required for MinGW
enum errs {
	ERR_OK = 0,
	ERR_NO_FILE = 1,
	ERR_NOT_BMP = 2,
	ERR_NOT_EXIST = 3,
	ERR_BAD_HEADER = 4,
	ERR_INEQUAL_SCALES = 5,
	ERR_INEQUAL_SIZES = 6,		// Sizes for binary
	ERR_INEQUAL_DEPTH = 7,		// Color depth
	ERR_NOT_24 = 8,
	ERR_COMPRESS = 9,
	ERR_BADFILE = 10
};

enum bitnames {
	BIT_ZERO = 0,
	BIT_IMGSTART = 0x0A,
	BIT_WIDTH = 0x12,
	BIT_HEIGHT = 0x16,
	BIT_PLANES = 0x1A,
	BIT_PIXELLEN = 0x1C,
	BIT_FSIZE = 0x02,
	BIT_PPM_W = 0x26,
	BIT_PPM_H = 0x2A,
	BIT_COMPRESS = 0x1E,
	BIT_HEADSIZE = 0x0E,

	// No. of bytes for each variable
	LEN_HEADER = 2,
	LEN_IMGSTART = 4,
	LEN_WIDTH = 4,
	LEN_HEIGHT = 4,
	LEN_PLANES = 2,
	LEN_PIXELLEN = 2,
	LEN_FSIZE = 4,
	LEN_PPM_W = 4,
	LEN_PPM_H = 4,
	LEN_COMPRESS = 4,
	LEN_HEADSIZE = 4
};

#include "BMPData.hpp"
#include "makebmp.hpp"
#endif
