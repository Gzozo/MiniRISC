#include "Grafika.h"
#include "freeglut.h"  

const char* title = "MiniRisc Sim 3D 0.6.2";
//0.1: Mûködõ regiszterek, grafika
//0.2: Database
//0.3: Def
//0.3.5: Led
//0.4 Modulok
//0.4.1 7 segmenses kijelző
//0.4.2 Mátrix kijelző
//0.5 Grafika
//0.6 Bemenet kezelése
//0.6.1 Nyomógomb
//0.6.2 Switch


int refreshMills = 1000 / 60;
Processzor* cpu;
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

int eltolas = -30;
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (size_t i = 0; i < cpu->modulok.size(); i++)
	{
		cpu->modulok[i]->Draw();
	}

	/*std::vector<std::string> s = cpu->RegToString();
	for (size_t i = 0; i < s.size(); i++)
	{
		if (h - eltolas - ((int)i + 3) * 24 <= 0 && i < s.size() - 1)
		{
			glRasterPos2i(10, h - eltolas - (i + 2) * 24);
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)(s[i] + s[i + 1]).c_str());
			break;
		}
		else if (h - eltolas - (i + 2) * 24 > 0)
		{
			glRasterPos2i(10, h - eltolas - (i + 2) * 24);
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)s[i].c_str());
		}
	}*/

	glutSwapBuffers();
}
void mouseWheel(int wheel, int direction, int x, int y)
{
	if (wheel == 0)
		eltolas += direction * 5;
	glutPostRedisplay();
}
void timer(int value)
{
	if (!cpu->End())
	{
		//(*cpu)();
		glutPostRedisplay();
		glutTimerFunc(refreshMills, timer, 0);
	}

}
void Mouse(int button, int lenyom, int x, int y)
{
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	if (lenyom == 0)
	{
		for (size_t i = 0; i < cpu->hitbox.size(); i++)
		{
			cpu->hitbox[i]->Detection(x, h - y);
		}
	}
	else
	{
		for (size_t i = 0; i < cpu->hitbox.size(); i++)
		{
			cpu->hitbox[i]->Callback();
		}
	}
}
void Init(int argc, char** argv, Processzor* proc)
{
	cpu = proc;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Enable double buffered mode
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);//Kulonbozo opciok a freegluthoz

	glutInitWindowSize(960, 480);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow(title);          // Create window with the given title
	initGL();

	glutDisplayFunc(Display);
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(Mouse);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();
}
void Refresh()
{
	glutPostRedisplay();
}
