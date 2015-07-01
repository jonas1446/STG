#include <windows.h>
#define GLEW_STATIC
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/audiere.h"


// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
    using namespace glm;

#include "common/shader.cpp"
#include "common/texture.cpp"
#include "common/objloader.cpp"

#include "planeTexture.c"
#include "skin.c"
#include "brickTexture.c"
#include "trackTexture.c"
#include "newTrackTexture.c"
#include "sandTexture.c"
#include "minekartTexture.c"
#include "carTexture.c"
#include "car2Texture.c"
#include "greenCheckPoint.c"
#include "greyCheckPoint.c"


#define pi 3.14159265
int w, h;
const int font=(int)GLUT_BITMAP_9_BY_15;
char s[30];
double t;
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    w = width;
    h = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);     glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
void setOrthographicProjection() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
       gluOrtho2D(0, w, 0, h);
    glScalef(1, -1, 1);
    glTranslatef(0, -h, 0);
    glMatrixMode(GL_MODELVIEW);
}
void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}
static void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1.0, 0.0, 0.0);
    setOrthographicProjection();
    glPushMatrix();
    glLoadIdentity();
    renderBitmapString(200,200,(void *)font,"Font Rendering - Programming Techniques");
    renderBitmapString(300,220, (void*)font, s);
    renderBitmapString(300,240,(void *)font,"Esc - Quit");
    glPopMatrix();
    resetPerspectiveProjection();
    glutSwapBuffers();
}
void update(int value){
    t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    int time = (int)t;
    sprintf(s, "TIME : %2d Sec", time);
    glutTimerFunc(1000, update, 0);
    glutPostRedisplay();
}
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);     glutCreateWindow("Font Rendering Using Bitmap Font - Programming Techniques0");     glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutTimerFunc(25, update, 0);     glutMainLoop();
    return EXIT_SUCCESS;
}
