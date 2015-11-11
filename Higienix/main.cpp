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
const int movee[4][2] = {{-1,0},{0,-1},{1,0},{0,1}};
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
void displayMaze(void);
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

////////////////////////////////////////////////////

void drawFloor(GLfloat x1, GLfloat x2, GLfloat z1, GLfloat z2)
{
    //glColor3fv(floorColor);
    
    glBindTexture (GL_TEXTURE_2D, texName[FLOOR_TEX]);
    
    glBegin(GL_POLYGON);
    
    glNormal3f(0.0, 1.0, 0.0);
    
    glTexCoord2f(0,0);
    glVertex3f(x1, 0, z2);
    
    glTexCoord2f(0.5,0);
    glVertex3f(x2, 0, z2);
    
    glTexCoord2f(0.5,0.5);
    glVertex3f(x2, 0, z1);
    
    glTexCoord2f(0,0.5);
    glVertex3f(x1, 0, z1);
    
    glEnd();
}

void drawBox (GLint j, GLint i)
{
    //glColor3fv(boxColor);
    
    GLfloat x1=i, x2=i+1, y1=0, y2=1.5, z1=j, z2=j+1;
    glBindTexture(GL_TEXTURE_2D, texName[WALL_TEX]);
    
    if ((j==map.z+1)||(data[j-1][i]!='x'))
    {
        glBegin(GL_POLYGON); // Back
        glNormal3f( 0.0, 0.0, -1.0);
        glTexCoord2f(0,0);
        glVertex3f( x2, y1, z1 );
        glTexCoord2f(1,0);
        glVertex3f( x1, y1, z1 );
        glTexCoord2f(1,1);
        glVertex3f( x1, y2, z1 );
        glTexCoord2f(0,1);
        glVertex3f( x2, y2, z1 );
        glEnd();
    }
    if ((j==map.z-1)||(data[j+1][i]!='x'))
    {
        glBegin(GL_POLYGON); // Front
        glNormal3f( 0.0, 0.0, 1.0);
        glTexCoord2f(0,0);
        glVertex3f( x1, y1, z2 );
        glTexCoord2f(1,0);
        glVertex3f( x2, y1, z2 );
        glTexCoord2f(1,1);
        glVertex3f( x2, y2, z2 );
        glTexCoord2f(0,1);
        glVertex3f( x1, y2, z2 );
        glEnd();
    }
    if ((i>0)&&(data[j][i-1]!='x'))
    {
        glBegin(GL_POLYGON); // Left
        glNormal3f( -1.0, 0.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x1, y1, z1 );
        glTexCoord2f(1,0);
        glVertex3f( x1, y1, z2 );
        glTexCoord2f(1,1);
        glVertex3f( x1, y2, z2 );
        glTexCoord2f(0,1);
        glVertex3f( x1, y2, z1 );
        glEnd();
    }
    if ((i<map.x)&&(data[j][i+1]!='x'))
    {
        glBegin(GL_POLYGON); // Right
        glNormal3f( 1.0, 0.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x2, y1, z2 );
        glTexCoord2f(1,0);
        glVertex3f( x2, y1, z1 );
        glTexCoord2f(1,1);
        glVertex3f( x2, y2, z1 );
        glTexCoord2f(0,1);
        glVertex3f( x2, y2, z2 );
        glEnd();
    }
    glBegin(GL_POLYGON); // Top
    glNormal3f( 0.0, 1.0, 0.0);
    glTexCoord2f(0,0);
    glVertex3f( x1, y2, z2 );
    glTexCoord2f(1,0);
    glVertex3f( x2, y2, z2 );
    glTexCoord2f(1,1);
    glVertex3f( x2, y2, z1 );
    glTexCoord2f(0,1);
    glVertex3f( x1, y2, z1 );
    glEnd();
}

void draw3dString (void *font, char *s, float x, float y, float z, float scale)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScaled(scale,scale,scale);
    for (int i = 0; i < s[i] != '\0'; i++){
        glutStrokeCharacter(font, s[i]);
    }
    glPopMatrix();
}

void animate()
{
    if ((player.x == maze_exit.x) && (player.z == maze_exit.z)){
        exitGame(true);
    }
    
    if (player.isGo == true){
        
        if(player.dx > 0)
            player.dx += 1;
        else if(player.dz > 0)
            player.dz += 1;
        else if(player.dx < 0)
            player.dx -= 1;
        else if(player.dz < 0)
            player.dz -= 1;
        
        if((player.dx >= sMax) || (player.dz >= sMax)){
            
            player.isGo = false;
            
            if (player.dx > 0)
                player.x += 1;
            
            if (player.dz > 0)
                player.z += 1;
            
            player.dx = 0; player.dz = 0;
            
        }else
            
            if((player.dx <= -sMax) || (player.dz <= -sMax)){
                player.isGo = false;
                if(player.dx < 0)
                    player.x -= 1;
                if(player.dz < 0)
                    player.z -= 1;
                player.dx = 0;
                player.dz = 0;
            }
    }
    
    glutPostRedisplay();
}

//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image, int k)
{
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //Map the image to the texture
    glTexImage2D(
                 GL_TEXTURE_2D,                	// Always GL_TEXTURE_2D
                 0,                            	// 0 for now
                 GL_RGB,                      	// Format OpenGL uses for image
                 image->width,
                 image->height,					// Width and height
                 0,                            	// The border of the image
                 GL_RGB, 						// GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, 				// GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
                 image->pixels					// The actual pixel data
                 );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void loadImage(std::string image_name, int image_id)
{
    Image *image;
    std::string ruta = fullPath + image_name;
    std::cout << "Filepath: " << ruta << std::endl;
    image = loadBMP(ruta.c_str());
    loadTexture(image,image_id);
    delete image;
}

bool good_move(int z, int x)
{
    return (0<=z && 0<=x && z<map.z && x<map.x && data[z][x]!='x');
}

void getParentPath()
{
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i,1);
    }
}

int step(int y, int x)
{
    int res=0;
    if (data[y][x]=='x') res++;
    if ((y<mm)&&(data[y+1][x]=='x')) res++;
    if ((y>0 )&&(data[y-1][x]=='x')) res++;
    if ((x<mm)&&(data[y][x+1]=='x')) res++;
    if ((x>0 )&&(data[y][x-1]=='x')) res++;
    return res;
}

int stepC(int y, int x)
{
    int res=0;
    if (cp[y][x]=='x') res++;
    if ((y<mm)&&(cp[y+1][x]=='x')) res++;
    if ((y>0 )&&(cp[y-1][x]=='x')) res++;
    if ((x<mm)&&(cp[y][x+1]=='x')) res++;
    if ((x>0 )&&(cp[y][x-1]=='x')) res++;
    return res;
}

void DataToCp(void)
{
    for(int j=1;j<=mm;j++)
        for (int i=1;i<=mm;i++)
            cp[j][i]=data[j][i];
}

void fill(int y,int x,Tpos *v, int *l)
{
    int st=1,en=0;
    int G[mm*mm],Ll[mm*mm];
    G[0]=y*mm+x; Ll[0]=0;
    cp[y][x]='x';
    while (st!=en)
    {
        if ((G[en]%mm+1<mm)&&(cp[(int)G[en]/mm][G[en]%mm+1]=='.'))
        {
            G[st]=G[en]+1;
            Ll[st]=Ll[en]+1;
            cp[(int)G[st]/mm][G[st]%mm]='x';
            st++;
        }
        if ((G[en]%mm-1>0)&&(cp[(int)G[en]/mm][G[en]%mm-1]=='.'))
        {
            G[st]=G[en]-1;
            Ll[st]=Ll[en]+1;
            cp[(int)G[st]/mm][G[st]%mm]='x';
            st++;
        }
        if ((((int)G[en]/mm)+1<mm)&&(cp[((int)G[en]/mm)+1][G[en]%mm]=='.'))
        {
            G[st]=G[en]+mm;
            Ll[st]=Ll[en]+1;
            cp[(int)G[st]/mm][G[st]%mm]='x';
            st++;
        }
        if ((((int)G[en]/mm)-1>0)&&(cp[((int)G[en]/mm)-1][G[en]%mm]=='.'))
        {
            G[st]=G[en]-mm;
            Ll[st]=Ll[en]+1;
            cp[(int)G[st]/mm][G[st]%mm]='x';
            st++;
        }
        en++;
    }
    int rnd=rand()%5+1;
    (*v).x =(int) G[en-rnd] % mm;
    (*v).y =(int) G[en-rnd] / mm;
    *l=Ll[en-rnd];
}

bool isGood(Tpos a)
{
    DataToCp();
    int k=0,i,j,lt;
    Tpos temp;
    cp[a.y][a.x]='x';
    if ((step(a.y,a.x)>3)||(step(a.y+1,a.x)>3)||(step(a.y-1,a.x)>3)||(step(a.y,a.x+1)>3)||(step(a.y,a.x-1)>3))
        return false;
    for(j=1;j<=mapSize;j++)
        for(i=1;i<=mapSize;i++)
            if (cp[j][i]=='.')
            {
                if (k>0)
                    return false;
                fill(j,i,&temp,&lt);
                k++;
            }
    return true;
}

void genMap()
{
    int i,j;
    map.x=mapSize+2; map.z=mapSize+2;
    
    if(DEBUGGING){
        printf("Loading...\n");
    }
    
    srand( (unsigned)time( NULL ) );
    for (j=0;j<map.z;j++)
        for (i=0;i<map.x;i++)
        {
            if ((i==0)||(j==0)||(i==map.x-1)||(j==map.z-1))
                data[j][i]='x';
            else
                data[j][i]='.';
        }
    
    int k=0;
    Tpos t;
    while (k<(int)mapSize*mapSize/2.5)
    {
        t.x = rand()%mm+1;
        t.y = rand()%mm+1;
        if ((data[t.y][t.x]=='.')&&(isGood(t)))
        {
            data[t.y][t.x]='x';
            k++;
        }
    }
    for (j=1;j<=mapSize;j++)
        for (i=1;i<=mapSize;i++)
            if ((data[j][i]=='x')&&(step(j,i)>3))
            {
                data[j][i]='.';
            }
    for (j=1;j<=mapSize;j++)
        for (i=1;i<=mapSize;i++)
            if ((data[j][i]=='.')&&(step(j,i)<2))
            {
                t.x = i; t.y=j;
                if (isGood(t))
                    data[j][i]='x';
            }
    
    Tpos ps[11]; k=0;
    while (k<=10)
    {
        t.x = rand()%mm+1;
        t.y = rand()%mm+1;
        if (data[t.y][t.x]=='.')
        {
            ps[k]=t;
            k++;
        }
    }
    k=rand()%11;
    player.x=ps[k].x;
    player.z=ps[k].y;
    DataToCp();
    fill(ps[k].y,ps[k].x,&t,&minPathLength);
    maze_exit.x=t.x;
    maze_exit.z=t.y;
    
    if(DEBUGGING){
        printf("Maze loading complete.\n");
        for (j=0;j<map.z;j++){
            for (i=0;i<map.x;i++){
                printf("%c ",data[j][i]);
            }
            printf("\n");
        }
    }
}

void exitGame(bool f)
{
    glutDestroyWindow(glWin);
    
    if (f == true && DEBUGGING){
        printf("The shortest path %d.\n",minPathLength);
        printf("Your path %d.\n",myPathLength);
    }
    
    exit(EXIT_SUCCESS);
}

/////////

//
// INIT + FUNCs
//

void init()
{
    glClearColor(220.0/255.0, 245.0/255.0, 255.0/255.0, 1.0); // light blue background color
    
    ///////////////
    
    //General settings
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glShadeModel(GL_SMOOTH);
    
    //Lightning settings
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHTING);
    
    //Texture settings
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    
    //Variable and flag initial values
    player.dx = 0; player.dz = 0; player.isGo = false;
    
    //Load textures
    glGenTextures(26, texName); //Make room for our texture
    
    GLuint i = 0;
    
    loadImage("textures/floor.bmp", i++);
    loadImage("textures/wall.bmp", i++);
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
        //drawPlay();
        displayMaze();
    }
    
    glutSwapBuffers();
}

void displayMaze()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*
     //BEGIN 2D
     glMatrixMode(GL_PROJECTION);
     glPushMatrix();
     glLoadIdentity();
     glOrtho(0, screenWidth, 0, screenHeight, 1, 20);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glDisable(GL_DEPTH_TEST);
     ///
     
     //Draw points in banner
     char points[50];
     sprintf(points, "Points: %d",0);
     glColor3d(1, 1, 1);
     draw3dString(GLUT_STROKE_ROMAN, points, 50, 50, 20, 0.2);
     
     //END 2D
     glEnable(GL_DEPTH_TEST);
     glMatrixMode(GL_PROJECTION);
     glPopMatrix();
     glMatrixMode(GL_MODELVIEW);
     ///
     */
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //Set camera
    gluLookAt(
              player.x+(1.0*player.dx/sMax)+0.5f+3*cos(M_PI_2+current_direction/double(direction_parts)*2.*M_PI),
              player.y+camera_zoom,
              player.z+(1.0*player.dz/sMax)+0.5f+3*sin(M_PI_2+current_direction/double(direction_parts)*2.*M_PI),
              player.x+(1.0*player.dx/sMax)+0.5f,
              player.y+0.5f,
              player.z+(1.0*player.dz/sMax)+0.5f,
              0,
              1,
              0
              );
    
    //Draw maze
    for (int i=0; i<map.x; i++){
        for (int j=0; j<map.z; j++){
            if (data[j][i] == 'x'){
                drawBox(j,i);
            }else {
                drawFloor(i,i+1,j,j+1);
            }
        }
    }
    
    //Draw player
    glPushMatrix();
    glTranslatef (player.x+(1.0*player.dx/sMax)+0.5f, player.y+0.5f, player.z+(1.0*player.dz/sMax)+0.5f);
    glColor3d(1,0,0);
    glutSolidSphere(0.3,100,100);
    glColor3d(1, 1, 1);
    glPopMatrix();
    
    //Draw maze exit
    glPushMatrix();
    glTranslatef (maze_exit.x+0.5f, maze_exit.y+0.5f, maze_exit.z+0.5f);
    glColor3d(0, 1, 0);
    glutSolidSphere(0.5,100,100);
    glColor3d(1, 1, 1);
    glPopMatrix();
    
    
    //glutSwapBuffers();
}

void reshape(int w, int h)
{
    //glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity ();
    //glOrtho(0, screenWidth, 0, screenHeight, 100, 300 ); //izq, der, abajo, arriba, cerca, lejos
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);
    
    ////////////////
    
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 60.0);
    //glOrtho(0, screenWidth, 0, screenHeight, 100, 300);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (game_screen != PLAY) {
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
    }
    else
    {
        switch (key) {
            case 'q':
            case 'Q':
            case 27:
                exitGame(false);
                break;
                
            case 'a':
            case 'A':
                current_direction--;
                if(current_direction<0) current_direction += direction_parts;
                break;
                
            case 'd':
            case 'D':
                current_direction++;
                if(current_direction == direction_parts) current_direction = 0;
                break;
                
            case 's':
            case 'S':
                if(camera_zoom < 58.0) camera_zoom += 0.25;
                break;
                
            case 'w':
            case 'W':
                if(camera_zoom > 3.0) camera_zoom -= 0.25;
                break;
                
            case 'r':
            case 'R':
                current_direction = 0;
                camera_zoom = 4.0;
                break;
                
            default:
                break;
        }
    }
    
    glutPostRedisplay();
}

/*
 void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        
    }
}
*/

void key_arrow_movements( int key, int x, int y)
{
    if (player.isGo) return;
    
    int dir = int(((direction_parts-current_direction-1) / double(direction_parts)) * 4.0 + 0.5);
    
    for (int i=0; i<4; i++) {
        if ( key == move_key[i] ) {
            int newz = player.z + movee[(dir+i)%4][0];
            int newx = player.x + movee[(dir+i)%4][1];
            
            if (good_move(newz,newx)) {
                player.isGo = true;
                player.dz += movee[(dir+i)%4][0];
                player.dx += movee[(dir+i)%4][1];
                myPathLength++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    //Find root path for files
    getParentPath();
    
    //Generate laberynth map
    genMap();
    
    //Glut settings
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(screenWidth,screenHeight);
    
    if(!DEBUGGING){
        glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-screenWidth)/2,(glutGet(GLUT_SCREEN_HEIGHT)-screenWidth)/2);
    }
    
    glWin = glutCreateWindow("Higienx");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(key_arrow_movements);
    glutIdleFunc(animate);
    
    createMenus();
    
    glutMainLoop();
    
    if(!DEBUGGING){
        glutFullScreen();
    }
    
    glutMainLoop();
    return EXIT_SUCCESS;
}
