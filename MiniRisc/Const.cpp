#include "Const.h"
#include "freeglut.h" 


#define _USE_MATH_DEFINES
#include <math.h>


std::string Hex(Cim szam)
{
	char hex[] = { '0', '1', '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  'A',  'B',  'C',  'D',  'E',  'F' };
	std::string ret;
	ret += hex[(int)szam / 16];
	ret += hex[(int)szam % 16];
	return ret;
}

void DrawCircle(double ori_x, double ori_y, double radius, bool fill)
{
	glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
	for (int i = 0; i <= 300; i++) {
		double angle = 2 * M_PI * i / 300;
		double x = cos(angle) * radius;
		double y = sin(angle) * radius;
		glVertex2d(ori_x + x, ori_y + y);
	}
	glEnd();
}
