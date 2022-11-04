#include "Moduls.h"
#include <vector>
#include <string>
#include "freeglut.h"  
#include "Processzor.h"
//
void Memoria::Draw(size_t x, size_t y)
{
	Cim* seged = new Cim[hossz];
	std::copy(cpu->AdatP(kezdo), cpu->AdatP(kezdo + hossz), seged);
	size_t i = 0;
	std::string elso = "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F";
	glRasterPos2i(x, y + 16 * ((hossz - 1) / 16) + 24);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)elso.c_str());
	for (; i < hossz;)
	{
		std::string szoveg = Hex(i) + " ";
		do
		{
			szoveg += " " + Hex(seged[i]);
			i++;
		} while (i < hossz && i % 16 != 0);

		glRasterPos2i(x, y + 16 * ((hossz - 1) / 16) + 16 - ((i - 1) / 16 + 1) * 16);
		glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)szoveg.c_str());
	}

	delete[] seged;
}

void Regiszter::Draw(size_t x, size_t y)
{
	/*int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);*/
	std::vector<Cim> s = cpu->RegToVector();
	std::string reg1 = "R0  R1  R2  R3  R4  R5  R6  R7";
	std::string reg2 = "R8  R9 R10 R11 R12 R13 R14 R15";
	glRasterPos2i(x, y + 64);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)reg1.c_str());
	glRasterPos2i(x, y + 48);
	std::string value;
	for (size_t i = 0; i < 7; i++)
	{
		value += Hex(s[i]) + "  ";
	}
	value += Hex(s[7]) + "\n\n";
	for (size_t i = 8; i < 15; i++)
	{
		value += Hex(s[i]) + "  ";
	}
	value += Hex(s[15]);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)(value.c_str()));
	glRasterPos2i(x, y);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)reg2.c_str());



}

void Led::Draw(size_t x, size_t y)
{
	std::string leds = "";
	Cim led = *(cpu->AdatP(kezdo));
	for (size_t i = 0; i < 8; i++)
	{
		DrawCircle(x + i * 30, y, 10, (led & (0b1 << (7 - i))) >> (7 - i) == 1);
	}
	glRasterPos2i(x, y);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)leds.c_str());
}
const int arany = 10;
//glBegin(GL_QAUDS)-ot meg kell hívni!
void DrawSeg(int seg, size_t x, size_t y)
{
	switch (seg)
	{
		case 0:
			glVertex2i(x + 5 * arany, y);
			glVertex2i(x + 6 * arany, y);
			glVertex2i(x + 6 * arany, y + arany);
			glVertex2i(x + 5 * arany, y + arany);
			break;
		case 1:
			glVertex2i(x + arany, y + 5 * arany);
			glVertex2i(x + 4 * arany, y + 5 * arany);
			glVertex2i(x + 4 * arany, y + 4 * arany);
			glVertex2i(x + arany, y + 4 * arany);
			break;
		case 2:
			glVertex2i(x, y + 8 * arany);
			glVertex2i(x + arany, y + 8 * arany);
			glVertex2i(x + arany, y + 5 * arany);
			glVertex2i(x, y + 5 * arany);
			break;
		case 3:
			glVertex2i(x, y + 4 * arany);
			glVertex2i(x + arany, y + 4 * arany);
			glVertex2i(x + arany, y + arany);
			glVertex2i(x, y + arany);
			break;
		case 4:
			glVertex2i(x + arany, y + arany);
			glVertex2i(x + 4 * arany, y + arany);
			glVertex2i(x + 4 * arany, y);
			glVertex2i(x + arany, y);
			break;
		case 5:
			glVertex2i(x + 4 * arany, y + arany);
			glVertex2i(x + 5 * arany, y + arany);
			glVertex2i(x + 5 * arany, y + 4 * arany);
			glVertex2i(x + 4 * arany, y + 4 * arany);
			break;
		case 6:
			glVertex2i(x + 4 * arany, y + 5 * arany);
			glVertex2i(x + 5 * arany, y + 5 * arany);
			glVertex2i(x + 5 * arany, y + 8 * arany);
			glVertex2i(x + 4 * arany, y + 8 * arany);
			break;
		case 7:
			glVertex2i(x + arany, y + 8 * arany);
			glVertex2i(x + 4 * arany, y + 8 * arany);
			glVertex2i(x + 4 * arany, y + 9 * arany);
			glVertex2i(x + arany, y + 9 * arany);
			break;
	}
}
void Seg7::Draw(size_t x, size_t y)
{

	Cim* value = cpu->AdatP(kezdo);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective
	glBegin(GL_QUADS); //Begin quadrilateral coordinates


	for (size_t db = 0; db < hossz; db++, value++)
	{
		Cim szam = *value;
		int oszt = 128;
		for (size_t i = 0; i < 8; i++)
		{
			if (szam / oszt == 1)
			{
				DrawSeg(i, x + db * 8 * arany, y);
				szam -= oszt;
			}
			else
			{
				glEnd();
				glBegin(GL_LINE_LOOP);
				DrawSeg(i, x + db * 8 * arany, y);
				glEnd();
				glBegin(GL_QUADS);
			}
			oszt /= 2;
		}
	}

	glEnd();
}

void Matrix::Draw(size_t x, size_t y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Cim* value = cpu->AdatP(kezdo);

	for (size_t darab = 0; darab < hossz; darab++, value++)
	{
		Cim ertek = *value;
		for (size_t i = 0; i < db; i++)
		{
			DrawCircle(x + darab * 15, y + (db - i - 1) * 15, 5, (ertek & (0b1 << i)) >> i == 1);
		}
	}
}

void Buttons::Draw(size_t x, size_t y)
{
	sx = x;
	sy = y;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < db; i++)
	{
		DrawCircle(x + i * tavolsag, y, radius, (buttons & (0b1 << (db - i - 1))) != 0);
	}
}

void Buttons::Callback()
{
	buttons = 0x00;
	Moduls::Write(kezdo, buttons);
}

void Buttons::Action(int bt)
{
	//buttons = (~buttons & (1 << bt)) | (buttons & (~(1 << bt))); //Ez a switch....
	buttons = buttons | (1 << bt);
	Moduls::Write(kezdo, buttons);
}

bool Buttons::Detection(size_t x, size_t y)
{
	for (int i = 0; i < db; i++)
	{
		//nem int!!!!!!
		if (pow((int)sx + i * (int)tavolsag - (int)x, 2) + pow((int)sy - (int)y, 2) <= (int)radius * (int)radius)
		{
			Action(db - i - 1);
			return true;
		}
	}
	return false;
}

void Moduls::Write(Cim addr, Cim ertek)
{
	*(cpu->AdatP(addr)) = ertek;
}

Cim Moduls::Read(Cim addr)
{
	return *(cpu->AdatP(addr));
}

void Switch::Draw(size_t x, size_t y)
{
	sx = x;
	sy = y;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < db; i++)
	{
		glBegin(GL_QUADS);
		//csúszka
		glVertex2i(x + i * tav + szel / 3, y);
		glVertex2i(x + i * tav + 2 * szel / 3, y);
		glVertex2i(x + i * tav + 2 * szel / 3, y + mag - 2);
		glVertex2i(x + i * tav + szel / 3, y + mag - 2);

		int szorzo = (sw & (1 << (db - i - 1))) == 0 ? 1 : 0;
		glVertex2i(x + i * tav, y + szorzo * 3 * mag / 4);
		glVertex2i(x + i * tav + szel, y + szorzo * 3 * mag / 4);
		glVertex2i(x + i * tav + szel, y + mag / 4 + szorzo * 3 * mag / 4);
		glVertex2i(x + i * tav, y + mag / 4 + szorzo * 3 * mag / 4);

		glEnd();
	}

}

void Switch::Action(int bt)
{
	sw = (~sw & (1 << bt)) | (sw & (~(1 << bt)));
	Moduls::Write(kezdo, sw);
}

bool Switch::Detection(size_t x, size_t y)
{
	if (y < sy || y > sy + mag || x < sx || x > sx + db * tav + szel)
		return false;
	for (int i = 0; i < db; i++)
	{
		if (x >= sx + i * tav && x <= sx + i * tav + szel)
		{
			Action(db - i - 1);
			return true;
		}
	}
	return false;
}
