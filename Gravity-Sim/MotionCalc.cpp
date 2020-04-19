/*YO GMan read This. Building this solution and running it should give you like a yellow screen thingy. That's about it. This makes sure that everything is fine and working on your end*/


#include<GL/glew.h>
#include<glut/glut.h>
#include<gl/GLU.h>//this is apparantly built in. Even glew was built in. We techically only needed to set the preprocessor thingy for it. GL is our include and gl is the built-in shit from Visual Studio.

void disp()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 0.0, 1.0);//change the background
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);//initialising glut
	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowPosition(200, 200);//initial position in pixels This is optional and not specifying it will mean window is at random location.
	glutInitWindowPosition(640, 480);//size of the window


	glutCreateWindow("lel");
	glutDisplayFunc(disp);
	glutMainLoop();


}