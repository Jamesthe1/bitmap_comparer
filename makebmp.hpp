#ifndef MAKEBMP_HPP_
#define MAKEBMP_HPP_
																									 // Pixels per meter
bool makeBMP (char size[4], char startingAddr[4], char headSize[4], char scale[2][4], char planes[2], char colorDepth[2], char ppms[2][4], bool hasColorData, ulong startAddrLong, char *data, ulong dataSize, string filename);
#endif
