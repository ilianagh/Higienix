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
#include <math.h>

#include "ImageLoader.hpp"

#define TRUE 1
#define FALSE 0

//Specify if debug logs should be printed
#define DEBUGGING FALSE

//Amount of textures used and texture ids
#define TEX_COUNT 2

#define FLOOR_TEX 0
#define WALL_TEX 1

using namespace std;

/// FLAGS AND VARIABLES ///

/////////////
string fullPath = __FILE__;

static GLuint texName[TEX_COUNT];

const int screenWidth = 800, screenHeight = 700;
const int sMax = 6, mapSize = 15, mm = mapSize+1, direction_parts = 72;
const int move[4][2] = {{-1,0},{0,-1},{1,0},{0,1}};
const int move_key[4] = {GLUT_KEY_UP, GLUT_KEY_LEFT, GLUT_KEY_DOWN, GLUT_KEY_RIGHT };

double camera_zoom = 15;
int current_direction = 0;
int glWin, minPathLength = 0, myPathLength = 0;
char data[mapSize+2][mapSize+2], cp[mapSize+2][mapSize+2];

float lightAmb [] = {0.05, 0.05, 0.05};
float lightDif [] = {0.95, 0.95, 0.95};
float lightPos [] = {(int)mapSize/2,  7,  (int)mapSize/2};

float backgroundColor	[] = {5.0/255.0,62.0/255.0,64.0/255.0};
float floorColor 		[] = {10.0/255.0,123.0/255.0,127.0/255.0};
float boxColor 			[] = {18.0/255.0,222.0/255.0,229.0/255.0};
/////////////

GLdouble xRaster, yRaster; //Text rasters

bool runningFlag = false;
int state = 0;
int textSize = 1;

typedef enum {notPlaying, playing, playerWon, playerLost} states;
states game_state = notPlaying;

typedef enum {MENU, INSTRUCTIONS, PLAY} screens;
int game_screen = MENU;

bool pauseFlag = false;

/// OBJECT MODELS ///

struct Tpos{
    char x,y;
};

struct Player{
    int x,y,z;
    int dx,dz;
    bool isGo;
} player,maze_exit;

struct Maps{
    int x,z;
} map;

/// INTERFCAFE DECLARATION ///

void display(void);
void genMap(void);
void exitGame(bool f=false);

/// IMPLEMENTATION ///

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
        glutStrokeCharacter(font, s[i]);
    }
    
    glPopMatrix();
}

void drawMenuScreen()
{
    glColor3d(0.0, 0.0, 0.0);
    
    char title1[] = "Las Aventuras de";
    textSize = 3;
    xRaster = screenWidth/3;
    yRaster = screenHeight-100;
    draw3dString(GLUT_STROKE_ROMAN, title1, xRaster, yRaster, 0);
    
    char title2[] = "Higienix";
    textSize = 4;
    xRaster = screenWidth/3;
    yRaster = screenHeight-155;
    draw3dString(GLUT_STROKE_ROMAN, title2, xRaster, yRaster, 0);
    
    char jugar[] = "Jugar";
    char niveles[] = "Niveles";
    char instr[] = "Instrucciones";
    
    
    textSize = 2;
    
    xRaster = screenWidth/3;
    yRaster = 400;
    draw3dString(GLUT_STROKE_ROMAN, jugar, xRaster, yRaster, 0);
    
    xRaster = screenWidth/3;
    yRaster = 300;
    draw3dString(GLUT_STROKE_ROMAN, niveles, xRaster, yRaster, 0);
    
    xRaster = screenWidth/3;
    yRaster = 200;
    draw3dString(GLUT_STROKE_ROMAN, instr, xRaster, yRaster, 0);
    
    textSize = 1;
    char salir[] = "salir-esc";
    xRaster = 5;
    yRaster = 10;
    draw3dString(GLUT_STROKE_ROMAN, salir, xRaster, yRaster, 0);
    
    
    /*
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(540.0f, 0.0f, 0);
    
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(700.0f, 0.0f, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(700.0f, 160.0f, 0);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(540.0f, 160.0f, 0);
    glEnd();
     */
}

void drawGameInstructions()
{
    glColor3d(0.0, 0.0, 0.0);
    textSize = 1;
    xRaster = 10;
    yRaster = screenHeight - 30;
    char juego[] = "Pantalla de instrucciones";
    draw3dString(GLUT_STROKE_ROMAN, juego, xRaster, yRaster, 0);
    
    glColor3d(0.0, 0.0, 0.0);
    textSize = 1;
    xRaster = 225;
    yRaster = 10;
    char creditos[] = "Eliezer Galvan - Iliana Garcia";
    draw3dString(GLUT_STROKE_ROMAN, creditos, xRaster, yRaster, 0);
}

void drawPlay()
{
    glColor3d(0.0, 0.0, 0.0);
    textSize = 1;
    xRaster = 10;
    yRaster = screenHeight - 30;
    char juego[] = "Pantalla de juego";
    draw3dString(GLUT_STROKE_ROMAN, juego, xRaster, yRaster, 0);
    
    if (pauseFlag) {
        textSize = 4;
        xRaster = screenWidth / 3 + 50;
        yRaster = screenHeight / 2;
        char pauseStr[] = "PAUSA";
        draw3dString(GLUT_STROKE_MONO_ROMAN, pauseStr, xRaster, yRaster, 0);
    }
}

/*
//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image,int k)
{
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
    
}

void initRendering()
{
    GLuint i=0;
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    
    GLfloat directedLight[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat directedLightPos[] = {-10.0f, 15.0f, 20.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, directedLight);
    glLightfv(GL_LIGHT0, GL_POSITION, directedLightPos);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE); ///Users/mariaroque/Imagenes
    
    glEnable(GL_TEXTURE_2D);
    
    // glEnable(GL_COLOR_MATERIAL);
    glGenTextures(36, texName); //Make room for our texture
    Image* image;
    
    image = loadBMP("/Users/ilianagh/Documents/Projects-Xcode/Higienix/Higienix/bmp Tec Mty.bmp");
    loadTexture(image,i++);
    
    delete image;
}
*/

void onMenu(int v)
{
    switch (v) {
        case MENU:
            game_screen = MENU;
            break;
        case INSTRUCTIONS:
            game_screen = INSTRUCTIONS;
            break;
        case 100:
            exit(0);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void createMenus()
{
    int mainMenu, creditsMenu;
    
    creditsMenu = glutCreateMenu(onMenu);
    glutAddMenuEntry("Eliezer Galvan", -1);
    glutAddMenuEntry("Iliana Garcia", -1);
    
    mainMenu = glutCreateMenu(onMenu);
    glutAddMenuEntry("Menú principal", MENU);
    glutAddMenuEntry("Instrucciones", INSTRUCTIONS);
    glutAddSubMenu("Creditos", creditsMenu);
    glutAddMenuEntry("SALIR", 100);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//
// INIT + FUNCs
//

void init()
{
    glClearColor(220.0/255.0, 245.0/255.0, 255.0/255.0, 1.0); // light blue background color
}

void display()
{
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (game_screen == MENU)
    {
        drawMenuScreen();
    }
    else if (game_screen == INSTRUCTIONS)
    {
        drawGameInstructions();
    }
    else if (game_screen == PLAY)
    {
        drawPlay();
    }
    
    glutSwapBuffers();
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

void timer(int value)
{
    
}

void keyboard(unsigned char key, int mouseX, int mouseY)
{
    switch (key){
        case 'm':
        case 'M':
            game_screen = MENU;
            break;
        case 'j': // jugar
        case 'J':
            game_screen = PLAY;
            game_state = playing;
            break;
        case 'n': // niveles
        case 'N':
            break;
        case 'i': // instrucciones
        case 'I':
            if (game_screen == INSTRUCTIONS)
            {
                game_screen = MENU;
            }
            else {
                game_screen = INSTRUCTIONS;
            }
            break;
        case 'p':
        case 'P':
            if (game_state == playing)
            {
                pauseFlag = !pauseFlag;
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
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        
    }
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
    //initRendering();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(5, timer, 1);
    
    createMenus();
    
    glutMainLoop();
    
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////

