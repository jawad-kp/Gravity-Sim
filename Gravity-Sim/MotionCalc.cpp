
#include<GL/glew.h>
#include<glut/glut.h>
#include<gl/GLU.h>//this is apparantly built in. Even glew was built in. We techically only needed to set the preprocessor thingy for it. GL is our include and gl is the built-in shit from Visual Studio.
#include<iostream>
#include<string>
//#include<sstream>

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
	UNKNOWN = -1, START_SCREEN = 3, MENU = 4, PRJMTN_INP_FIRSTVAR = 0, PRJMTN_INP_SECONDVAR = 1, 
	PRJMTN_DISP = 5, DROP_INP_HT = 2, DROP_DISP = 6, ABOUT_PAGE = 7

};//the input states get 0 - 2 so I can use the arrays and not duplicate code for everything here.

bool TakeInput = false;
//AppStat is an enum that we can use to tell our display mode to switch to a particular mode. 
/* Unknown we can use to do Error Handling
	START_SCREEN tells it to display the first initial start screen, MENU is similar
	PRJMTN = Projectile Motion
	INP = input mode
	the next part tells us what we're taking in input for. I'll rename it optimise once we have a calc function that tells us what inputs it needs and compute the equation for it.
	DISP = Show the animation
	DROP = Dropping an object
	ABOUT_PAGE = redirect to about page
	*/



AppStat DispStat = AppStat::UNKNOWN;


float values[3];//the values we need are stored in this array as floats

std::string inp[3];//we save the strings we want to flush in this. They're small numbers so this should work fine for our purposes. Saving it here allows us to implement backspace easily and error hnadling later on is also easier

void KeyProc(unsigned char key, int x, int y)//This is function bound to the keystroke from my keyboard
{
	
	if (key == 'q')//for debug, remove in deploy
	{
		exit(0);

	}
	if (key == 'b')//Remove on deploy Use this to go back to menu incase you need to test something.
	{
		DispStat = AppStat::MENU;
		glutPostRedisplay();
	}
	if (TakeInput)//we process and take input here. common processing for each varaible
	{
		if (key == 13)
		{
			//enter key	
			TakeInput = false;
			glutPostRedisplay();
		}
		else if(key == 8)
		{
			int len = inp[(int)DispStat].length();
			inp[(int)DispStat] = inp[(int)DispStat].substr(0, (len - 1));//extract everything but the last charcter here.
			values[(int)DispStat] = std::stof(inp[(int)DispStat]);//save the value so we can do stuff with it. I'll try and flush it into a buffer later on but there's no noticable performance impact as of now.
			glutPostRedisplay();
		}
		else
		{
			inp[(int)DispStat].push_back(key);
			values[(int)DispStat] = std::stof(inp[(int)DispStat]);
			glutPostRedisplay();
		}

	}
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
	else if (DispStat == AppStat::MENU)
	{
		if (key == '1')
		{
			DispStat = AppStat::PRJMTN_INP_FIRSTVAR;
			TakeInput = true;
			glutPostRedisplay();
		}
		else if (key == '2')
		{
			DispStat = AppStat::DROP_INP_HT;
			TakeInput = true;
			glutPostRedisplay();
		}
		else if (key == '3')
		{
			DispStat = AppStat::ABOUT_PAGE;
			glutPostRedisplay();
		}
		else if (key == '4' || key == 'q' || key == 'Q')
		{
			exit(0);
		}
	}
}


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
		//std::cout << (int)DispStat << std::endl;

		glFlush();
	}

	else if (DispStat == AppStat::MENU)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Menu");
		DrawString(-0.3, 0.6, 0.0, "1) Projectile Simulation");
		DrawString(-0.3, 0.5, 0.0, "2) Dropping an Object");
		DrawString(-0.3, 0.4, 0.0, "3) About");
		DrawString(-0.3, 0.3, 0.0, "4) Exit (You can press Q to leave)");
		DrawString(-0.3, 0.1, 0.0, "Type in 1,2,3 or 4 Depending on your choice:");
		//std::cout << (int)DispStat << std::endl;
		glFlush();
	}

	else if (DispStat == AppStat::PRJMTN_INP_FIRSTVAR)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Screen");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "The Value Entered is:");
			char buf[20];
			snprintf(buf, sizeof(buf), "%f", values[0]); //converts a float into a character array so we can display it
			DrawString(-0.3, 0.3, 0.0, buf );
		}
		else
		{
			DispStat = AppStat::PRJMTN_INP_SECONDVAR;
			TakeInput = true;
			glutPostRedisplay();
			
		}
		glFlush();
	}
	else if (DispStat == AppStat::PRJMTN_INP_SECONDVAR)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Screen for the second value:");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "The Value Entered is:");
			char buf[20];
			snprintf(buf, sizeof(buf), "%f", values[1]);
			DrawString(-0.3, 0.3, 0.0, buf);
			
		}
		else
		{
			DispStat = AppStat::PRJMTN_DISP;
			
			//DEBUG was below
			/*std::cout << "\nThe first value entered is:\n";
			std::cout << values[0];
			std::cout << "\nThe second value entered is:\n";
			std::cout << values[1];*/
			glutPostRedisplay();

		}
		glFlush();
	}
	
	else if (DispStat == AppStat::DROP_INP_HT)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Drop Screen");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "The Value Entered is:");
			char buf[20];
			snprintf(buf, sizeof(buf), "%f", values[2]);//converts a float into a character array so we can display it
			DrawString(-0.3, 0.3, 0.0, buf);
			
		}
		else
		{
			DispStat = AppStat::DROP_DISP;
			//Debug stuff is here
			/*std::cout << "\nThe value for height entered is:\n";
			std::cout << values[2];*/
			glutPostRedisplay();

		}
		glFlush();
		
	}
	else if (DispStat == AppStat::PRJMTN_DISP)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Here we demo the projectile motion:");
		glFlush();
	}
	else if (DispStat == AppStat::DROP_DISP)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Here we demo the drop thingy");
		glFlush();
	}

	else if (DispStat == AppStat::ABOUT_PAGE)
	{
		glClearColor(0.15, 0.15, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "About:");
		glFlush();
	}
	glutSwapBuffers();

	
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