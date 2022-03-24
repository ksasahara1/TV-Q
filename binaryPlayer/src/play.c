#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <jpeglib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>

int calc_time(struct timespec* start_time, struct timespec* end_time) {
    int nsec;
    nsec = end_time->tv_nsec - start_time->tv_nsec;
    return (nsec / 1000.0) ;
}

int main(int argc, char** argv) {
    if(argc < 2) {
	fprintf(stderr, "Usage %s input\n", argv[0]);
	return EXIT_FAILURE;
    }
    static struct jpeg_decompress_struct cinfo;
    static struct jpeg_error_mgr jerr;
    uint16_t fmt, ver, width, height;
    uint32_t nof, frame_size;
    uint8_t* buffer;
    float mspf;
    FILE* f = NULL, *g = NULL;
    uint8_t* bmp_buffer;
    uint8_t* buffer_array[1];
    uint8_t* tmpArea = NULL;
    FILE* tmp = NULL;

    struct timespec start_time, end_time;

    int fp = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize = 0;
    char* fbp = 0;

    // open frame buffer
    fp = open("/dev/fb0", O_RDWR);

    if(fp < 0) {
	fprintf(stderr, "cannot open screen\r\n");
	return EXIT_FAILURE;
    }
    if (ioctl(fp,FBIOGET_FSCREENINFO,&finfo)){
	printf("Error reading fixed information/n");
	exit(2);
    }
    if (ioctl(fp,FBIOGET_VSCREENINFO,&vinfo)){
	printf("Error reading variable information/n");
	exit(3);
    }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // mmap
    fbp =(char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fp,0);

    if ((int) fbp == -1)
    {
	printf ("Error: failed to map framebuffer device to memory./n");
	exit (4);
    }


    bmp_buffer = (uint8_t*)malloc(480 * 320 * 3);

    f = fopen(argv[1], "rb");
    if(f == NULL) {
	/* std::cerr << "Could not open " << argv[1] << std::endl; */
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
    // read height
    fread(&nof, sizeof(uint32_t), 1, f);
    // read ms per frame
    fread(&mspf, sizeof(float), 1, f);

    fprintf(stdout, "FORMAT     :0x%04X\n", fmt);
    fprintf(stdout, "VERSION    :0x%04X\n", ver);
    fprintf(stdout, "WIDTH      :%d\n", width);
    fprintf(stdout, "HEIGHT     :%d\n", height);
    fprintf(stdout, "# of frame :%d\n", nof);
    fprintf(stdout, "mili second per frame :%.2f\n", mspf);

    // read frame size
    for(int i = 0; i < nof; ++i) {
	clock_gettime(CLOCK_REALTIME, &start_time);
	fread(&frame_size, sizeof(uint32_t), 1, f);
	// fprintf(stdout, "nbytes of frame :%d\n", frame_size);
	buffer = (uint8_t*)malloc(sizeof(uint8_t)*frame_size);
	fread(buffer, sizeof(uint8_t), frame_size, f);

	jpeg_create_decompress(&cinfo);
	cinfo.err = jpeg_std_error(&jerr);
	g = fmemopen(buffer, sizeof(uint8_t)*frame_size, "rb");
	jpeg_stdio_src(&cinfo, g);
	jpeg_read_header(&cinfo, TRUE);

	int width = cinfo.image_width;
	int height = cinfo.image_height;
	int ch = cinfo.num_components;

	jpeg_start_decompress(&cinfo);

	while (cinfo.output_scanline < cinfo.output_height) {
	    buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * width * ch;
	    jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	for(int j = 0; j < height; ++j) {
	    for(int i = 0; i < width; ++i) {
		uint8_t* ptr = &bmp_buffer[j * width * ch + i * ch];
		uint8_t* rptr = (uint8_t*)&fbp[width * j * 4 + i * 4];
		for(int k = 0; k < ch; ++k) {
		    rptr[k] = ptr[ch-k-1];
		}
	    }
	}
	fclose(g);
	free(buffer);
	clock_gettime(CLOCK_REALTIME, &end_time);
// 	while(calc_time(&start_time, &end_time) < mspf) {
// 	    usleep(100);
// 	    clock_gettime(CLOCK_REALTIME, &end_time);
// 	}
    }
    fclose(f);
    free(bmp_buffer);
    munmap(fbp, screensize);
    close(fp);
    return EXIT_SUCCESS;
}

