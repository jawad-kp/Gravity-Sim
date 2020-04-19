
#include<GL/glew.h>
#include<glut/glut.h>
#include<gl/GLU.h>//this is apparantly built in. Even glew was built in. We techically only needed to set the preprocessor thingy for it. GL is our include and gl is the built-in shit from Visual Studio.
#include<iostream>
#include<string>
#include<sstream>

void* Currentfont; //saves the font as a void pointer


void SetFont(void* font)
{
	Currentfont = font;
}//allows you to reset the font

void DrawString(float x, float y, float z, const char* string)
{
	const char* c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glColor3f(0.0, 1.0, 1.0);
		glutBitmapCharacter(Currentfont, *c);
	}
}//This function draws our string in the position we ask it too. This way it's abstracted and cleaner if we have to display multiple strings



enum class AppStat
{
	UNKNOWN = -1, START_SCREEN = 0, MENU = 1  
};

AppStat DispStat = AppStat::UNKNOWN;

void KeyProc(unsigned char key, int x, int y)//This is function bound to the keystroke from my keyboard
{
	if (DispStat == AppStat::START_SCREEN)
	{
		if (key == 'X' || key == 'x')
		{
			DispStat = AppStat::MENU; //now we're displaying the menu so we're changing the DispStat accordingly
			glutPostRedisplay();//we need to call our displayfunc again to render the next screen instead
		}
		else if (key == 'q' || key == 'Q')
		{
			exit(0);//user go bye byeeeee, khuda haifiz userji
		}
	}
	else if (DispStat == AppStat::MENU && (key == 'q'|| key == 'Q'))
	{
		exit(0);
	}//this gonna change when I build the menu out tomorrow and try and do the input boxes too.
}
std::stringstream inp;

void disp()
{
	if (DispStat == AppStat::START_SCREEN)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Motion and Garvity Simulation");

		DrawString( 0.15, 0.7, 0.0, " -By Jawad and Gaurav");

		DrawString(-0.3, 0.0, 0.0, "Press X to Begin");
		DrawString(-0.3, -0.1, 0.0, "Press Q to Quit");
		std::cout << (int)DispStat << std::endl;

		glFlush();
	}

	else if (DispStat == AppStat::MENU)
	{
		glClearColor(0.7, 0.7, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "This is the Second Screen. Menu will be dispalyed here");
		DrawString(-0.3, -0.1, 0.0, "Press Q to Quit");
		std::cout << (int)DispStat << std::endl;
		glFlush();
	}
	//glutSwapBuffers();

	
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);//initialising glut
	DispStat = AppStat::START_SCREEN;

	glutInitDisplayMode(GLUT_RGB);

	glutInitWindowPosition(0,0);//initial position in pixels This is optional and not specifying it will mean window is at random location.
	glutInitWindowSize(1300,750);//size of the window


	glutCreateWindow("lel");
	glutDisplayFunc(disp);
	glutKeyboardFunc(KeyProc);


	glutMainLoop();


}