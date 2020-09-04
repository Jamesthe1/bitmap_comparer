#ifndef RGBDATA_HPP_
#define RGBDATA_HPP_

class RGBData {
	public:
		const static uchar MAXCOLORS = 3;
	private:
		uchar colors[MAXCOLORS];		// Variable made private, as set and get have safety checks
	public:
		const static uchar R = 0;
		const static uchar G = 1;
		const static uchar B = 2;

		RGBData (uchar r, uchar g, uchar b) {
			colors[R] = r;
			colors[G] = g;
			colors[B] = b;
		}

		uchar getColor (uchar index) {
			if (index >= MAXCOLORS) return 0;
			return colors[index];
		}

		bool setColor (uchar index, uchar data) {
			if (index >= MAXCOLORS) return false;

			colors [index] = data;
			return true;
		}

		RGBData getAvg (RGBData altColor) {
			RGBData avg (0, 0, 0);
			for (uchar i = 0; i < MAXCOLORS; i++) {
				uchar colorAvg = (altColor.getColor (i) + colors[i]) / 2;
				avg.setColor (i, colorAvg);
			}

			return avg;
		}

		void getHex (uchar (&out)[MAXCOLORS]) {
			// Loop done because we can't make direct assignments with arrays
			for (int i = 0; i < MAXCOLORS; i++)
				out[i] = colors[MAXCOLORS-1-i];
		}
};
#endif
