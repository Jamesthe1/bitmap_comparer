#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "main.hpp"

vector<RGBData> colorDiff;
BMPData bmps[2];
string fname;
bool allPassed = false;				// Used for check in closeBMPs () to only display a specific message if the exit code is not ERR_OK

void closeBmps () {
	// Separated because atexit does not like using foreach
	if (bmps[0].bmp.is_open ())
		bmps[0].bmp.close ();
	if (bmps[1].bmp.is_open ())
		bmps[1].bmp.close ();

	if (!allPassed) cerr << "PROGRAM ABORTED\n";
}

// Because of the use of to_string (), this must be compiled in MinGW 5.2.1 until a version comes out to fix a bug or another method is used to convert numbers to strings
void writeLog (ulong qScore1, ulong qScore2, char *argv[]) {
	string winner = (qScore1 == qScore2 ? "No file" : (qScore1 > qScore2 ? argv[1] : argv[2]));
	string vs = '(' + to_string (qScore1) + " vs " + to_string (qScore2) + ')';
	string winScore = to_string ((ulong)abs ((llong)qScore2 - (llong)qScore1));
	string str = "Comparison complete. " + winner + " won with a quality score difference of " + winScore + ' ' + vs;

	ofstream logFile;
	logFile.open("logfile.txt");
	if (!logFile.is_open ()) {
		cout << "Log file not written to; install location of bmpcompare is most likely write-protected\n";
		return;
	}
	logFile.write (str.c_str (), str.length ());
	logFile.close ();

	cout << "Log file written to.\n";
}

int main (int argc, char *argv[]) {
	if (argc < 2) cerr << "Missing initial file (bmpcompare >file1< file2)\n";
	if (argc < 3) cerr << "Missing second file (bmpcompare file1 >file2<)\n";

	if (argc < 3) return ERR_NO_FILE;

	cout << "Output name: ";
	cin >> fname;
	fname += ".bmp";

	atexit (closeBmps);

	for (int i = 0; i < 2; i++) {
		string filename = argv[i+1];
		bmps[i].bmp.open (filename, ios::in | ios::binary);

		if (bmps[i].bmp.is_open ()) {
			size_t dotPos = filename.find_last_of ('.');
			// The second value should be ignored if the first is false; we will check the header if it contains no dot
			if (dotPos != (size_t)-1 && filename.substr (dotPos + 1) != "bmp") {
				cerr << "File " << filename << " is not of type bitmap (.bmp)\n";
				return ERR_NOT_BMP;
			}
		} else {
			cerr << "File " << filename << " does not exist\n";
			return ERR_NOT_EXIST;
		}

		// Get info from file according to the header formats (https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header)
		char buffer[BUF_SIZE] = {};

		bmps[i].bmp.seekg (BIT_ZERO);	// Set to bit zero for safe measure
		bmps[i].bmp.read (buffer, LEN_HEADER);
		string bmh = buffer;

		if (bmh != "BM") {
			cerr << "File " << filename << " does not have a proper bitmap header\n";
			return ERR_BAD_HEADER;
		}

		bmps[i].bmp.seekg (BIT_IMGSTART);
		bmps[i].bmp.read (buffer, LEN_IMGSTART);
		bmps[i].imgLoc = hexStrToULong (buffer, LEN_IMGSTART) + 1;	// 1 added as offset because, strangely, it acts like it starts at the bit before

		bmps[i].bmp.seekg (BIT_WIDTH);
		bmps[i].bmp.read (buffer, LEN_WIDTH);
		bmps[i].scale[BMPData::W] = hexStrToUShort (buffer, LEN_WIDTH);

		bmps[i].bmp.seekg (BIT_HEIGHT);
		bmps[i].bmp.read (buffer, LEN_HEIGHT);
		bmps[i].scale[BMPData::H] = hexStrToUShort (buffer, LEN_HEIGHT);
		bmps[i].whdiff = max ((int)bmps[i].scale[BMPData::W] - (int)bmps[i].scale[BMPData::H], 0);

		bmps[i].bmp.seekg (BIT_PLANES);
		bmps[i].bmp.read (buffer, LEN_PLANES);
		bmps[i].planes = hexStrToUShort (buffer, LEN_PLANES);

		bmps[i].bmp.seekg (BIT_PIXELLEN);
		bmps[i].bmp.read (buffer, LEN_PIXELLEN);
		bmps[i].bpp = hexStrToUShort (buffer, LEN_PIXELLEN);

		if (bmps[i].bpp != 24) {
			cerr << "File " << filename << " is not 24-bit; the program currently does not support transparency\n";
			return ERR_NOT_24;
		}

		bmps[i].bmp.seekg (BIT_PPM_W);
		bmps[i].bmp.read (buffer, LEN_PPM_W);
		bmps[i].ppms[BMPData::W] = hexStrToUShort (buffer, LEN_PPM_W);

		bmps[i].bmp.seekg (BIT_PPM_H);
		bmps[i].bmp.read (buffer, LEN_PPM_H);
		bmps[i].ppms[BMPData::H] = hexStrToUShort (buffer, LEN_PPM_H);

		bmps[i].bmp.seekg (BIT_COMPRESS);
		bmps[i].bmp.read (buffer, LEN_COMPRESS);
		if (hexStrToUShort (buffer, LEN_COMPRESS)) {
			cout << filename << " has compression; the program currently does not support this";
			return ERR_COMPRESS;
		}

		bmps[i].bmp.seekg (BIT_HEADSIZE);
		bmps[i].bmp.read (buffer, LEN_HEADSIZE);
		bmps[i].hsize = hexStrToUShort (buffer, LEN_HEADSIZE);

		cout << filename << " made it past data assignments (Data start:" << bmps[i].imgLoc << "; Size:[" << bmps[i].scale[BMPData::W] << "," << bmps[i].scale[BMPData::H] << "]; Planes:" << bmps[i].planes << "; Bits per pixel:" << bmps[i].bpp << "; Pixels per meter:[" << bmps[i].ppms[BMPData::W] << ',' << bmps[i].ppms[BMPData::H] << "])\n";

		bmps[i].bmp.seekg (BIT_FSIZE);
		bmps[i].bmp.read (buffer, LEN_FSIZE);
		bmps[i].fsize = hexStrToULong (buffer, LEN_FSIZE);

		bmps[i].bmp.seekg (bmps[i].imgLoc-1);					// Subtraction added for seekg () to start in the right place; would originally start 1 bit ahead despite curLoc being at the correct bit position

		ulong eof = bmps[i].fsize;
		ulong curLoc = bmps[i].imgLoc;
		cout << "Analyzing (this may take a while)...\n";
		while (curLoc < eof) {
			if ((curLoc - bmps[i].imgLoc) % (3 * bmps[i].scale[BMPData::W] + 1) == 3 * bmps[i].scale[BMPData::W]) {
				curLoc += bmps[i].whdiff;
				bmps[i].bmp.seekg (curLoc);
			}

			//cout << (curLoc - bmps[i].imgLoc) % (3 * bmps[i].scale[BMPData::W] + 1) << ' ' << 3 * bmps[i].scale[BMPData::W] << '\n';

			RGBData colorData (0, 0, 0);
			uchar maxColors = RGBData::MAXCOLORS;
			for (uchar j = 0; j < maxColors; j++) {
				//cout << "Accessing byte " << curLoc-1 << "... ";
				bmps[i].bmp.read (buffer, 1);
				//cout << "OK ";
				uchar realChar = (uchar)buffer[0];				// Cast created to use 0-255 instead of -127-127

																// Inverted because the bits read BGR
				//cout << (ushort)(maxColors-1-j) << '=' << (int)realChar << '\n';
				colorData.setColor (maxColors-1-j, realChar);

				bmps[i].bmp.seekg (curLoc++);					// Simultaneous writing done to compact the code
			}

			//cout << "DONE; pushing\n";

			bmps[i].imgBits.push_back (colorData);				// Eclipse shows an error but it is perfectly normal to put in a standard value; no compilation issues
			cout << (ulong)((double)(curLoc - bmps[i].imgLoc) * 100 / (eof - bmps[i].imgLoc)) << "% (" << (curLoc - bmps[i].imgLoc) << '/' << (eof - bmps[i].imgLoc) << ")\n";
		}

		bmps[i].bmp.close ();
	}

	cout << "READ COMPLETE\n";

	// Check needed just in case the files are unequal in dimension
	bool widthEqual = bmps[0].scale[BMPData::W] == bmps[1].scale[BMPData::W];
	bool heightEqual = bmps[0].scale[BMPData::H] == bmps[1].scale[BMPData::H];
	if (!widthEqual || !heightEqual) {
		cerr << "Files are not equal dimensions (" << bmps[0].scale[BMPData::W] << "x" << bmps[0].scale[BMPData::H] << ", " << bmps[1].scale[BMPData::W] << "x" << bmps[1].scale[BMPData::H] << ")\n";
		return ERR_INEQUAL_SCALES;
	}

	colorDiff = bmps[0].getDifference (bmps[1]);
	writeLog (bmps[0].qScore, bmps[1].qScore, argv);

	uchar w = BMPData::W;
	uchar h = BMPData::H;

	BMPData newData;
	newData.imgBits = colorDiff;
	newData.scale[w] = bmps[0].scale[w];
	newData.scale[h] = bmps[0].scale[h];
	newData.whdiff = bmps[0].whdiff;
	newData.getHexMap ();


	ulong trueLength = bmps[0].fsize - bmps[0].imgLoc + 1;
	cout << "Calculated byte length: " << trueLength << '\n';

	uchar bigmap[trueLength] = {};

	cout << "Converting bitmap vector to array (this may take a while)...\n";
	for (ulong i = 0; i < trueLength; i++) {
		bigmap[i] = newData.trueBitmap[i];
		cout << (ulong)((double)(i * 100) / trueLength) << "% (" << i << '/' << trueLength << ")\n";
	}
	cout << "DATA COPIED\n";

	/*for (uchar ch : bigmap) {
		cout << (short)ch << ' ';
	}*/

	uchar size[4] = {};
	uchar startAddr[4] = {};
	uchar headSize[4] = {};
	uchar scale[2][4] = {};
	uchar planes[2] = {};
	uchar colorDepth[2] = {};
	uchar ppms[2][4] = {};

	uLongToHexStr (bmps[0].fsize, size);
	uLongToHexStr (bmps[0].imgLoc-1, startAddr);
	uLongToHexStr (bmps[0].hsize, headSize);
	uLongToHexStr (bmps[0].scale[w], scale[w]);
	uLongToHexStr (bmps[0].scale[h], scale[h]);
	uLongToHexStr (bmps[0].planes, planes);
	uLongToHexStr (bmps[0].bpp, colorDepth);
	uLongToHexStr (bmps[0].ppms[w], ppms[w]);
	uLongToHexStr (bmps[0].ppms[h], ppms[h]);

	bool didPass = makeBMP ((char *)size, (char *)startAddr, (char *)headSize, (char (*)[4])scale, (char *)planes, (char *)colorDepth,
							(char (*)[4])ppms, bmps[0].hsize > 40, bmps[0].imgLoc-1, (char *)bigmap, trueLength, fname);

	if (!didPass) return ERR_BADFILE;
	allPassed = true;
	return ERR_OK;
}

/*
 * Reads big-endian as that's how data in BMP is formatted
 * char *str - The string consisting of hexadecimal values
 * uchar len - Length in bytes
 */
ushort hexStrToUShort (char *str, uchar len) {
	ushort endres = 0;
		for (int i = 0; i < len; i++)
			endres += (uchar)str[i] * pow (256, i);				// 256 is here because every byte stores up to 256 numbers

	return endres;
}

/*
 * Needed for 4-byte values
 * Refer to hexStrToUShort () for value purposes
 */
ulong hexStrToULong (char *str, uchar len) {
	ulong endres = 0;
	for (int i = 0; i < len; i++)
		endres += (uchar)str[i] * pow (256, i);

	return endres;
}

/*
 * Inverse of hexStrToULong ()
 * ulong data - The data to be put in
 * char *out - The array variable to output to
 */
void uLongToHexStr (ulong data, uchar *out) {
	int power = log (data) / log (256);							// Division of logarithms because we cannot set the log base
	for (int i = 0; i <= power; i++)
		out[i] = data / pow (16, i*2);
}
