/*
 * Remoteroid - A remote control solution for Android platform, including handy file transfer and notify-to-PC.
 * Copyright (C) 2012 Taeho Kim(jyte82@gmail.com), Hyomin Oh(ohmnia1112@gmail.com), Hongkyun Kim(godgjdgjd@nate.com), Yongwan Hwang(singerhwang@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, writeD to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/



#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <jni.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/kd.h>
#include <linux/fb.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <asm/page.h>

#include "include/fbuffer.h"
#include "turbojpeg.h"


jint Java_org_secmem_remoteroid_natives_FrameHandler_getFrameBuffer  (JNIEnv * env, jobject thiz, jbyteArray jByte, jint pixelformat){
	
	int		fd;
	int		ret;

	unsigned char	*pFrame, *jpegBuf, *imageBuf;
	int *scale, *buf;
	unsigned long	jpegSize;


	static Display						disinfo;
	static struct fb_var_screeninfo		vinfo;
	struct fb_fix_screeninfo			finfo;
	
	tjhandle	 handle;

	fd = open("/dev/graphics/fb0", O_RDONLY);

	if(fd<0)
	{
		LOGD(LOGTAG, "Cannot open device - 'remoteroid'");
		return FB_FAIL;
	}

	ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

	if(ret < 0 )
	{
		LOGD(LOGTAG, "Cannot open Variable screen information. - 'remoteroid'");
		close(fd);
		return VINFO_FAIL;
	}

	ret = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);

	if(ret < 0 )
	{
		LOGD(LOGTAG, "Cannot open fixed screen information. - 'remoteroid'");
		close(fd);
		return FINFO_FAIL;
	}

	disinfo.width			= vinfo.xres/2;
	disinfo.height			= vinfo.yres/2;
	disinfo.byteperpixel	=	vinfo.bits_per_pixel/8;
	disinfo.pixelformat		=	pixelformat;

	jpegBuf = (unsigned char *) malloc(1000000);
	imageBuf = (unsigned char *)malloc(disinfo.width*disinfo.height*4);

	pFrame = mmap(0,finfo.smem_len, PROT_READ, MAP_PRIVATE, fd,0);
	
	buf = (int *) pFrame;
	scale = (int *) imageBuf;

	if(pFrame == MAP_FAILED){
		close(fd);
		return MMAP_FAIL;
	}

	/*
	for(i = 0; i < disinfo.height; i++){
		for(j = 0; j<disinfo.width; j++){
			*(scale+(i*disinfo.width)+j) = *(buf+(i*disinfo.width*4)+(j*2));
		}
	}
	*/	

	handle = tjInitCompress();

	if(disinfo.pixelformat == PIXEL_UNKNOWN)	return FORMAT_UNKNOWN;

	else
	if(disinfo.pixelformat == PIXEL_ARGB_8888){
		imageScaling(buf, scale,disinfo.width, disinfo.height);
		imageBuf = (unsigned char *) scale;
		tjCompress2(handle, imageBuf, disinfo.width, 0, disinfo.height, TJPF_RGBX, &jpegBuf,	&jpegSize, TJSAMP_444, 60, TJFLAG_NOREALLOC);
	}

	else
	if(disinfo.pixelformat == PIXEL_XBGR_8888){
		imageScaling(buf, scale,disinfo.width, disinfo.height);
		imageBuf = (unsigned char *) scale;
		 tjCompress2(handle, imageBuf, disinfo.width, 0, disinfo.height, TJPF_BGRX, &jpegBuf,	&jpegSize, TJSAMP_444, 70, TJFLAG_NOREALLOC);
	}

	else{
		return FORMAT_VALUE_FAIL;
	}
	
	munmap(pFrame,finfo.smem_len);

	(*env)->SetByteArrayRegion(env,jByte,0,jpegSize,(jbyte*)jpegBuf);

	free(imageBuf);
	free(jpegBuf);
	close(fd);
	tjDestroy(handle);
	
	return (int)jpegSize;
}	

void imageScaling(int * pF, int * iB,int width, int height){
	
	int i,j;

	for(i=0; i < height; i++){
		for(j=0; j < width; j++){
			*(iB+(i*width) +j) = imageSum((pF+(i*width*4)+ (j*2)), width);
		}
	}
}

int imageSum(int * pF , int width){
	
	int red, green, blue;
	int a, b, c;

	a = 1; b = width *2; c = width *2 +1;

	red = (*pF & 0x00ff0000)+(*(pF+a) & 0x00ff0000) + (*(pF+b) & 0x00ff0000) + (*(pF+c) & 0x00ff0000);
	red = red >>2;

	
	green = (*pF & 0x0000ff00)+(*(pF+a) & 0x0000ff00) + (*(pF+b) & 0x0000ff00) + (*(pF+c) & 0x0000ff00);
	green = green >> 2;

	
	blue = (*pF & 0x000000ff)+(*(pF+a) & 0x000000ff) + (*(pF+b) & 0x000000ff) + (*(pF+c) & 0x000000ff);
	blue = blue >> 2;

	return (red & 0x00ff0000)+(green & 0x0000ff00)+(blue & 0x000000ff);
}