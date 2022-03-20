#include <iostream>
#include <chrono>
#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/omapfb.h>

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

void scale(uint32_t*, uint32_t*, float);

int main(const int argc, const char** argv)
{
    std::chrono::system_clock::time_point  start, end;
    uint16_t fmt, ver, width, height;
    uint32_t nof;
    float mspf;
    FILE* f = nullptr;
    if(argc < 2) {
	std::cerr << "Usage " << argv[0] << " input" << std::endl;
	return EXIT_FAILURE;
    }

    f = fopen(argv[1], "rb");
    if(f == NULL) {
	std::cerr << "Could not open " << argv[1] << std::endl;
	return EXIT_FAILURE;
    }
    // read format
    fread(&fmt, sizeof(uint16_t), 1, f);
    // read version
    fread(&ver, sizeof(uint16_t), 1, f);
    // read width
    fread(&width, sizeof(uint16_t), 1, f);
    // read height
    fread(&height, sizeof(uint16_t), 1, f);
    // read # of frame
    fread(&nof, sizeof(uint32_t), 1, f);
    // read time per frame(ms)
    fread(&mspf, sizeof(uint32_t), 1, f);

    fprintf(stdout, "FORMAT       :0x%04X\n", fmt);
    fprintf(stdout, "VERSION      :0x%04X\n", ver);
    fprintf(stdout, "WIDTH        :%d\n", width);
    fprintf(stdout, "HEIGHT       :%d\n", height);
    fprintf(stdout, "# of frame   :%d\n", nof);
    fprintf(stdout, "ms per frame :%.3f\n", mspf);

    uint32_t *frame_info = new uint32_t[width * height];
    uint32_t *canvas = new uint32_t[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    for(int i = 0; i < nof; ++i) {
	int fd;
	start = std::chrono::system_clock::now();
	fd = open("/dev/fb0", O_WRONLY);
	memset(canvas, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
	fread(frame_info, sizeof(uint32_t), width * height, f);
	scale(canvas, frame_info, DISPLAY_WIDTH/width);
	write(fd, canvas, DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
	close(fd);
	end = std::chrono::system_clock::now();
	auto time = end - start;
	auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
	while(msec < mspf) {
		end = std::chrono::system_clock::now();
		time = end - start;
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
	}
    }
    delete[] canvas;
    delete[] frame_info;
    return 0;
}


void scale(uint32_t* dest, uint32_t* src, float scale) {
    if(scale == 0.5) {
	for(int y = 0; y < DISPLAY_HEIGHT; ++y) {
	    uint32_t* pdest = &dest[DISPLAY_WIDTH * y];
	    uint32_t* psrc = &src[DISPLAY_WIDTH*2 * y];
	    for(int x = 0; x < DISPLAY_WIDTH; ++x) {
		pdest[x] = psrc[2*x];
	    }
	}
    }
    else if(scale == 2.0) {
	for(int y = 0; y < DISPLAY_HEIGHT; y+=2) {
	    uint32_t* pdest = &dest[DISPLAY_WIDTH * y];
	    uint32_t* pdest2 = &dest[DISPLAY_WIDTH * (y + 1)];
	    uint32_t* psrc = &src[DISPLAY_WIDTH/2 * y/2];
	    for(int x = 0; x < DISPLAY_WIDTH; x+=2) {
		pdest[x] = psrc[x/2];
		pdest2[x] = psrc[x/2];
		pdest[x+1] = psrc[x/2];
		pdest2[x+1] = psrc[x/2];
	    }
	}
    }
}
