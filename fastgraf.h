#include <graphics.h>
#define TEXT 0x03

/*------------------------------------------------------------------------*/

unsigned char far *video_buffer = (unsigned char far *)0xA0000000L;
unsigned char* virtual_screen;
unsigned char palette[256][3];

/*------------------------------------------------------------------------*/

void putPixel(unsigned int x, unsigned int y, unsigned char color);

/*------------------------------------------------------------------------*/

void getPaletteCell(unsigned char ColorNo, 
                    unsigned char* r,
                    unsigned char* g,
                    unsigned char* b);

void setPaletteCell(unsigned char ColorNo, 
                    unsigned char r,
                    unsigned char g,
                    unsigned char b);

/*------------------------------------------------------------------------*/

void grabPalette();

void sendPalette();

void blackPalette();

void fadeUp();

void fadeDown();

void waitRetrace();

/*------------------------------------------------------------------------*/

/*
void randDraw(void);
int random(int max);
*/

/*------------------------------------------------------------------------*/

void putPixel(unsigned int x, unsigned int y, unsigned char color) {
  unsigned int offset = (y << 8) + (y << 6) + x;
	video_buffer[offset] = (unsigned char)color;
}

/*------------------------------------------------------------------------*/

void getPaletteCell(unsigned char ColorNo, 
                    unsigned char* r,
                    unsigned char* g,
                    unsigned char* b) {
	outp(0x3C7, ColorNo);
	*r = (unsigned char)inp(0x3C9);
	*g = (unsigned char)inp(0x3C9);
	*b = (unsigned char)inp(0x3C9);
}

/*------------------------------------------------------------------------*/

void setPaletteCell(unsigned char ColorNo, 
                    unsigned char r,
                    unsigned char g,
                    unsigned char b) {
	outp(0x3C8, ColorNo);
	outp(0x3C9, r);
	outp(0x3C9, g);
	outp(0x3C9, b);
}

/*------------------------------------------------------------------------*/

void grabPalette() {
	int i;
	for (i=0; i<256; ++i)	{
		getPaletteCell(i, &palette[i][0], &palette[i][1], &palette[i][2]);
	}
}

/*------------------------------------------------------------------------*/

void sendPalette() {
	int i;
	for (i=0; i<256; ++i)	{
		setPaletteCell(i, palette[i][0], palette[i][1], palette[i][2]);
	}
}

/*------------------------------------------------------------------------*/

void blackPalette() {
	int i;
	for (i=0; i<256; ++i)	{
		setPaletteCell(i, 0, 0, 0);
	}
}

/*------------------------------------------------------------------------*/

fadeUp() {
	int i, j;
	unsigned char temp[3];
	for (i=0; i<64; ++i) {
		waitRetrace();
		for (j=0; j<256; ++j) {
			getPaletteCell(j, &temp[0], &temp[1], &temp[2]);
			if (temp[0] > palette[j][0]) temp[0]--;
			if (temp[1] > palette[j][1]) temp[1]--;
			if (temp[2] > palette[j][2]) temp[2]--;
			if (temp[0] < palette[j][0]) temp[0]++;
			if (temp[1] < palette[j][1]) temp[1]++;
			if (temp[2] < palette[j][2]) temp[2]++;
			setPaletteCell(j, temp[0], temp[1], temp[2]);
		}
	}
}

/*------------------------------------------------------------------------*/

fadeDown() {
	int i, j;
	unsigned char temp[3];
	for (i=0; i<64; ++i) {
		waitRetrace();
		for (j=0; j<256; ++j) {
			getPaletteCell(j, &temp[0], &temp[1], &temp[2]);
			if (temp[0] > 0) temp[0]--;
			if (temp[1] > 0) temp[1]--;
			if (temp[2] > 0) temp[2]--;
			setPaletteCell(j, temp[0], temp[1], temp[2]);
		}
	}
}

/*------------------------------------------------------------------------*/

waitRetrace() {
	while (!(inp(0x3DA) & 0x08)) {}
	while (inp(0x3DA) & 0x08) {}
}

/*------------------------------------------------------------------------*/
/*
int random(int max_) {
	return (int) drand(max_);
}
*/
/*------------------------------------------------------------------------*/
/*
randDraw()
{
	int i;
	int j;
	for (i=0; i<320; i++)
	{
		for (j=0; j<200; j++)
		{
			putPixel(i, j, random(255));
		}
	}
}
*/
/*------------------------------------------------------------------------*/
