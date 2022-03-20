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

// color = AARRGGBB
void drawColor(int width, int height, int color)
{
    uint32_t *canvas = new uint32_t[width * height];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            canvas[x + y * width] = color;
        }
    }

    int fd;
    fd = open("/dev/fb0", O_WRONLY);
    write(fd, canvas, width * height * 4);
    close(fd);
    delete[] canvas;
}

int main(void)
{
	drawColor(480, 320, 0x700000FF);
	return 0;
}
