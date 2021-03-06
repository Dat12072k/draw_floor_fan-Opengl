#include <iostream>
#include <conio.h>
#include <GL/freeglut.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265
#endif


//remote view
int x_mouse_pos, y_mouse_pos;
double rotate_x, rotate_y;
double zoom_z = 0;
// texture image
unsigned int texture;
int t_width = 0, t_height = 0, t_channels = 0;
// fan state
bool isfanon = false;
bool isRotale = false;
GLint fanPart1, fanPart2, fanPart3;
bool isLightMove = false, lightState = true;
// beizer pos to draw fan wing
 GLfloat ctrlpoints[8][3] ={
{4,6,0},
{-1,5,0},
{-4,3,0},
{4,0,0},
{7,-2,0},
{3,3,0},
{0.5,3.5,0},
{4,6,0},
};




void shadowMatrix(GLfloat shadowMat[4][4],GLfloat groundplane[4],GLfloat lightpos[4])
{
  GLfloat dot;

  /* Find dot product between light position vector and ground plane normal.*/
    dot = groundplane[0] * lightpos[0] + groundplane[1] * lightpos[1]
            + groundplane[2] * lightpos[2] + groundplane[3] * lightpos[3];

    shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
    shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
    shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
    shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];

    shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
    shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
    shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
    shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];

    shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
    shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
    shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
    shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];

    shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
    shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
    shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
    shadowMat[3][3] = dot - lightpos[3] * groundplane[3];

}


void drawCylinder(int nmax, int nmin, float height, float radius)
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texture);


    double MAX = height / nmax;
    double MIN = 2.0 * M_PI / nmin;
    int i, j;

    for (i = 0; i < nmax; ++i) {
        GLfloat z0 = 0.5 * height - i * MAX;
        GLfloat z1 = z0 - MAX;

        glBegin(GL_TRIANGLE_STRIP);
        for (j = 0; j <= nmin; ++j) {
            double a = j * MIN;
            GLfloat x = radius * cos(a);
            GLfloat y = radius * sin(a);
            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat) nmin, i / (GLfloat) nmax);
            glVertex3f(x, y, z0);

            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat) nmin, (i + 1) / (GLfloat) nmax);
            glVertex3f(x, y, z1);
            }
            glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

int fan_state = 0;
double powerbtn = 0;
double twobtn = 0;
double onebtn = 0;
double threebtn = 0;

void loadTexture (char* file,float size){
    unsigned char* image = stbi_load(file, &t_width, &t_height, &t_channels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-size/2.0,-size/2.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-size/2.0, size/2.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(size/2.0, size/2.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(size/2.0, -size/2.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void loadCylinderTexture(char* file){
    unsigned char* image = stbi_load(file, &t_width, &t_height, &t_channels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
}

void initLightSorce(){
    glEnable(GL_LIGHTING);
    GLfloat ambient[] = { 1, 1, 1, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    //GLfloat spot_direction[] = { -1.0,0, 0.0 };/*
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    /*glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.5);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.2);

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/
    glEnable(GL_LIGHT0);

    GLfloat light1_ambient [] = {0.2, 0.2, 0.2, 1,0};
    GLfloat light1_diffuse [] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light1_specular [] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light1_position [] = {0,0,10};
    GLfloat spot_direction [3] = {0, 0, 1};

    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);
    /*glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
    glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
    glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);

    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
*/
    glEnable (GL_LIGHT1);
}

void initFog(){
    glEnable(GL_FOG);
    GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
    glFogi (GL_FOG_MODE, GL_EXP);
    glFogfv (GL_FOG_COLOR, fogColor);
    glFogf (GL_FOG_DENSITY, 0.05);
    glHint (GL_FOG_HINT, GL_DONT_CARE);
    glFogf (GL_FOG_START, 1.0);
    glFogf (GL_FOG_END, 5.0);
}

void initMaterial(GLfloat amdiff[], GLfloat spec[]){
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, amdiff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
}




void canh(){

    glRotated(-12,1,1,1);
    glTranslated(-4.5,-6,0);
    glColor3f(1, 1, 1);

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 60; i++)
        glEvalCoord1d(i/60.0);
    glEnd();

}


void dequat(){
    GLfloat amdiff[] = {0.25,0.25,0.28,0.7};
    GLfloat spec[] = {0.2,0.2,0.2};
    initMaterial(amdiff,spec);

    glTranslated(0,-6,0);
    glRotated(180,0,1,0);

    glColor3d(0.13,0.11,0.17);
    glPushMatrix();
    glRotated(90,1,0,0);
    glTranslated(0,0,-1.001);
    loadTexture("D:/OpenGlApp/FloorFan/texture/dequat.png",8);  // config here to show texture
    glPopMatrix();
    glScaled(4,1,4);
    glutSolidCube(2);
    glScaled(0.25,1,0.25);
}


void nutbam(){
    glPushMatrix();
    glScaled(1,1,1.5);
    glColor3d(0.15,0.2,0.57);
    glTranslated(-2,1.3,-2.82);

    glPushMatrix();
    glPushMatrix();
    glPushMatrix();

    //so 3
    glTranslated(0,threebtn,0);
    glutSolidCube(0.75);
    glPopMatrix();
    //so 2
    glTranslated(0,twobtn,0);
    glTranslated(1.25,0,0);
    glutSolidCube(0.75);
    glPopMatrix();
    //so 1
    glTranslated(0,onebtn,0);
    glTranslated(2.5,0,0);
    glutSolidCube(0.75);
    glPopMatrix();
    //bat tat
    GLfloat amdiff[] = {1,0,0};
    GLfloat spec[] = {1,1,1,1};
    initMaterial(amdiff,spec);
    glTranslated(0,powerbtn,0);
    glColor3d(1,0.12,0.12);
    glTranslated(3.75,0,0);
    glutSolidCube(0.75);
    glPopMatrix();
}

void chanquat(){
    glTranslated(0,0,2);
    glPushMatrix();
    loadCylinderTexture("D:/OpenGlApp/FloorFan/texture/chanquat.png");  // config here to show texture
    glTranslated(0,4,0);
    glRotated(-90,1,0,0);
    drawCylinder(24,24,8,0.75);
    glPopMatrix();
    loadCylinderTexture("D:/OpenGlApp/FloorFan/texture/floor.jpg"); // config here to show texture
}


double swing_control = 0.3;

void cantocnang(){

    GLfloat amdiff[] = {0.27,0.2,0.07};
    GLfloat spec[] = {1,1,1,1};
    initMaterial(amdiff,spec);


    glColor3f(0.2,0.7,0.4);
    glPushMatrix();

    glTranslated(0,11.5+swing_control,1);
    glRotated(90,1,0,0);
    glutSolidCylinder(0.2,2,24,24);
    glColor3f(0.7,0.3,0.4);
    glutSolidTorus(0.2,0.2,12,12);
    glPopMatrix();
}

double swing = 0;
bool swing_state = false;
bool prev_swing_state = swing_state;

void dongco(){
    glTranslated(0,9,-2);
    glColor3d(0,1,0);

    GLfloat amdiff[] = {0.45,0.45,0.45,0.7};
    GLfloat spec[] = {1,1,1,1};
    initMaterial(amdiff,spec);

    glPushMatrix();
    glRotated(-90,0,0,1);
    glutSolidCylinder(2,4,24,24);
    glPopMatrix();
    glColor3d(0.18,0.65,0.18);
    glTranslated(0,0,0.35);
    glutSolidTorus(1,1,36,36);

    glTranslated(0,0,3.5);
    glutSolidTorus(2,0,36,36);
    glColor3d(0,0.5,0.5);
    glTranslated(0,0,-5.5);
    glPushMatrix();
    glTranslated(0,0,-0.5);
    glutSolidCylinder(0.35,4,24,24);
    glPopMatrix();

}


void longquat(){

    GLfloat amdiff[] = {0.18,0.18,0.18};
    GLfloat spec[] = {1,1,1,1};
    initMaterial(amdiff,spec);

    glPushMatrix();
    glScaled(1,1,0.5);
    glColor3d(0.8,0.5,0.5);
    glutWireTorus(3,3,9,36);
    glTranslated(5,5,5);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0,0,-1.5);
    glutSolidCone(1.5,0.75,24,24);
    glPopMatrix();

}


int fanspeed = 0;


void canhquat(){
    GLfloat amdiff[] = {0.84,0.75,0.32,0.5};
    GLfloat spec[] = {1,1,1,1};
    initMaterial(amdiff,spec);

    glRotated(fanspeed,0,0,1);
    glPushMatrix();
    glPushMatrix();
    glPushMatrix();
    canh();
    glPopMatrix();
    glRotated(120,0,0,1);
    canh();
    glPopMatrix();
    glRotated(-120,0,0,1);
    canh();
    glPopMatrix();
    glColor3d(1,0.25,0.25);
    glutSolidTorus(0.5,0.5,36,36);
}


void fan(){
    glTranslated(0,7,0);
    glCallList(fanPart1);
    nutbam();
    glRotated(swing,0,1,0);
    cantocnang();
    glCallList(fanPart2);
    glRotated(fanspeed,0,0,1);
    glCallList(fanPart3);
}

void drawFloor(){
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-35,0,-35.0);
    glTexCoord2f(0,10); glVertex3f(35.0,0, -35.0);
    glTexCoord2f(10,10); glVertex3f(35.0,0, 35.0);
    glTexCoord2f(10,0); glVertex3f(-35.0,0, 35.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}




void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1, 500.0);
    gluLookAt(0,30,30,0,7,0,0,1,0);
    glMatrixMode(GL_MODELVIEW);
}

void moveCamera(){
    glTranslated(0,0,zoom_z);
    glRotated (rotate_y,1,0,0);
    glRotated (rotate_x,0,1,0);
}

static GLfloat floorShadow[4][4];
GLfloat lightPosition[4] = {15,15,15,0};
float lightRadius = 15*sqrt(2);
GLfloat floorMatrix[4] = {0,70*70,0,-0};
void drawShadow(){

    shadowMatrix(floorShadow, floorMatrix, lightPosition);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glColor4f(0.0, 0.0, 0.0, 0.9);
    glPushMatrix();
    glTranslated(0,0.001,0);

    glMultMatrixf((GLfloat*) floorShadow);

    fan();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawReflection(){

    glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
	drawFloor();
    glDisable(GL_CULL_FACE);

    glColorMask(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	glScaled(1,-1,1);
	fan();

	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5, 0.5, 0.5, 0.7f);
	drawFloor();
	glDisable(GL_BLEND);
}




void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glLoadIdentity();

    glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);

    moveCamera();
    glPushMatrix();
    fan();
    glPopMatrix();


    // draw light source position
    glPushMatrix();
    glTranslatef(lightPosition[0],lightPosition[1],lightPosition[2]);
    glutSolidSphere(1,24,24);
    glPopMatrix();

    drawReflection();

	drawShadow();

	glutSwapBuffers();
}




void keyboard(unsigned char key, int x, int y){

    switch (key) {
    case 27:
        exit(0);
        break;
    case 't':
        if (fan_state == 0)
            prev_swing_state = !prev_swing_state;
        else {
            swing_state = !swing_state;
            prev_swing_state = swing_state;
        }

        if (prev_swing_state)
            swing_control = 0;
        else
            swing_control = 0.3;
        break;
    case '0':
        powerbtn = -0.5;
        fan_state = 0;
        onebtn = 0;
        twobtn = 0;
        threebtn = 0;
        prev_swing_state = swing_state;
        swing_state = false;
        break;
    case '1':
        fan_state = 1;
        onebtn = -0.5;
        twobtn = 0;
        threebtn = 0;
        swing_state = prev_swing_state;
        break;
    case '2':
        fan_state = 2;
        twobtn = -0.5;
        onebtn = 0;
        threebtn = 0;
        swing_state = prev_swing_state;
        break;
    case '3':
        fan_state = 3;
        threebtn = -0.5;
        onebtn = 0;
        twobtn = 0;
        swing_state = prev_swing_state;
        break;
    case 'l':
        isLightMove = !isLightMove;
    }
}



void mouseFunc(int button, int state, int x, int y){
    switch (button) {
    case GLUT_LEFT_BUTTON:

        if (state == GLUT_DOWN) {
            x_mouse_pos = x;
            y_mouse_pos = y;
            isRotale = true;
        } else
            isRotale = false;
        break;
    }
}

void motionFunc(int x, int y){
    if (isRotale){
        rotate_x += (x - x_mouse_pos) * 0.7;
        rotate_y += (y - y_mouse_pos) * 0.7;
        x_mouse_pos = x;
        y_mouse_pos = y;
    }
}

void mouseWheel(int button, int dir, int x, int y){
    if (dir>0)
        zoom_z += 1;
    else
        zoom_z -= 1;
}


bool state = true;
void timer(int){
    glutPostRedisplay();
    glutTimerFunc(1000/60,timer,0);
    if (swing_state) {
        switch(state){
        case true:
            if (swing < 60)
                swing += 0.5;
            else state = false;
            break;
        case false:
            if (swing >-60)
                swing -= 0.5;
            else state = true;
            break;
        }
    }
    switch (fan_state) {
    case 0:
        if (powerbtn < 0) powerbtn += 0.05;
        break;
    case 1:
        fanspeed = (fanspeed +10) % 360 ;
        break;
    case 2:
        fanspeed = (fanspeed +15) % 360 ;
        break;
    case 3:
        fanspeed = (fanspeed +20) % 360 ;
        break;
    }


    if (isLightMove) {
        if (abs(lightPosition[0]) > lightRadius-lightRadius/100)
        lightState = !lightState;

        if (lightState) {
            lightPosition[0] -= lightRadius/100;
            lightPosition[2] = sqrt(lightRadius*lightRadius - lightPosition[0]*lightPosition[0]);
        } else {
            lightPosition[0] += lightRadius/100;
            lightPosition[2] = -sqrt(lightRadius*lightRadius - lightPosition[0]*lightPosition[0]);
        }
    }
}


void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 8, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    //glDisable(GL_MAP1_VERTEX_3);

    initLightSorce();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);





    initFog();

    fanPart1 = glGenLists (1);
    glNewList (fanPart1, GL_COMPILE);
    dequat();
    chanquat();
    glEndList ();

    fanPart2 = glGenLists (1);
    glNewList (fanPart2, GL_COMPILE);
    dongco();
    longquat();
    glEndList ();

    fanPart3 = glGenLists (1);
    glNewList (fanPart3, GL_COMPILE);
    canhquat();
    glEndList ();
}


int main(int argc, char **argv)
{

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800,450);
    glutInitWindowPosition(20,20);
    glutCreateWindow("Floor Fan");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutKeyboardFunc(keyboard);
    glutMouseWheelFunc(mouseWheel);
    glutTimerFunc(0,timer,0);
    init();
    glutMainLoop();
    return 0;
}
