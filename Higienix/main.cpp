//
//  Graficas Computacionales
//  Proyecto Semestral
//
//  Higienix
//  Eliezer Galvan  A01190876
//  Iliana Garcia   A01191428
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <cstring>

#define TRUE 1
#define FALSE 0

//Specify if debug logs should be printed
#define DEBUGGING FALSE

using namespace std;

int	screenWidth = 700, screenHeight = 700;
GLdouble xRaster, yRaster; //Text rasters

bool runningFlag = false;
int state = 0;
int textSize = 1;

typedef enum {notRunning, running, playerWon, playerLost} states;
states game_state = notRunning;

typedef enum {menu, instructions} screens;
screens game_screen = menu;

//Despliega texto en la ventana gráfica
void draw3dString (void *font, char *s, float x, float y, float z)
{
    unsigned int i;
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    if (textSize == 1)
        glScaled(0.2, 0.2, 0.01);
    
    else if (textSize == 2)
        glScaled(0.25, 0.25, 0.25);
    
    else if (textSize == 3)
        glScaled(0.3, 0.3, 0.2);
    
    else if (textSize == 4)
        glScaled(0.4, 0.4, 0.1);
    
    for (i = 0; i < s[i] != '\0'; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, s[i]);
    }
    
    glPopMatrix();
}

void timer(int value)
{
    
}

void drawMenuScreen()
{
    glColor3ub(0, 0, 0);
    
    char title1[] = "Las Aventuras de";
    textSize = 3;
    xRaster = 175;
    yRaster = screenHeight-100;
    draw3dString(GLUT_STROKE_MONO_ROMAN, title1, xRaster, yRaster, 0);
    
    char title2[] = "Higienix";
    textSize = 4;
    xRaster = 250;
    yRaster = screenHeight-155;
    draw3dString(GLUT_STROKE_MONO_ROMAN, title2, xRaster, yRaster, 0);
    
    char jugar[] = "Jugar";
    char niveles[] = "Niveles";
    char instr[] = "Instrucciones";
    
    
    textSize = 2;
    
    xRaster = 300;
    yRaster = 400;
    draw3dString(GLUT_STROKE_MONO_ROMAN, jugar, xRaster, yRaster, 0);
    
    xRaster = 290;
    yRaster = 300;
    draw3dString(GLUT_STROKE_MONO_ROMAN, niveles, xRaster, yRaster, 0);
    
    xRaster = 250;
    yRaster = 200;
    draw3dString(GLUT_STROKE_MONO_ROMAN, instr, xRaster, yRaster, 0);
    
    textSize = 1;
    char salir[] = "salir-esc";
    xRaster = 5;
    yRaster = 10;
    draw3dString(GLUT_STROKE_MONO_ROMAN, salir, xRaster, yRaster, 0);
}

void drawGameInstructions()
{
    glColor3ub(0, 0, 0);
    textSize = 1;
    xRaster = 150;
    yRaster = 10;
    char creditos[] = "Eliezer Galvan - Iliana Garcia";
    draw3dString(GLUT_STROKE_MONO_ROMAN, creditos, xRaster, yRaster, 0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (game_screen == menu) {
        drawMenuScreen();
    }
    else if (game_screen == instructions) {
        drawGameInstructions();
    }
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int mouseX, int mouseY)
{
    switch (key){
        case 'j': // jugar
        case 'J':
            break;
        case 'n': // niveles
        case 'N':
            break;
        case 'i': // instrucciones
        case 'I':
            if (game_screen == instructions) {
                game_screen = menu;
            }
            else {
                game_screen = instructions;
            }
            break;
        case 27: //esc - salir
            exit(0); //terminate the program
        default:
            break; //do nothing
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    
}

void init()
{
    glClearColor(220.0/255.0, 245.0/255.0, 255.0/255.0, 1.0); // light blue background color
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(0, screenWidth, 0, screenHeight, 100, 300 ); //izq, der, abajo, arriba, cerca, lejos
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitWindowSize(screenWidth,screenHeight);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - screenWidth) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - screenWidth) / 2);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Higienix");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(5, timer, 1);
    
    glutMainLoop();
    
    return EXIT_SUCCESS;
}
