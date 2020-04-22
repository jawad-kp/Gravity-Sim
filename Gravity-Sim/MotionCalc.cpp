
#include<GL/glew.h>
#include<glut/glut.h>
#include<gl/GLU.h>//this is apparantly built in. Even glew was built in. We techically only needed to set the preprocessor thingy for it. GL is our include and gl is the built-in shit from Visual Studio.
#include<iostream>
#include<string>
#include<math.h>
//#include<sstream>

void* Currentfont; //saves the font as a void pointer


void SetFont(void* font)
{
	Currentfont = font;
}//allows you to reset the font

void DrawString(double x, double y, double z, const char* string)
{
	const char* c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glColor3f(0.0, 1.0, 1.0);
		glutBitmapCharacter(Currentfont, *c);
	}
}//This function draws our string in the position we ask it too. This way it's abstracted and cleaner if we have to display multiple strings

void reshape(int width, int height)
{
	glViewport(0, 0, 750, 750);
	//glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(-10, 10, -10, 10);
	//glMatrixMode(GL_MODELVIEW);
}



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



AppStat DispStat = AppStat::DROP_DISP;


float values[3];//the values we need are stored in this array as floats

std::string inp[3];//we save the strings we want to flush in this. They're small numbers so this should work fine for our purposes. Saving it here allows us to implement backspace easily and error hnadling later on is also easier
double DropPos = 8.0, DropTime = 0.0;
double TotalDropTime = 0.0;
double DropTCalc(float);

void KeyProc(unsigned char key, int x, int y)//This is function bound to the keystroke from my keyboard
{
	
	if (key == 'q')//for debug, remove in deploy
	{
		exit(0);

	}
	if (key == 'b')//Remove on deploy Use this to go back to menu incase you need to test something.
	{
		DispStat = AppStat::MENU;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
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
			if (len - 1 <= 0)
			{
				values[(int)DispStat] = 0;
				glutPostRedisplay();
				return;
			}//if they enter backspace and there's nothing to clear.
			std::cout << len << std::endl;
			inp[(int)DispStat] = inp[(int)DispStat].substr(0, (len - 1));//extract everything but the last charcter here.
			std::cout << inp[(int)DispStat]<<std::endl;
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

void animater(int a)
{
	//glutTimerFunc(1000 / 60, animater, 0);
	if (DispStat == AppStat::DROP_DISP)
	{
		if (DropTime < TotalDropTime && DropPos > -47.5)
		{
			DropTime = DropTime + 0.016667;//this wasn't taking 1/60 for some reason and I hard-coded and made the rest doubles for it to work
			double DisInM = -4.9 * (DropTime * DropTime); // s = 0.5gt^2 => 0.5g = 4.9 (negative because we goin down.)
			std::cout << "\nSphere should've moved by:\n";
			std::cout << DisInM;
			std::cout << "\nTime Elapsed = \n";
			std::cout << DropTime;

			DropPos += DisInM;//remove distance travelled 
			

			

		}
		//glutSwapBuffers();

		

	}
	glutPostRedisplay();
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
			glutSwapBuffers();
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
		DrawString(-0.3, 0.8, 0.0, "Drop Motion Simulator");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "Enter the height of the drop:");
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

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(-50, 50, -50, 50);//
			glMatrixMode(GL_MODELVIEW);
			DropPos = values[2] - 50 - 2.5;//Initialising where you drop or drop height.
			//glutTimerFunc(0, animater, 0);
			TotalDropTime = DropTCalc(values[2]);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glutTimerFunc(1000/60, animater, 0);//call animater once every 1000/60th of a milli second. (60fps is the refresh atm)
		glLoadIdentity();
		gluOrtho2D(-50, 50, -50, 50);
		//I am changing the projection to 100 total span. we can make it bigger but This looks fine so far.
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(15,DropPos,0);//we basically move our camera by the distance specified here.
		
		/*We need to do this because we can only draw a sphere at the origin. So, I change where the origin is, 
		instead of moving object. 60 looks smooth and because we have a black~ish background, no dropped frames 
		that I can make out. */
		
		

		glutSolidSphere(2.5, 100, 100);//draws a solid sphere of radius 2.5. This is thus far, the most computationally intensive part from what I could gather. I don't have a GPU I can loan this too nor am I using shader. So this will be it.
		
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

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowPosition(0,0);//initial position in pixels This is optional and not specifying it will mean window is at random location.
	glutInitWindowSize(1000,1000);//size of the window


	glutCreateWindow("lel");
	glutDisplayFunc(disp);
	glutReshapeFunc(reshape);
	//glutTimerFunc(0, animater, 0);

	glutKeyboardFunc(KeyProc);


	glutMainLoop();


}

double DropTCalc(float dis)
{
	double temp = (2 * dis) / 9.8;
	double tm = sqrt(temp);
	return tm;
}