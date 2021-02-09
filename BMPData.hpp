#ifndef BMPDATA_HPP_
#define BMPDATA_HPP_
#include "RGBData.hpp"

class BMPData {
	public:
		const static uchar W = 0;
		const static uchar H = 1;

		ifstream bmp;
		ulong qScore = 0;								// Quality score used by getDifference ()
		ulong scale[2];
		ulong whdiff = 0;
		ulong ppms[2];									// Pixels per meter
		ulong imgLoc;
		ushort planes;
		ushort bpp;										// Bits per pixel
		ulong fsize;
		ulong hsize;
		vector<RGBData> imgBits;
		vector<uchar> trueBitmap;						// Only used for getHexMap ()'s output

		vector<RGBData> smoothenBits () {
			vector<RGBData> avg;

			for (ulong i = 0; i < imgBits.size (); i++) {
				if (i != imgBits.size ()-1) avg.push_back (imgBits[i].getAvg (imgBits[i+1]));
				else avg.push_back (imgBits[i]);
			}

			return avg;
		}

		static vector<RGBData> getAvg (vector<RGBData> avg1, vector<RGBData> avg2) {
			vector<RGBData> result;
			for (ulong i = 0; i < avg1.size (); i++)
				result.push_back (avg1[i].getAvg (avg2[i]));

			return result;
		}

		vector<RGBData> getDifference (BMPData &bmpFile) {
			vector<RGBData> secondMap = bmpFile.imgBits;
			if (imgBits.size () != secondMap.size ()) {
				cerr << "Data is not of equal length\n";
				exit (ERR_INEQUAL_SIZES);
			}

			vector<RGBData> diff;
			vector<RGBData> avg1 = smoothenBits ();
			vector<RGBData> avg2 = bmpFile.smoothenBits ();
			vector<RGBData> avgFinal = getAvg (avg1, avg2);

			cout << "Calculating difference...\n";
			for (ulong i = 0; i < imgBits.size (); i++) {
				uchar r = RGBData::R;
				uchar g = RGBData::G;
				uchar b = RGBData::B;

				uchar cols[RGBData::MAXCOLORS] = { secondMap [i].getColor (r), secondMap [i].getColor (g), secondMap [i].getColor (b) };

				uchar rDiff = abs ((int)(cols[r]) - (int)(imgBits[i].getColor (r)));	// getColor is recognized by MinGW, but Eclipse does not seem to recognize it
				uchar gDiff = abs ((int)(cols[g]) - (int)(imgBits[i].getColor (g)));
				uchar bDiff = abs ((int)(cols[b]) - (int)(imgBits[i].getColor (b)));

				uchar rQuality1 = abs ((int)avgFinal[i].getColor (r) - (int)imgBits[i].getColor (r));
				uchar gQuality1 = abs ((int)avgFinal[i].getColor (g) - (int)imgBits[i].getColor (g));
				uchar bQuality1 = abs ((int)avgFinal[i].getColor (b) - (int)imgBits[i].getColor (b));

				uchar rQuality2 = abs ((int)avgFinal[i].getColor (r) - (int)bmpFile.imgBits[i].getColor (r));
				uchar gQuality2 = abs ((int)avgFinal[i].getColor (g) - (int)bmpFile.imgBits[i].getColor (g));
				uchar bQuality2 = abs ((int)avgFinal[i].getColor (b) - (int)bmpFile.imgBits[i].getColor (b));

				bool qScores1[RGBData::MAXCOLORS] = { rQuality1 < rQuality2, gQuality1 < gQuality2, bQuality1 < bQuality2 };
				bool qScores2[RGBData::MAXCOLORS] = { rQuality1 > rQuality2, gQuality1 > gQuality2, bQuality1 > bQuality2 };

				for (uchar j = 0; j < RGBData::MAXCOLORS; j++) {
					if (qScores1[j] == qScores2[j]) continue;
					uchar qScoreColor1 = 256 - (j == 0 ? rQuality1 : (j == 1 ? gQuality1 : bQuality1));
					uchar qScoreColor2 = 256 - (j == 0 ? rQuality2 : (j == 1 ? gQuality2 : bQuality2));

					if (qScores1[j]) qScore += qScoreColor1;
					else bmpFile.qScore += qScoreColor2;
				}

				RGBData colorDiff (rDiff, gDiff, bDiff);
				diff.push_back (colorDiff);

				cout << (ulong)((double)(i * 100) / imgBits.size ()) << "% (" << i << '/' << imgBits.size () << ")" << endl;
			}
			cout << "DIFFERENCE CALCULATED\n";

			return diff;
		}

		// Could not do a static function because it did not like using arbitrary sizes
		void getHexMap () {
			cout << "Getting hex map...\n";
			for (uint i = 0; i < imgBits.size (); i++) {
				uchar result[RGBData::MAXCOLORS];
				imgBits[i].getHex (result);

				for (uchar j = 0; j < RGBData::MAXCOLORS; j++)
					trueBitmap.push_back (result[j]);

				// Bitmap row must have proceeding length difference if W > H
				if (i % scale[W] == scale[W] - 1)
					for (uint j = 0; j < whdiff; j++)
						trueBitmap.push_back (0);
				cout << (ulong)((double)(i * 100) / imgBits.size ()) << "% (" << i << '/' << imgBits.size () << ")" << endl;
			}
			cout << "HEX MAP CREATED\n";
		}
};
#endif
