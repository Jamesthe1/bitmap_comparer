#include <iostream>
#include <fstream>

#define WRITE(s, sz) out.write (s, sz)
#define ulong unsigned long

using namespace std;
/*
 * All bytes are written left-to-right, and in Windows BITMAPINFOHEADER format
 * Documentation on creation: https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
 */
bool makeBMP (char size[4], char startingAddr[4], char headSize[4], char scale[2][4], char planes[2], char colorDepth[2], char ppms[2][4], bool hasColorData, ulong startAddrLong, char *data, ulong dataSize, string filename) {
	const char head[2] = {'B', 'M'};
	const char reserved[2] = {};
	const char compression[4] = {};
	const char imgSize[4] = {};
	const char colorPalette[4] = {};
	const char importantColors[4] = {};

	ofstream out;
	out.open (filename, ios::out | ios::binary);
	if (!out.is_open ()) {
		cerr << "Could not create file \"" << filename << "\"\n";
		return false;
	}

	WRITE (head, 2); WRITE (size, 4); WRITE (reserved, 2); WRITE (reserved, 2); WRITE (startingAddr, 4);
	WRITE (headSize, 4); WRITE (scale[0], 4); WRITE (scale[1], 4); WRITE (planes, 2); WRITE (colorDepth, 2);
	WRITE (compression, 4); WRITE (imgSize, 4); WRITE (ppms[0], 4); WRITE (ppms[1], 4); WRITE (colorPalette, 4);
	WRITE (importantColors, 4);
	if (hasColorData) {
		const char byteOrder[4] = {'B', 'G', 'R', 's'};
		const char gap1[48] = {};
		const char gap2[12] = {};
		const char colorEncode[4] = {2};

		WRITE (byteOrder, 4); WRITE (gap1, 48); WRITE (colorEncode, 4); WRITE (gap2, 12);
	}
	WRITE (data, dataSize);

	cout << "Write successful\n";

	out.close ();

	return true;
}
