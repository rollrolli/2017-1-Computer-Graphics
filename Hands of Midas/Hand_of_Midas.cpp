#include <iostream>
#include <fstream>
#include <chrono>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "Matrix.h"
#include "Object.h"
#include "WaveFrontOBJ.h"
#include "Shader.h"
#include "textfile.h"

#define MAPSIZE 200
#define M_PI 3.14159265358979323846

// 'cameras' stores infomation of 5 cameras.
float cameras[5][9] =
{
	{28, 18, 28, 0, 2, 0, 0, 1, 0},
	{28, 18, -28, 0, 2, 0, 0, 1, 0},
	{-28, 18, 28, 0, 2, 0, 0, 1, 0},
	{30, 6, 0, 0, 6, 0, 0, 1, 0},
	{0, 100, 0, 0, 0, 0, 1, 0, 0}
};
int cameraCount = sizeof(cameras) / sizeof(cameras[0]);
int cameraIndex;

std::chrono::duration <double> dt;
std::chrono::system_clock::time_point t;

float fov_distance = 30.0;
float fov_angle = 0.0;
int hand_height = 0;

vector<Matrix> wld2cam, cam2wld;


Objects model_list;


unsigned floorTexID;

int frame = 0;
int width, height;

Shader cowShader;


GLhandleARB v, f, f2, p;
float lpos[4] = { 1,0.5,1,0 };

void InitCamera () {

	
	for (int i = 0; i < cameraCount; i++){

		float * c = cameras[i];

		wld2cam.push_back(Matrix());
		glPushMatrix();

		glLoadIdentity();
		gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);
		glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam[i].matrix());
		cam2wld.push_back(wld2cam[i].inverse());

		glPopMatrix();
	}

	cameraIndex = 0;
}

void drawCamera () {

	glLoadMatrixf(wld2cam[cameraIndex].matrix());

	for (int i = 0; i < (int)wld2cam.size(); i++){

		if (i != cameraIndex){

			glPushMatrix();													
			glMultMatrixf(cam2wld[i].matrix());
			glPopMatrix();
		}
	}
}


void printInfoLog(GLhandleARB obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
		&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void initializeObject () {

		model_list.addObject("cow.obj");				// idx : 0
		model_list.addObject("bunny.obj");				// idx : 1
		model_list.addObject("hand2.obj");				// idx : 2
		model_list.addObject("ironman.obj");			// idx : 3

		model_list.addTexture("ironman_d.tga", 3);
		model_list.addTexture("test.tga", -1);

		model_list.initObjects();

		model_list.set2dPosObject(-20, 10, 0);
		model_list.set2dPosObject(30, -20, 1);
		model_list.set2dPosObject(fov_distance * cosf(fov_angle) + 20 * cosf(fov_angle + 10), fov_distance * sinf(fov_angle) + 20 * sinf(fov_angle + 10), 2);

		model_list.rotateObject(-90, 0, 1, 0, 2);
}

void drawObjects () {

		model_list.drawObjects();
}

void drawBackground () {

	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 0.1);
	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(MAPSIZE, MAPSIZE, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(MAPSIZE, MAPSIZE, MAPSIZE);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, MAPSIZE, MAPSIZE);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, MAPSIZE, -MAPSIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(MAPSIZE, 0, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(MAPSIZE, 0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, 0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, 0, -MAPSIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, MAPSIZE, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, MAPSIZE, MAPSIZE);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, -0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, -0, -MAPSIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(MAPSIZE, MAPSIZE, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(MAPSIZE, -0, -MAPSIZE);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(MAPSIZE, -0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(MAPSIZE, MAPSIZE, MAPSIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(MAPSIZE, -0, -MAPSIZE);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(MAPSIZE, MAPSIZE, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, MAPSIZE, -MAPSIZE);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, -0, -MAPSIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, model_list.textures[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(MAPSIZE, MAPSIZE, MAPSIZE);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(MAPSIZE, -0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(-MAPSIZE, -0, MAPSIZE);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(-MAPSIZE, MAPSIZE, MAPSIZE);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawFloor () {

	if (frame == 0){
		// Initialization part.
		// After making checker-patterned texture, use this repetitively.

		// Insert color into checker[] according to checker pattern.
		const int size = 8;
		unsigned char checker[size*size * 3];

		for (int i = 0; i < size*size; i++){

			if (((i / size) ^ i) & 1){

				checker[3 * i + 0] = 100;
				checker[3 * i + 1] = 100;
				checker[3 * i + 2] = 100;
			}
			else{

				checker[3 * i + 0] = 200;
				checker[3 * i + 1] = 200;
				checker[3 * i + 2] = 200;
			}
		}

		// Make texture which is accessible through floorTexID. 
		glGenTextures(1, &floorTexID);
		glBindTexture(GL_TEXTURE_2D, floorTexID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
	}

	glDisable(GL_LIGHTING);

	glColor3d(0.35, 0.2, 0.1);

	// Draw background rectangle.
	glBegin(GL_POLYGON);
	glVertex3f(-2000, -0.2, -2000);
	glVertex3f(-2000, -0.2, 2000);
	glVertex3f(2000, -0.2, 2000);
	glVertex3f(2000, -0.2, -2000);
	glEnd();


	// Set color of the floor.

	// Assign checker-patterned texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, floorTexID);


	// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(-12, -0.1, -12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
	glTexCoord2d(0, 1);
	glVertex3d(-12, -0.1, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
	glTexCoord2d(1, 1);
	glVertex3d(12, -0.1, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
	glTexCoord2d(1, 0);
	glVertex3d(12, -0.1, -12);		// Texture's (1,0) is bound to (12,-0.1,-12).
	glEnd();

	glDisable(GL_TEXTURE_2D);

}


void display () {

	float * c = cameras[cameraIndex];


	glClearColor(0, 0.6, 0.8, 1);								// Clear color setting


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear the screen
	glShadeModel(GL_SMOOTH);

	drawCamera();													// and draw all of them.
	drawFloor();													// Draw floor.
	//drawBackground();



	// 카메라 부분
	wld2cam.push_back(Matrix());
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);
	glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam[cameraIndex].matrix());
	cam2wld.push_back(wld2cam[cameraIndex].inverse());
	glPopMatrix();
	// 카메라 부분

	model_list.drawObjects();

	glFlush();

	// If it is not backbuffer mode, swap the screen. In backbuffer mode, this is not necessary because it is not presented on screen.

	glutSwapBuffers();


	frame++;
	dt = std::chrono::system_clock::now() - t;
	t = std::chrono::system_clock::now();
}

void reshape (int w, int h) {

		width = w;
		height = h;
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float aspect = width / float(height);
		gluPerspective(45, aspect, 1, 1024);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
}

void initialize () {

		// Set up OpenGL state
		glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
		// Initialize the matrix stacks
		reshape(width, height);
		// Define lighting for the scene
		float light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
		float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
		float light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glEnable(GL_LIGHT0);

		InitCamera();
	
		initializeObject();
		t = std::chrono::system_clock::now();
}

void onKeyPress (unsigned char key, int x, int y) {

		switch (key){

			case '1':

				cameraIndex = 0;

				break;

			case '2':

				cameraIndex = 1;

				break;

			case '3':

				cameraIndex = 2;

				break;

			case '4':

				cameraIndex = 3;

				break;

			case '5':

				cameraIndex = 4;

				break;

			case 'w':

				if (fov_distance > 25 && !model_list.isColideNaive(2)) {

					fov_distance -= 0.5 * dt.count() * 10;
					cameras[3][0] = fov_distance * cosf(fov_angle);
					cameras[3][2] = fov_distance * sinf(fov_angle);

					model_list.set2dPosObject(fov_distance * cosf(fov_angle) + 20 * cosf(fov_angle + 10), fov_distance * sinf(fov_angle) + 20 * sinf(fov_angle + 10), 2);
					model_list.rotateObject(-90, 0, 1, 0, 2);
					model_list.moveObject(0, hand_height, 0, 2);

					glPushMatrix();
					glLoadIdentity();
					glMultMatrixf(model_list.obj2wld_list[2].matrix());

					glRotatef(-57.3 * fov_angle, 0, 1, 0);

					glGetFloatv(GL_MODELVIEW_MATRIX, model_list.obj2wld_list[2].matrix());
					glPopMatrix();
				}

				break;

			case 'a':

				if (model_list.isColideNaive(2) != 1 && model_list.isColideNaive(2) != 2) {

					fov_angle += 0.05 * dt.count() * 10;
					cameras[3][0] = fov_distance * cosf(fov_angle);
					cameras[3][2] = fov_distance * sinf(fov_angle);

					model_list.set2dPosObject(fov_distance * cosf(fov_angle) + 20 * cosf(fov_angle + 10), fov_distance * sinf(fov_angle) + 20 * sinf(fov_angle + 10), 2);
					model_list.rotateObject(-90, 0, 1, 0, 2);
					model_list.moveObject(0, hand_height, 0, 2);

					glPushMatrix();
					glLoadIdentity();
					glMultMatrixf(model_list.obj2wld_list[2].matrix());

					glRotatef(-57.3 * fov_angle, 0, 1, 0);

					glGetFloatv(GL_MODELVIEW_MATRIX, model_list.obj2wld_list[2].matrix());
					glPopMatrix();
				}

				break;

			case 's':

				if (fov_distance < MAPSIZE && !model_list.isColideNaive(2)) {

					fov_distance += 0.5 * dt.count() * 10;
					cameras[3][0] = fov_distance * cosf(fov_angle);
					cameras[3][2] = fov_distance * sinf(fov_angle);

					model_list.set2dPosObject(fov_distance * cosf(fov_angle) + 20 * cosf(fov_angle + 10), fov_distance * sinf(fov_angle) + 20 * sinf(fov_angle + 10), 2);
					model_list.rotateObject(-90, 0, 1, 0, 2);
					model_list.moveObject(0, hand_height, 0, 2);

					glPushMatrix();
					glLoadIdentity();
					glMultMatrixf(model_list.obj2wld_list[2].matrix());

					glRotatef(-57.3 * fov_angle, 0, 1, 0);

					glGetFloatv(GL_MODELVIEW_MATRIX, model_list.obj2wld_list[2].matrix());
					glPopMatrix();
				}

				break;

			case 'd':

				if (model_list.isColideNaive(2) != 3 && model_list.isColideNaive(2) != 4) {

					fov_angle -= 0.05 * dt.count() * 10;
					cameras[3][0] = fov_distance * cosf(fov_angle);
					cameras[3][2] = fov_distance * sinf(fov_angle);

					model_list.set2dPosObject(fov_distance * cosf(fov_angle) + 20 * cosf(fov_angle + 10), fov_distance * sinf(fov_angle) + 20 * sinf(fov_angle + 10), 2);
					model_list.rotateObject(-90, 0, 1, 0, 2);
					model_list.moveObject(0, hand_height, 0, 2);

					glPushMatrix();
					glLoadIdentity();
					glMultMatrixf(model_list.obj2wld_list[2].matrix());

					glRotatef(-57.3 * fov_angle, 0, 1, 0);

					glGetFloatv(GL_MODELVIEW_MATRIX, model_list.obj2wld_list[2].matrix());
					glPopMatrix();
				}

				break;

			case '=':
			case '+':

				if (hand_height < 8){

					model_list.moveObject(0, 1, 0, 2);
					hand_height++;
				}
				
				break;

			case '-':
			case '_':

				if (hand_height > 0){

					model_list.moveObject(0, -1, 0, 2);
					hand_height--;
				}

				break;

			default:

				break;
		}

		glutPostRedisplay();
}

void idle () {
	
		model_list.drawObjects();
		glutPostRedisplay();
}

void setShaders() {

	char *vs = NULL, *fs = NULL, *fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);


	vs = textFileRead("toon.vert");
	fs = textFileRead("toon3.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printInfoLog(v);
	printInfoLog(f);
	printInfoLog(f2);

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);
	printInfoLog(p);

	glUseProgram(p);

	GLint loc1, loc2, loc3, loc4;
	float sc[4] = { 0.8,0.8,0.8,1.0 };




	loc2 = glGetUniformLocation(p, "specColor");
	glUniform4fvARB(loc2, 1, sc);
	glUniform4f(loc2, sc[0], sc[1], sc[2], sc[3]);


}

void main(int argc, char* argv[]){

	width = 1280;
	height = 720;
	frame = 0;

	glutInit(&argc, argv);							// Initialize openGL.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	// Initialize display mode. This project will use float buffer and RGB color.
	glutInitWindowSize(width, height);				// Initialize window size.
	glutInitWindowPosition(100, 100);				// Initialize window coordinate.
	glutCreateWindow("Hand of Midas");

	glutDisplayFunc(display);						// Register display function to call that when drawing screen event is needed.
	glutReshapeFunc(reshape);						// Register reshape function to call that when size of the window is changed.
	glutKeyboardFunc(onKeyPress);					// Register onKeyPress function to call that when user presses the keyboard.

	glutIdleFunc(idle);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//	glEnable(GL_CULL_FACE);

	glewInit();
	if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

	setShaders();


	initialize();									// Initialize the other thing.
	glutMainLoop();									// Execute the loop which handles events.
}