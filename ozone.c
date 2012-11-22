/*---------------------------------------------------------------------------
 Space Loungers 3.0                                           by Bryan Klimt
 5/11/98                                                    bklimt@gmail.com
---------------------------------------------------------------------------*/

	#include <stdio.h>		/* Standard includes */
	#include <stdlib.h>
  #include <bios.h>   /* curson, cursoff */
  #include <conio.h>  /* getch */
	#include <dos.h>		/* genInterrupt function */
	#include <time.h>		/* srand, time functions */
	#include "fastgraf.h"	/* Includes setvmode, video_buffer */

/*-------------------------------------------------------------------------*/

	#define bool int		/* Definitions for BOOL variable */
	#define TRUE 1
	#define FALSE 0

/*-------------------------------------------------------------------------*/

	#define SHIP_Y 180		/* Definitions for Static Variables */
	#define ALIENS_Y 40
	#define ENEMY_Y 1
	#define ARROW_LEFT  75
	#define ARROW_RIGHT 77

/*-------------------------------------------------------------------------*/

	typedef struct {
		int x;
		int y;
		bool button1;
		bool button2;
	} mouseInfo;

/*-------------------------------------------------------------------------*/

	int main();			/* Prototyping for ANSI-C standard */
	void initialize();
	void levelInit();
	void appMain();
	void appFire();
	void appInput();
	void drawEnemy();
	void drawShip();
	void eraseShip();
	void drawAliens();
	void fireGood();
	void fireBad();
	void drawShots();
	void drawEShot();
	void drawIndividual(int i, int j);
	void eraseIndividual(int i, int j);
	void M_getMouseStatus(mouseInfo* mouse);
	void M_enableMouse();
	void M_disableMouse();
	void M_waitforClick(mouseInfo* mouse);
	void checkHit();
	void checkEnemyHit();
	void checkWinner();
	int isWinner();

/*-------------------------------------------------------------------------*/

	mouseInfo mouse;			/* Global Variables */
	bool mousecon = TRUE;
	bool vertRet = TRUE;

	bool alienLives[7][4];
	unsigned int aliensX = 20;
	bool right = TRUE;

	bool shipRight = TRUE;
	unsigned int shipX = 160;
	unsigned int oldX = 160;

	bool shot = FALSE;
  unsigned int shotX;
	unsigned int shotY;

	bool eShot = FALSE;
	unsigned eShotX;
	unsigned int eShotY;

	unsigned int enemyX = 100;
	int index = 10;
	int delta;
	int enemyLife;
	int level = 1;

	unsigned char backup[256][3];

#define LOOKUP(i) ((unsigned char)(55+(i/320)))

/*-------------------------------------------------------------------------*/

	unsigned char alienpic[15*17] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,13,13,13,13,13,0,0,0,0,0,0,
		0,0,0,0,0,13,13,13,13,13,13,13,0,0,0,0,0,
		0,0,0,0,13,13,0,0,13,0,0,13,13,0,0,0,0,
		0,0,0,13,13,0,0,0,13,0,0,0,13,13,0,0,0,
		0,0,0,13,13,13,13,13,13,13,13,13,13,13,0,0,0,
		0,0,0,0,13,13,13,0,0,0,13,13,13,0,0,0,0,
		0,0,0,0,0,13,13,13,13,13,13,13,0,0,0,0,0,
		0,0,0,0,5,5,5,5,5,5,5,5,5,0,0,0,0,
		0,0,0,5,5,5,5,5,5,5,5,5,5,5,0,0,0,
		0,0,5,5,5,5,14,5,5,5,14,5,5,5,5,0,0,
		0,0,0,5,5,14,14,5,5,5,14,14,5,5,0,0,0,
		0,0,0,0,0,14,14,14,14,14,14,14,0,0,0,0,0,
		0,0,0,0,0,0,14,14,14,14,14,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	unsigned char enemypic[15*19] =
	{
		0,0,8,3,3,3,3,3,3,3,3,3,3,3,3,3,8,0,0,
		0,0,0,8,8,3,3,3,3,3,3,3,3,3,8,8,0,0,0,
		0,0,0,0,0,8,3,3,3,3,3,3,3,8,0,0,0,0,0,
		0,0,0,0,0,8,3,3,3,3,3,3,3,8,0,0,0,0,0,
		0,0,0,0,8,3,3,3,3,3,3,3,3,3,8,0,0,0,0,
		0,0,0,8,3,3,3,3,3,4,3,3,3,3,3,8,0,0,0,
		0,0,0,8,3,3,3,3,4,4,4,3,3,3,3,8,0,0,0,
		0,0,0,8,3,3,3,4,4,4,4,4,3,3,3,8,0,0,0,
		0,0,0,8,3,3,3,4,4,4,4,4,3,3,3,8,0,0,0,
		0,0,0,8,3,3,3,4,4,4,4,4,3,3,3,8,0,0,0,
		0,0,0,0,8,8,8,4,4,4,4,4,8,8,8,0,0,0,0,
		0,0,0,0,0,0,0,4,4,4,4,4,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0
	};

/*-------------------------------------------------------------------------*/

	int main(int argc, char* argv[]) {
		int each;
		if (argc - 1) {
			for (each = 1; each < argc; ++each) {
				if (argv[each][1] == 'k') mousecon = FALSE;
				if (argv[each][1] == 't') vertRet = FALSE;
			}
		}
		initialize();
		while (1) {
			appMain();
			appFire();
			appInput();
		}
		return 0;
	}

/*-------------------------------------------------------------------------*/

	void initialize()	{
		unsigned int i;
		int j;
		struct time timebuf;
		gettime(&timebuf);
		srand((timebuf.ti_sec * 100) + timebuf.ti_hund);
		enemyLife = level;
		for (i=0; i<7; i++) {
			for (j=0; j<4; j++) {
				alienLives[i][j] = TRUE;
			}
		}
		shot = FALSE;
		setvmode(VGA_256);
		grabPalette();
		for (i=0; i<256; i++) {
			for (j=0; j<3; j++) {
				backup[i][j] = palette[i][j];
			}
		}
		for (i=0; i<200; i++) {
      char color = (i * 63) / 200;
			palette[i+55][0] = 0;
			palette[i+55][1] = 0;
			palette[i+55][2] = color;
		}
		blackPalette();
		for (i=0; i<64000; i++) {
			video_buffer[i] = LOOKUP(i);
		}
		drawAliens();
		drawShip();
		pen_color(7);
		move_to(0, 190);
		plots("LEVEL:");
		move_to(246, 190);
		plots("ENEMY:");
		fadeUp();
	}

/*-------------------------------------------------------------------------*/

	void levelInit() {
		unsigned int i;
		int j;
		struct time timebuf;
		gettime(&timebuf);
		srand((timebuf.ti_sec * 100) + timebuf.ti_hund);
		enemyLife = level;
		for (i=0; i<7; i++) {
			for (j=0; j<4; j++) {
				alienLives[i][j] = TRUE;
			}
		}
		shot = FALSE;
		blackPalette();
		for (i=0; i<64000; i++)	{
			video_buffer[i] = LOOKUP(i);
		}
		drawAliens();
		drawShip();
		drawEnemy();
		pen_color(7);
		move_to(0, 190);
		plots("LEVEL:");
		move_to(246, 190);
		plots("ENEMY:");
		fadeUp();
	}

/*-------------------------------------------------------------------------*/

	void appMain() {
		/* Move the aliens. */
		if (right) {
			aliensX++;
			if (aliensX > 39) right = FALSE;
		} else {
			aliensX--;
			if (aliensX < 21) right = TRUE;
		}

		if (index == 10) {
			delta = (1-((int)(drand(3)-1))) << 1;
			index = 0;
		}
		enemyX+=delta;
		index++;
		if (enemyX > 300) enemyX = 300;
		if (enemyX < 1) enemyX = 1;

		if (vertRet) waitRetrace();
		drawAliens();
		if (mousecon) eraseShip();
		drawShip();
		drawEnemy();
		drawShots();
		drawEShot();
		checkWinner();
	}

/*-------------------------------------------------------------------------*/

	void appFire() {
		fireBad();
		checkEnemyHit();
		fireGood();
		checkHit();
	}

/*-------------------------------------------------------------------------*/

	void fireGood() {
		int i, j;
		unsigned int x;
		if (!shot) return;
		shotY-=6;
		if (shotY < 1) {
			shot = FALSE;
			x = (shotX+(shotY*320));
			for (j=0; j<6; j++)	{
				for (i=0; i<3; i++)	{
					video_buffer[x] = LOOKUP(x);
					x++;
				}
				x+=317;
			}
		}
	}

/*-------------------------------------------------------------------------*/

	void fireBad() {
		int i, j;
		unsigned int x;

		if (!eShot) return;
		eShotY+=6;
		if (eShotY > 190) {
			eShot = FALSE;
			x = (eShotX+(eShotY*320));
			for (j=0; j<6; j++) {
				for (i=0; i<3; i++) {
					video_buffer[x] = LOOKUP(x);
					x++;
				}
				x+=317;
			}
		}
	}

/*-------------------------------------------------------------------------*/

	void appInput() {
    int c;
		int i, j;
		if (!eShot) {
			eShotX = (enemyX + 6);
			eShotY = ENEMY_Y;
			eShot = 1;
		}
		if (mousecon) {
			M_getMouseStatus(&mouse);
			oldX = shipX;
			shipX = (mouse.x >> 1);
			if (mouse.button1 || mouse.button2) {
				if (!shot) {
					shotX = (shipX + 6);
					shotY = SHIP_Y;
					shot = 1;
				}
			}
		}
		if ((c = getkey()) == EOF) return;
		if (c == ' ') {
			if (shot == FALSE) {
				shotX = (shipX + 6);
				shotY = SHIP_Y;
				shot = TRUE;
			}
		}
		if (c == ARROW_LEFT) {
			shipRight = FALSE;
			shipX-=5;
		}
		if (c == ARROW_RIGHT) {
			shipRight = TRUE;
			shipX+=5;
		}
		if (c == '\x1b') {  /* Escape? */
			for (i=0; i<256; i++) {
				for (j=0; j<3; j++) {
					palette[i][j] = backup[i][j];
				}
			}
			fadeDown();
			setvmode(TEXT);
			sendPalette();
			exit(0);
		}
	}

/*-------------------------------------------------------------------------*/

	void drawAliens() {
		int i, j;
		for (i=0; i<7; i++) {
			for (j=0; j<4; j++) {
				if (alienLives[i][j]) drawIndividual(i, j);
			}
		}
	}

/*-------------------------------------------------------------------------*/

const unsigned int ship_offset = 57600;

	void drawShip() {
		int i, j;
		unsigned int x, y;
		x = (shipX+ship_offset);
		if (shipRight) {
			y=x-5;
		} else {
			y=x+5;
		}
		for (j=0; j<9; j++) {
			for (i=0; i<15; i++) {
				video_buffer[y] = LOOKUP(y);
				y++;
			}
			y+=305;
		}
		x+=6;
		for (j=0; j<3; j++) {
			for (i=0; i<3; i++) {
				video_buffer[x] = 7;
				x++;
			}
			x+=317;
		}
		x-=3;
		for (j=0; j<3; j++) {
			for (i=0; i<9; i++) {
				video_buffer[x] = 7;
				x++;
			}
			x+=311;
		}
		x-=3;
		for (j=0; j<3; j++) {
			for (i=0; i<15; i++) {
				video_buffer[x] = 7;
				x++;
			}
			x+=305;
		}
		pen_color(7);
		move_to(56, 190);
		plotch(level+'0');
		move_to(302, 190);
		plotch(enemyLife+'0');
	}

/*-------------------------------------------------------------------------*/

	void eraseShip() {
		int i, j;
		unsigned int x;
		x = (oldX+ship_offset);
		x+=6;
		for (j=0; j<3; j++) {
			for (i=0; i<3; i++) {
				video_buffer[x] = LOOKUP(x);
				x++;
			}
			x+=317;
		}
		x-=3;
		for (j=0; j<3; j++) {
			for (i=0; i<9; i++) {
				video_buffer[x] = LOOKUP(x);
				x++;
			}
			x+=311;
		}
		x-=3;
		for (j=0; j<3; j++) {
			for (i=0; i<15; i++) {
				video_buffer[x] = LOOKUP(x);
				x++;
			}
			x+=305;
		}
	}

/*-------------------------------------------------------------------------*/

	drawIndividual(int ix, int jy) {
		unsigned int i, j, x, y, plotX, plotY;
    unsigned char color;
		plotX = ((40 * ix) + aliensX);
		plotY = ((30 * jy) + ALIENS_Y);

		x = y = plotY;
		y = y << 8;		/* x = (plotX+(plotY*320)); optimized*/
		x = x << 6;
		y+=x;
		x = plotX;
		x+=y;
		x--;

		y = 0;

		for (i=0; i<15; i++) {
			for (j=0; j<17; j++) {
				if (color = alienpic[y]) {
					video_buffer[x] = color;
				} else {
				  video_buffer[x] = LOOKUP(x);
				}
				x++;
				y++;
			}
			x+=303;
		}
	}

/*-------------------------------------------------------------------------*/

	void drawEnemy() {	
		int x, y, i, j;
    unsigned char color;

		x = enemyX + 320;
		y = 0;

		for (i=0; i<15; i++) {
			for (j=0; j<19; j++) {
				if (color = enemypic[y]) {
					video_buffer[x] = color;
				} else {
				  video_buffer[x] = LOOKUP(x);
				}
				x++;
				y++;
			}
			x+=301;
		}
	}

/*-------------------------------------------------------------------------*/

	void drawShots() {
		int i, j;
		unsigned int x;
		if (!shot) return;
		x = (shotX+(shotY*320));
		for (j=0; j<6; j++) {
			for (i=0; i<3; i++) {
				video_buffer[x] = LOOKUP(x);
				x++;
			}
			x+=317;
		}
		x = (shotX+((shotY-6)*320));
		for (j=0; j<6; j++) {
			for (i=0; i<3; i++) {
        unsigned char color = 5;
				video_buffer[x] = color;
				x++;
			}
			x+=317;
		}
	}

/*-------------------------------------------------------------------------*/

	void drawEShot() {
		int i, j;
		unsigned int x;
		if (!eShot) return;
		x = (eShotX+(eShotY*320));
		for (j=0; j<6; j++) {
			for (i=0; i<3; i++) {
				video_buffer[x] = LOOKUP(x);
				x++;
			}
			x+=317;
		}
		x = (eShotX+((eShotY+6)*320));
		for (j=0; j<6; j++) {
			for (i=0; i<3; i++) {
        unsigned char color = 25;
				video_buffer[x] = color;
				x++;
			}
			x+=317;
		}
	}

/*-------------------------------------------------------------------------*/

	void checkHit() {  /* Did I hit one? */
		unsigned char i, j;
		unsigned int x, y;
		unsigned int gridX, gridY;
		unsigned int possibleX, possibleY;
		if (!shot) return;
		if (shotY > 160) return;
		if (shotY > 20) {
			gridX = (int)((shotX - aliensX)/40);
			gridY = (int)((shotY - ALIENS_Y)/30);
			possibleX = ((gridX*40)+aliensX);
			possibleY = ((gridY*30)+ALIENS_Y);
			if (shotX >= (possibleX+15)) return FALSE;
			if (shotY >= (possibleY+15)) return FALSE;
			if (!alienLives[gridX][gridY]) return FALSE;
			alienLives[gridX][gridY] = FALSE;
			x = y = possibleY-5;
			y = y << 8;		/* x = (possX+(possY*320)); optimized*/
			x = x << 6;
			y+=x;
			x = possibleX-5;
			x+=y;
			for (i=0; i<30; i++) {
				for (j=0; j<30; j++) {
				  video_buffer[x] = LOOKUP(x);
					x++;
				}
				x+=290;
			}
			shot = FALSE;
			enemyLife++;
      if (enemyLife > 5) {
        enemyLife = 5;
      }
		} else {
			if ((shotX < (enemyX+13)) && (shotX > (enemyX-3))) {
				x = enemyX + 320;
				shot = FALSE;
				for (gridX=0; gridX<50; gridX++) {
					y = gridY = x;
					y = 0;
					for (i=0; i<15; i++) {
						for (j=0; j<19; j++) {
							if (enemypic[y]) video_buffer[gridY] = 7;
							gridY++;
							y++;
						}
						gridY+=301;
					}
				}
				sound(25, 10);
				sound(30, 10);
				sound(25, 10);
				x = y = shotY-5;
				y = y << 8;		/* x = (shotX+(shotY*320)); optimized */
				x = x << 6;
				y+=x;
				x = shotX-5;
				x+=y;
				for (i=0; i<20; i++) {
					for (j=0; j<20; j++) {
				    video_buffer[x] = LOOKUP(x);
						x++;
					}
					x+=300;
				}
				enemyLife--;
				if (enemyLife == 0) {
					level++;
					levelInit();
				}
			}
		}
	}

/*-------------------------------------------------------------------------*/

	void checkEnemyHit() {  /* Did I hit one? */
		int i, j;
	  unsigned int x, y;
		int gridX, gridY;
		int possibleX, possibleY;
		if (!eShot) return FALSE;
		if (eShotY < 170) {
			gridX = (int)((eShotX - aliensX)/40);
			gridY = (int)((eShotY - ALIENS_Y)/30);
			possibleX = ((gridX*40)+aliensX);
			possibleY = ((gridY*30)+ALIENS_Y);
			if (eShotX >= (possibleX+15)) return FALSE;
			if (eShotY >= (possibleY+15)) return FALSE;
			if (!alienLives[gridX][gridY]) return FALSE;
			alienLives[gridX][gridY] = FALSE;
			x = y = possibleY-10;
			y = y << 8;		/* x = (possX+(possY*320)); optimized*/
			x = x << 6;
			y+=x;
			x = possibleX-10;
			x+=y;
			for (i=0; i<50; i++) {
				for (j=0; j<50; j++) {
				  video_buffer[x] = LOOKUP(x);
					x++;
				}
				x+=270;
			}
			eShot = FALSE;
			enemyLife++;
      if (enemyLife > 5) {
        enemyLife = 5;
      }
		} else {
			if ((eShotX < (shipX+13)) && (eShotX > (shipX-3))) {
				levelInit();
			}
		}
	}

/*-------------------------------------------------------------------------*/

	void checkWinner() {  /* See if I won. */
    int c;
		int i, j;
		if (!isWinner()) return;
		if (mousecon) {
			M_enableMouse();
			poscurs(7, 9);
			printf("     You Destroyed     ");
			poscurs(8, 9);
			printf("    the Environment!!! ");
			poscurs(10, 9);
			printf("    Click anywhere...  ");
			M_waitforClick(&mouse);
			M_disableMouse();
		} else {
			cursoff();
			poscurs(8, 13);
			printf("   Loser!!!    ");
			poscurs(9, 13);
			printf(" Press enter... ");
			c = getch();
			while (c == ' ') {
				c = getch();
			}
		}
		for (i=0; i<256; i++) {
			for (j=0; j<3; j++) {
				palette[i][j] = backup[i][j];
			}
		}
		fadeDown();
		setvmode(TEXT);
		sendPalette();
		exit(0);
	}

/*-------------------------------------------------------------------------*/

	int isWinner() {  /* Did I kill 'em all? */
		int i, j;
		for (i=0; i<7; i++) {
			for (j=0; j<4; j++) {
				if (alienLives[i][j]) return 0;
			}
		}
		return 1;
	}

/*-------------------------------------------------------------------------*/

	void M_getMouseStatus(mouseInfo* mouse) {
		_AX = 3;
		geninterrupt(0x33);
		mouse->button1 = _BX & 1;
		mouse->button2 = _BX & 2; 
		mouse->x = _CX;
		mouse->y = _DX;
	}

	void M_enableMouse() {
		cursoff();
		_AX = 1;
		geninterrupt(0x33);
	}

	void M_disableMouse() {
		_AX = 2;
		geninterrupt(0x33);
		curson();
	}

	void M_waitforClick(mouseInfo* mouse) {
		M_getMouseStatus(mouse);
		while (mouse->button1) M_getMouseStatus(mouse);
		while (!mouse->button1) M_getMouseStatus(mouse);
		while (mouse->button1) M_getMouseStatus(mouse);
	}
