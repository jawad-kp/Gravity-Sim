
#include<GL/glew.h>
#include<glut/glut.h>
#include<gl/GLU.h>//this is apparantly built in. Even glew was built in. We techically only needed to set the preprocessor thingy for it. GL is our include and gl is the built-in shit from Visual Studio.
#include<iostream>
#include<string>
#include<math.h>

/*
Equations of Motion to implement:
	Total Time of Flight:
		t = (2*u*sin(theta))/g

	At a time interval t

	x = u*t*cos(theta)

	y = u*t*sin(theta) - 0.5*(g*t*t)

*/

void* Currentfont; //saves the font as a void pointer

void ResetValues();//our reset function to update and modify our values for continual use.


void SetFont(void* font)
{
	Currentfont = font;
}//allows you to reset the font

void DrawString(double x, double y, double z, const char* string)
{
	const char* c;
	glRasterPos3d(x, y, z);

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
	UNKNOWN = -1, START_SCREEN = 3, MENU = 4, PRJMTN_INP_INIT_VELOCITY = 0, PRJMTN_INP_THETA = 1,
	PRJMTN_DISP = 5, PRJMTN_PLOT = 6, DROP_INP_HT = 2, DROP_DISP = 7, ABOUT_PAGE = 10

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
double DropPos = 0.0, DropTime = 0.0, DisToMove = 0.0, InitDrop = 0.0;
double TotalDropTime = 0.0;
double DropTCalc(float);
double ToF = 0.0;
double uSinTh = 0.0, uCosTh = 0.0, TimeInAir = 0.0;//u*sin(theta) and u*cos(th) precalculated to optimise for speed.
bool st = false;//incase it's vertically thrown, this helps keep distance at 0
double xProj, yProj;
double TimeOfFlight(float, float);
void DrawGrid();

enum  class PlayModes {
	Play = 0, Pause = 1 , Fin = 2
};

PlayModes PlayState = PlayModes::Play;

double speed = 1; //Controls speed of Projectile

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
		else if (key == 8)
		{
			int len = inp[(int)DispStat].length();
			if (len - 1 <= 0)
			{
				values[(int)DispStat] = 0;
				inp[(int)DispStat] = "";
				glutPostRedisplay();
				return;
			}//if they enter backspace and there's nothing to clear.
			std::cout << len << std::endl;
			inp[(int)DispStat] = inp[(int)DispStat].substr(0, (len - 1));//extract everything but the last charcter here.
			std::cout << inp[(int)DispStat] << std::endl;
			values[(int)DispStat] = std::stof(inp[(int)DispStat]);//save the value so we can do stuff with it. I'll try and flush it into a buffer later on but there's no noticable performance impact as of now.
			glutPostRedisplay();
		}
		else
		{
			inp[(int)DispStat].push_back(key);//appending value to the string here
			values[(int)DispStat] = std::stof(inp[(int)DispStat]); //Converting string to float here
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
			DispStat = AppStat::PRJMTN_INP_INIT_VELOCITY;
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
	else if (DispStat == AppStat::PRJMTN_DISP || DispStat == AppStat::DROP_DISP)
	{
		if ((key == 'p' || key == 'P' || key == ' ' ) && PlayState != PlayModes::Fin)
		{
			PlayState = PlayState == PlayModes::Pause ? PlayModes::Play : PlayModes::Pause; //if the PlayState is Play then Pause otherwise Play. I've typed play so much it feels weird noe
			glutPostRedisplay();
		}
		else if (key == '+')
		{
			speed = 2.0;
			glutPostRedisplay();
		}
		else if(key == '-')
		{
			speed = 1.0;
			glutPostRedisplay();
		}
		else if (key == 'r' || key == 'R')
		{
			ResetValues();
			glutPostRedisplay();
			return;
		}
		else if (key == 's' || key == 'S')
		{
			ResetValues();
			if (DispStat == AppStat::PRJMTN_DISP)
			{
				DispStat = AppStat::PRJMTN_PLOT;
				ResetValues();
				glutPostRedisplay();
			}
			else if (DispStat == AppStat::DROP_DISP)
			{
				DispStat = AppStat::MENU;
				glutPostRedisplay();
			}
			//move to next page for final values, if not leave it here idk TBD That's why else if and not just an else
		}

	}
}

void plotTrajectory(int);

void animater(int a)
{
	
	if (a == (int)PlayModes::Fin || a == (int) PlayModes::Pause)
	{
		glutPostRedisplay();
		return;
	}
	if (DispStat == AppStat::DROP_DISP)
	{
		if (DropTime <= TotalDropTime)
		{
			/* h = ut + 0.5gt^2 , Since we're dropping it u = 0 we're left with 0.5gt^2 */
			
			DisToMove = -4.9 * DropTime * DropTime;
			DropPos = InitDrop + DisToMove;
			DropTime += 0.016666667;
		}
		else
		{
			PlayState = PlayModes::Fin;
			std::cout<<"\n Time spent in air = \n ";
			std::cout << DropTime;
			std::cout << "\n Speed =  \n ";
			std::cout << speed;
			
		}
	}
	else if (DispStat == AppStat::PRJMTN_DISP)
	{
		if (TimeInAir < ToF)
		{
			/*
				At a time interval t

					x = u*t*cos(theta)

					y = u*t*sin(theta) - 0.5*(g*t*t)
			*/
			xProj = (uCosTh * TimeInAir) - 87.7;//87.7 is leftmost co-ordinate
			yProj = uSinTh * TimeInAir - 4.9 * TimeInAir * TimeInAir;
			TimeInAir += 0.016666667 * speed; //multiplied by how fast you want the sphere to move here.
		}
		else
		{
			PlayState = PlayModes::Fin;
		}
	}
	glutPostRedisplay();
}


void disp()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	if (DispStat == AppStat::START_SCREEN)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Motion and Garvity Simulation");

		DrawString(0.15, 0.7, 0.0, " -By Jawad and Gaurav");

		DrawString(-0.3, 0.0, 0.0, "Press X to Begin");
		DrawString(-0.3, -0.1, 0.0, "Press Q to Quit");
		//std::cout << (int)DispStat << std::endl;

		glFlush();
	}

	else if (DispStat == AppStat::MENU)
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);//Redrawing matrix because coming back from elsewhere messes it up otherwise
		glMatrixMode(GL_MODELVIEW);
		
		ResetValues();

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

	else if (DispStat == AppStat::PRJMTN_INP_INIT_VELOCITY)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Screen");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "Initial Velocity (m/s):");
			char buf[20];
			snprintf(buf, sizeof(buf), "%f", values[0]); //converts a float into a character array so we can display it
			DrawString(-0.3, 0.3, 0.0, buf);
		}
		else
		{
			DispStat = AppStat::PRJMTN_INP_THETA;
			TakeInput = true;
			glutSwapBuffers();
			glutPostRedisplay();

		}
		glFlush();
	}

	else if (DispStat == AppStat::PRJMTN_INP_THETA)
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SetFont(GLUT_BITMAP_HELVETICA_18);
		glColor3f(0.0, 0.0, 0.0);
		DrawString(-0.3, 0.8, 0.0, "Projectile Screen for the second value:");
		if (TakeInput)
		{
			DrawString(-0.3, 0.5, 0.0, "Projectile Angle:");
			char buf[20];
			snprintf(buf, sizeof(buf), "%f", values[1]);
			DrawString(-0.3, 0.3, 0.0, buf);

		}
		else
		{
			DispStat = AppStat::PRJMTN_DISP;


			if (values[1] == 90)
			{
				st = true;
			}
			values[1] = (4.0 * std::atan2(1.0, 1.0)) * values[1] / 180.0;//deg2Radians
			ToF = TimeOfFlight(values[0], values[1]);
			/*std::cout << "The Time of Flight is:\n";*/

			std::cout << values[1] << std::endl;
			uSinTh = values[0] * sin(values[1]);
			uCosTh = values[0] * cos(values[1]); //Saving u*sin(theta) and u*cos(theta) so we won't waste compute time with calls

			glutPostRedisplay();

		}
		glFlush();
	}

	else if (DispStat == AppStat::PRJMTN_DISP)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//gluOrtho2D(-100, 100, -100, 100);
		glOrtho(-100, 100, -100, 100, -100, 100);
		//I am changing the projection to 200 total span. we can make it bigger but This looks fine so far.
		glMatrixMode(GL_MODELVIEW);
		DrawGrid();
		glColor3f(0, 1, 1);
		glutTimerFunc(1000/60, animater, int(PlayState));

		//Displaying Movement Options
		DrawString(-90, -20, 0, "P  or Space -> Play/Pause");
		DrawString(-90, -25, 0, "R -> Restart");
		DrawString(-90, -30, 0, "S -> Plot Trajectory");
		DrawString(-90, -35, 0, "Q -> Quit");
		DrawString(-90, -40, 0, "+ -> Increase Speed");
		DrawString(-90, -45, 0, "- -> Decrease/Normalise Speed");

		//Displaying Current Values
		DrawString(40, -20, 0, "Height: ");
		char bufHt[20];
		double ht;
		if (PlayState == PlayModes::Fin)
			ht = 0.0;
		else
			ht = yProj;
		//It fixes the height issue where it's slightly higher on the bottom because of the last frame that gets skipped or something, I am not sure. It looks better this way.

		snprintf(bufHt, sizeof(bufHt), "%f", ht); //converts a float into a character array so we can display it
		DrawString(60, -20, 0, bufHt);

		char bufDis[20];
		DrawString(40, -25, 0, "Distance: ");
		if (st)
		{
			//snprintf(bufDis, sizeof(bufDis), "%f", 0.0000);
			DrawString(70, -25, 0, "0.0000000");
		}
			
		else
		{
			snprintf(bufDis, sizeof(bufDis), "%f", (xProj + 87.7)); //adding because the projectile is in the third quadrat
			DrawString(70, -25, 0, bufDis);
		}

		
		

		char bufTime[20];
		snprintf(bufTime, sizeof(bufTime), "%f", TimeInAir);
		DrawString(40, -30, 0, "Time in Air: ");
		DrawString(70, -30, 0, bufTime);



		glTranslated(xProj, yProj, 0);
		glutSolidSphere(2.5, 100, 100);//Start form 0 and travels exactly to the grid at the moment. Math is correct we need to figure out how to modify mapping and then this will be done.


		glFlush();
	}

	else if (DispStat == AppStat::PRJMTN_PLOT)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-100, 100, -100, 100, -100, 100);
		glMatrixMode(GL_MODELVIEW);
		glutTimerFunc(1000 / 60, plotTrajectory, 0);//Can't flush from the function, so it instead updates values and we display them here.
		//double xProj, yProj;
		for (double i = 0, xProj = 0, yProj = 0; i <= TimeInAir; i += 0.0166667)
		{
			glPointSize(2);
			xProj = uCosTh * i - 87.7;
			yProj = uSinTh * i - 4.9 * i * i;
			glBegin(GL_POINTS);

			//TimeInAir += 0.016666667;
			glVertex2d(xProj, yProj);
			glEnd();
			//glFlush();
		}

		DrawGrid();
		glColor3f(0, 1, 1);//resets colour after you draw the grid

	}

	else if (DispStat == AppStat::DROP_INP_HT)
	{
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
			std::cout << "\nThe value for height entered is:\n";
			std::cout << values[2];
			DropPos = values[2];//Initialising where you drop or drop height.
			InitDrop = values[2];
			//glutTimerFunc(0, animater, 0);
			TotalDropTime = DropTCalc(values[2]);
			//DropTime = TotalDropTime;
			DropTime = 0;
			std::cout << "\nDrop Time is:\n";
			std::cout << TotalDropTime;
			glutPostRedisplay();

		}
		glFlush();

	}

	else if (DispStat == AppStat::DROP_DISP)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glutTimerFunc(1000 / 60, animater, int(PlayState));//call animater once every 1000/60th of a milli second. (60fps is the refresh atm)
		glLoadIdentity();
		glOrtho(-100, 100, -100, 100, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		DrawGrid();
		glColor3f(0, 1, 1);
		//Displaying Movement Options
		DrawString(-90, -20, 0, "P or Space -> Play/Pause");
		DrawString(-90, -25, 0, "R -> Restart");
		DrawString(-90, -30, 0, "S -> Main Menu");//we can add a plot or a stats page here if we wanted to
		DrawString(-90, -35, 0, "Q -> Quit");
		DrawString(-90, -40, 0, "+ -> Increase Speed");
		DrawString(-90, -45, 0, "- -> Decrease/Normalise Speed");


		//Displaying Current Values
		DrawString(40, -20, 0, "Height: ");
		char bufHt[20];
		double ht;
		if (PlayState == PlayModes::Fin)
			ht = 0.0;
		else
			ht = DropPos;
		snprintf(bufHt, sizeof(bufHt), "%f", ht); //converts a float into a character array so we can display it
		DrawString(60, -20, 0, bufHt);

		DrawString(40, -30, 0, "Time: ");
		char bufTime[20];
		snprintf(bufTime, sizeof(bufTime), "%f", DropTime);
		DrawString(60, -30, 0, bufTime);
		
		glTranslatef(15, DropPos, 0);//we basically move our camera by the distance specified here





		glutSolidSphere(2.5, 100, 100);//draws a solid sphere of radius 2.5. This is thus far, the most computationally intensive part from what I could gather. I don't have a GPU I can loan this too nor am I using shader. So this will be it.

		glFlush();
	}

	else if (DispStat == AppStat::ABOUT_PAGE)
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-100, 100, -100, 100, -100, 100);
		glMatrixMode(GL_MODELVIEW);

		SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
		glColor3f(0, 0, 0);
		DrawString(-20, 80, 0, "About This Project:");

		SetFont(GLUT_BITMAP_HELVETICA_18);

		/* Projectile Part */
		glColor3f(0,0,0);
		DrawString(-70, 50, 0, "Projectile:");
		
		//SetFont(GLUT_BITMAP_HELVETICA_12);
		DrawString(-45, 50, 0, "This is a simulation of an object's motion in two dimensions which is under constant acceleration");
		DrawString(- 67, 45,0, "due to gravity. Our project presents an approach at mimicking how a sphere thrown from a certain height");
		DrawString(-67, 40,0,"would travel through space.");


		DrawString(-67, 30, 0, "The simulation achieves accuracy in capturing the trajectory of motion of the projectile using");
		DrawString(-67,25,0,"trigonometry and calculus. It also shows us the maximum distance and height the object thrown can");
		DrawString(-67,20,0,"achieve given an initial velocity and launch angle.");


		DrawString(-67,10,0, "Since most games and war simulations involve the propelling of an object to achieve a certain");
		DrawString(-67,5,0,"distance accurately, our simulation has its share of applications.");

		/*Drop Part*/
		glColor3f(0, 0.0, 0);
		DrawString(-70,-15,0, "Drop:");

		DrawString(-53.25, -15, 0, "How would an object dropped from a certain height fall? How much time would it take to reach the");
		DrawString(-67,-20,0,"ground? How high would it bounce back up? The drop simulation takes a crack at answering these");
		DrawString(-67,-25,0,"questions by replicating real-world conditions for an object being dropped.");

		SetFont(GLUT_BITMAP_HELVETICA_12);
		glColor3f(0, 0, 0);
		DrawString(60, -50,0, "-");
		glColor3f(0, 0, 0);
		DrawString(63, -50.33, 0, "A Project by Gaurav and Jawad");
		


		glFlush();
	}

	glutSwapBuffers();

}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);//initialising glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowPosition(10, 10);//initial position in pixels This is optional and not specifying it will mean window is at random location.
	glutInitWindowSize(1000,1000);//size of the window

	glutCreateWindow("MAIN WINDOW");
	glutDisplayFunc(disp);
	glutReshapeFunc(reshape);
	
	DispStat = AppStat::START_SCREEN;


	glutKeyboardFunc(KeyProc);

	glutMainLoop();
	return 0;


}

double DropTCalc(float dis)
{
	double temp = (2 * dis) / 9.8;
	double tm = sqrt(temp);
	return tm;
}

double TimeOfFlight(float velo, float ang)
{
	//	t = (2*u*sin(theta))/g
	double t = 2 * velo * sin(ang) / 9.8;
	return t;
}

void ResetValues()
{
	//modify to display full value set later on, maybe pass an enum and evaluate or use PolyMorphism.
	values[0] = 0;
	values[1] = 0;
	values[2] = 0;
	inp[0] = "";
	inp[1] = "";
	inp[2] = "";
	xProj = -87.7;//decent start? It looks right here.
	yProj = 0.0;
	TimeInAir = 0.0;
	DropTime = 0;
	PlayState = PlayModes::Play;
	speed = 1.0;

}

void plotTrajectory(int)
{
	if(TimeInAir < ToF)
		TimeInAir += 2*0.016666667;

	glutPostRedisplay();
}

void DrawGrid()
{
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		//X-Axis (ground)
		glVertex2f(-90, -2.6);//screen left
		glVertex2f(90, -2.6);//screen right

		//Y-Axis
		glVertex2f(-90, -2.6);//screen bottom
		glVertex2f(-90, 100);//screen top

		//Markings on X-Axis
		glColor3f(0, 1, 0);
		glEnd();
		for (float Xmark = -80, Ymark = 7.4; Xmark < 90; Xmark += 10, Ymark += 10)
		{
			glBegin(GL_LINES);
				//X-axis Marking
				glVertex2f(Xmark, -3.6);//bottom
				glVertex2f(Xmark, -1.6);//top

				// Y-Axis Marking
				glVertex2f(-91, Ymark);//left
				glVertex2f(-89, Ymark);//right
			glEnd();

		}

}