# bitmapcomparer source code
This repository is for a project that compares two bitmap files and outputs an image showing their differences. The purpose of this is to:
1. Compare two renders produced by different hardware
1. Output a file that shows which image was more accurate according to a presumed average

Only 24-bit bitmaps with the Windows BITMAPINFOHEADER DIB are supported, but there is some support for sRGB color space and full support for none. The source code was created in Eclipse and written in a way in hopes of being expandible.
