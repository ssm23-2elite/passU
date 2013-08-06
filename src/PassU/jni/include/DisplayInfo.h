#ifndef DISPLAYINFO_H_
#define DISPLAYINFO_H_

#include <stdint.h>

class DisplayInfo{
public:
	bool			initialized;
	uint32_t		width;
	uint32_t		height;
	uint8_t			pixelFormat;
	uint8_t			byteperpixel;
	uint32_t		fullbyte;

public:
	DisplayInfo();
	~DisplayInfo();

};

#endif //DISPLAYINFO_H
