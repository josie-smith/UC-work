//  ========================================================================
//  COSC363 Computer Graphics Assignment 1
//
//  FILE NAME: Museum.cpp
//  AUTHOR: Josie Smith
//  ========================================================================

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "loadTGA.h"
using namespace std;
#define GL_CLAMP_TO_EDGE 0x812F

// world parameters
float moveSpeed = 5;
float startAngle = -30;
float startx = 300;
float startz = 600;
float height = 30;


float black[4] = {0.0, 0.0, 0.0, 1.0};
float grey[4] = {0.2, 0.2, 0.2, 1.0};
float white[4]  = {1.0, 1.0, 1.0, 1.0};
float yellow[4] = {237.0/255, 150.0/255, 70.0/255};
GLUquadric *q;    //Required for creating cylindrical objects
GLuint txId[23];   //Texture ids

float cameraAngle=startAngle*M_PI/180, look_x, look_z, eye_x = startx, eye_z = startz, new_x, new_z;  //Camera parameters
float orreryAngle = 0; //Planet angles

// comet variables
float cometAngle = 0;
float cometSize = 70;
float cometEccentricity = 0.8;
float cometMass = 150;
float cometx;
float comety;

// book variable
float pageParameter = -1;
float bookParameter = -1;
bool openingLeft = false;
bool closingLeft = false;
bool openingRight = false;
bool closingRight = false;
bool turningForwards = false;
bool turningBackwards = false;
bool showPage = false;
int pageCount = 0;
int numPages = 2;

//fractal variable
float fractalAngle = 70;
float fractalHeight = 0;
bool up = true;

// rotate the molecule via virtual trackball
//float screenRadius = 425;
//float moleculeAngle = 0;
//float moleculeAxis[] = {0.0, 1.0, 0.0};
//float startMotion[] = {0.0, 0.0, 0.0};
//float stopMotion[] = {0.0, 0.0, 0.0};

// for loading a .off mesh file
float *x, *y, *z;					//vertex coordinates
int *nv, *t1, *t2, *t3, *t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces
float xmin, xmax, ymin, ymax;		//min, max values of  object coordinates

//shadow
float shadowMat[16] = { 800.0f,0,0,0, -300.0f,0,-2000.0f,-1, 0,0,800.0f,0, 0,0,0,800.0f }; // uses lgt_pos

//--------------------------------------------------------------------------------
void loadTexture()
{
    glGenTextures(23, txId); 	// Create 6 texture ids

    glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadTGA("Textures/skybox/bottom.tga"); // calls glTexImage2D
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[1]);
    loadTGA("Textures/skybox/back.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[2]);
    loadTGA("Textures/skybox/front.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[3]);
    loadTGA("Textures/skybox/left.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[4]);
    loadTGA("Textures/skybox/right.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[5]);
    loadTGA("Textures/skybox/top.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[6]);
    loadTGA("Textures/Snow.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, txId[7]);
    loadTGA("Textures/Stars.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[8]);
    loadTGA("Textures/door.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[9]);
    loadTGA("Textures/book/cover.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[10]);
    loadTGA("Textures/book/pages.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[11]);
    loadTGA("Textures/book/book_side.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[12]);
    loadTGA("Textures/book/book_end.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[13]);
    loadTGA("Textures/planets/sun.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[14]);
    loadTGA("Textures/planets/mercury.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[15]);
    loadTGA("Textures/planets/venus.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[16]);
    loadTGA("Textures/planets/earth.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[17]);
    loadTGA("Textures/planets/mars.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[18]);
    loadTGA("Textures/planets/jupiter.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[19]);
    loadTGA("Textures/planets/saturn.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[20]);
    loadTGA("Textures/planets/uranus.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[21]);
    loadTGA("Textures/planets/neptune.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, txId[22]);
    loadTGA("Textures/planets/saturn_rings.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT) cameraAngle -= 0.1;   //Change direction
    else if (key == GLUT_KEY_RIGHT) cameraAngle += 0.1;
    else if (key == GLUT_KEY_DOWN) {                //Move backward
        new_x = eye_x - moveSpeed*sin(cameraAngle);
        new_z = eye_z + moveSpeed*cos(cameraAngle);
    }
    else if(key == GLUT_KEY_UP) {                   //Move forward
        new_x = eye_x + moveSpeed*sin(cameraAngle);
        new_z = eye_z - moveSpeed*cos(cameraAngle);
    }

    // collision detection
    float orrerySize = 20;
    float floorSize = 1000;
    float orreryRoom = 130;
    float museumWidth = 150;
    float museumBack = 50;
    float museumFront = 250;

    bool collision_x = false;
    bool collision_z = false;

    if (sqrt(new_x * new_x + new_z * new_z) < orrerySize ||              // don't walk into the sun
            sqrt(new_x * new_x + new_z * new_z) > floorSize) {           // don't fall off the floor
        collision_x = true;
        collision_z = true;
    }
    if (sqrt(eye_x * eye_x + eye_z * eye_z) <= orreryRoom - 5 &&
            sqrt(new_x * new_x + new_z * new_z) > orreryRoom - 5) {      // if trying to leave the orrery
        if ( abs(new_x) > 15 || new_z < 0 ) {                               // if leaving through the door
            collision_x = true;
            collision_z = true;
        } else {
            new_z += 10;
        }
    }
    if (sqrt(eye_x * eye_x + eye_z * eye_z) >= orreryRoom + 5 &&
            sqrt(new_x * new_x + new_z * new_z) < orreryRoom + 5) {      // if trying to enter the orrery
        if ( abs(new_x) > 15 || new_z < 0 ) {                               // if entering through the door
            collision_x = true;
            collision_z = true;
        } else {
            new_z -= 10;
        }
    }
    if (sqrt(eye_x * eye_x + eye_z * eye_z) > orreryRoom) {              // outside the orrery
        if (eye_z >= museumBack + 5 && new_z < museumBack + 5 &&            // if trying to leave through the back of the museum
                abs(new_x) <= museumWidth) {                                // if outside already
            collision_z = true;
        }
        if (eye_z <= museumBack - 5 && new_z > museumBack -5 &&             // if trying to enter through the back of the museum
                abs(new_x) <= museumWidth) {                                // if outside already
            collision_z = true;
        }
        if (eye_z >= museumFront + 5 && new_z < museumFront + 5) {          // if trying to enter through the front of the museum
            if (abs(new_x) > 30 && abs(new_x) <= museumWidth) {             // if entering through the door or outside already
                collision_z = true;
            } else {
                new_z -= 10;
            }
        }
        if (eye_z <= museumFront - 5 && new_z > museumFront - 5) {          // if trying to leave through the front of the museum
            if (abs(new_x) > 30 && abs(new_x) <= museumWidth) {             // if leaving through the door or outside already
                collision_z = true;
            } else {
                new_z += 10;
            }
        }
        if (abs(eye_x) >= museumWidth + 5 && abs(new_x) < museumWidth + 5 && // if trying to enter through the side of the museum
                ( new_z <= museumFront && new_z >= museumBack )) {           // if outside already
            collision_x = true;
        }
        if (eye_x >= -museumWidth + 80 && new_x < -museumWidth + 80 &&      // if trying to leave through the left side of the museum
                ( new_z <= museumFront && new_z >= museumBack )) {          // if outside already
            collision_x = true;
        }
        if (eye_x <= museumWidth - 60 && new_x > museumWidth - 60 &&        // if trying to leave through the left side of the museum
                ( new_z <= museumFront && new_z >= museumBack )) {          // if outside already
            collision_x = true;
        }
    }
    if (!collision_x) {
        eye_x = new_x;
    }
    if (!collision_z) {
        eye_z = new_z;
    }

    look_x = eye_x + 100*sin(cameraAngle);
    look_z = eye_z - 100*cos(cameraAngle);
    glutPostRedisplay();
}

void mouse(int button, int state, int mousex, int mousey)
{
    //virtual trackball without passive mouse movements
//    float x = mousex-300;
//    float y = 300-mousey;
//    float z = sqrt(screenRadius*screenRadius - x*x - y*y);
//    float magnitude = sqrt(x*x + y*y + z*z);
//    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
//    {
//        startMotion[0] = x/magnitude;
//        startMotion[1] = y/magnitude;
//        startMotion[2] = z/magnitude;
//    }
//    if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
//    {
//        stopMotion[0] = x/magnitude;
//        stopMotion[1] = y/magnitude;
//        stopMotion[2] = z/magnitude;
//        moleculeAngle = (180/M_PI)*acos(startMotion[0]*stopMotion[0] + startMotion[1]*stopMotion[1] + startMotion[2]*stopMotion[2]);
//        moleculeAxis[0] = startMotion[1]*stopMotion[2] - stopMotion[1]*startMotion[2];
//        moleculeAxis[1] = startMotion[2]*stopMotion[0] - stopMotion[2]*startMotion[0];
//        moleculeAxis[2] = startMotion[0]*stopMotion[1] - stopMotion[0]*startMotion[1];
//
//        glutPostRedisplay();
//    }

    if (!openingRight && !openingLeft && !closingRight && !closingLeft && !turningForwards && !turningBackwards) {
        if (button == GLUT_LEFT_BUTTON) {
            if (bookParameter > 0.99) {
                bookParameter = 1;
                openingRight = true;
            } else if (abs(bookParameter) < 0.01) {
                bookParameter = 0;
                if (pageCount > 0) {
                    turningBackwards = true;
                    pageParameter = 1;
                    showPage = true;
                } else {
                    closingRight = true;
                }
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (bookParameter < -0.99) {
                bookParameter = -1;
                openingLeft = true;
            } else if (abs(bookParameter) < 0.01) {
                bookParameter = 0;
                if (pageCount < numPages) {
                    turningForwards = true;
                    pageParameter = -1;
                    showPage = true;
                } else {
                    closingLeft = true;
                }
            }
        }
    }
}

void myTimer(int value)
{
    // normal planets
    orreryAngle++;

    // comet
    float r = cometSize*(1-cometEccentricity*cometEccentricity) / (1+ cometEccentricity*cos(cometAngle)); // r and cometAngle are the polar coordinates
    cometx = r*cos(cometAngle); // calculate cartesian coordinates
    comety = r*sin(cometAngle);
    cometAngle += cometMass / (r*r); // calculate angle to use for the next iteration

    // fractal
    fractalAngle += 0.5;
    if (fractalHeight >= 2) {
        up = false;
    } else if (fractalHeight <= -2) {
        up = true;
    }
    if (up) {
        fractalHeight += 0.05;
    } else {
        fractalHeight -= 0.05;
    }

    // book
    if (openingLeft) {
        if (bookParameter < 0) {
            bookParameter += 0.05;
        } else {
            openingLeft = false;
        }
    }
    if (closingRight) {
        if (bookParameter > -1) {
            bookParameter -= 0.05;
        } else {
            closingRight = false;
        }
    }
    if (openingRight) {
        if (bookParameter > 0) {
            bookParameter -= 0.05;
        } else {
            openingRight = false;
        }
    }
    if (closingLeft) {
        if (bookParameter < 1) {
            bookParameter += 0.05;
        } else {
            closingLeft = false;
        }
    }
    if (turningForwards) {
        if (pageParameter < 1) {
            pageParameter += 0.1;
        } else {
            turningForwards = false;
            showPage = false;
            pageCount += 1;
        }
    }
    if (turningBackwards) {
        if (pageParameter > -1) {
            pageParameter -= 0.1;
        } else {
            turningBackwards = false;
            showPage = false;
            pageCount -= 1;
        }
    }
    if (abs(bookParameter) < 0.01) {
        bookParameter = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(50, myTimer, 0);
}

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int ne;

    fp_in.open(fname, ios::in);
    if(!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');				//ignore first line
    fp_in >> nvert >> nface >> ne;			    // read number of vertices, polygons, edges (not used)

    x = new float[nvert];                        //create arrays
    y = new float[nvert];
    z = new float[nvert];

    nv = new int[nface];
    t1 = new int[nface];
    t2 = new int[nface];
    t3 = new int[nface];
    t4 = new int[nface];

    for(int i=0; i < nvert; i++)                         //read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for(int i=0; i < nface; i++)                         //read polygon list
    {
        fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i];
        if (nv[i] == 4)
            fp_in >> t4[i];
        else
            t4[i] = -1;
    }

    fp_in.close();
    cout << " File successfully read." << endl;
}

//-- Computes the min, max values of coordinates  -----------------------
void computeMinMax()
{
    xmin = xmax = x[0];
    ymin = ymax = y[0];
    for(int i = 1; i < nvert; i++)
    {
        if(x[i] < xmin) xmin = x[i];
        else if(x[i] > xmax) xmax = x[i];
        if(y[i] < ymin) ymin = y[i];
        else if(y[i] > ymax) ymax = y[i];
    }
}

void normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 )
{
    float nx, ny, nz;
    nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
    ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
    nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

    glNormal3f(nx, ny, nz);
}

//--------------------------------------------------------------------------------
void skybox()
{
    float skyboxSize = 4000;
    float skyboxRatio = 0.7;

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.6, 0.6, 0.6);  // replace with texture

    ////////////////////// FLOOR ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(1., 0.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 1.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 1.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glEnd();

    ////////////////////// BACK WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0., 1.);     glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 0.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 1.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glEnd();

    ////////////////////// FRONT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(1., 1.);     glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(1., 0.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 1.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glEnd();

    ////////////////////// LEFT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(1., 1.);     glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 0.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(-1*skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 1.);     glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glEnd();


    ////////////////////// RIGHT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0., 1.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 0.);     glVertex3f(skyboxSize, -skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(1., 1.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glEnd();


    ////////////////////// SKY ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(1., 1.);      glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glTexCoord2f(1., 0.);     glVertex3f(-1*skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 0.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, skyboxSize);
    glTexCoord2f(0., 1.);     glVertex3f(skyboxSize, skyboxRatio*skyboxSize, -1*skyboxSize);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

void floor()
{
    glColor3f(0.8, 0.8, 0.8);  // replace with texture
    glNormal3f(0.0, 1.0, 0.0);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, txId[6]);

    glPushMatrix();
        glRotatef(-90.0, 1., 0., 0.);
        gluDisk(q, 130.0, 1000, 20, 3);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

void orreryRoom()
{
    float orrerySize =  130;

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, txId[7]);

    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
        glTranslatef(0, -100, 0);
        glRotatef(-90.0, 1., 0., 0.);
        gluCylinder(q, orrerySize, orrerySize, 200, 20, 5);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, -100, 0);
        glRotatef(-90.0, 1., 0., 0.);
        gluDisk(q, 0.0, orrerySize, 20, 3);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 73, 0);
        glRotatef(-90.0, 1., 0., 0.);
        gluDisk(q, 0.0, orrerySize, 20, 3);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glColor3f(0.7, 0.7, 0.7);
    glPushMatrix();
        glRotatef(-90.0, 1., 0., 0.);
        gluCylinder(q, orrerySize + 1, orrerySize + 1, 100, 20, 5);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor3f(218.0/255, 165.0/255, 32.0/255); // gold
    glPushMatrix();
        glTranslatef(0, 99, 0);
        glScalef(orrerySize-1, orrerySize-1, orrerySize-1);
        glRotatef(90.0, 0., 0., 1.);
        // draw hemisphere
        for(int indx = 0; indx < nface; indx++)		//draw each face
        {
            if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
            else				glBegin(GL_QUADS);
            glNormal3f(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
            glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
            glNormal3f(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
            glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
            glNormal3f(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
            glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
            if(nv[indx]==4)
                glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
            glEnd();
        }
    glPopMatrix();

    // door
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glColor3f(0.8, 0.7, 0.3);   //replace with a texture
    glBindTexture(GL_TEXTURE_2D, txId[8]);
    glBegin(GL_QUADS);
        glTexCoord2f(0., 0.);     glVertex3f(-15, 65, orrerySize + 1);
        glTexCoord2f(0., 1.);     glVertex3f(-15, 0, orrerySize + 1);
        glTexCoord2f(0.5, 1.);    glVertex3f(15, 0, orrerySize + 1);
        glTexCoord2f(0.5, 0.);    glVertex3f(15, 65, orrerySize + 1);

        glTexCoord2f(0., 0.);     glVertex3f(-15, 65, orrerySize - 2.6);
        glTexCoord2f(0., 1.);     glVertex3f(-15, 0, orrerySize - 2.6);
        glTexCoord2f(0.5, 1.);    glVertex3f(15, 0, orrerySize - 2.6);
        glTexCoord2f(0.5, 0.);    glVertex3f(15, 65, orrerySize - 2.6);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void orrery()
{
    orreryRoom();

    float baseSize = 0.5;
    float baseRadius = 8.0;

    // [sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune]
    float size[] = {baseSize*25, baseSize*1, baseSize*3, baseSize*4, baseSize*2, baseSize*11, baseSize*10, baseSize*7, baseSize*6};
    float orbit[] = {0, baseRadius*2.3f, baseRadius*3, baseRadius*4, baseRadius*5, baseRadius*8, baseRadius*10, baseRadius*13, baseRadius*15};
    float period[] = {1000, 0.2, 0.6, 1, 1.9, 3, 4, 10, 15};
    float color[] = {255.0/255, 160.0/255, 50/255, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    float tilt[] = {7.25, 0.01, 117.3, 23.26, 25.19, 3.13, 26.73, 97.77, 28.32};
    float orbitAngle[] = {0, 7, 3.4, 0, 1.85, 1.3, 3.4, 0.77, 1.77};

    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glPushMatrix();
        glTranslatef(0, baseSize*70, 0);
        for (int i=0; i<9; i++) {
            glPushMatrix();
                glRotatef(orbitAngle[i], 1, 0, 0);          // tilt the orbit
                glRotatef(orreryAngle/period[i], 0., 1., 0.);   // orbit the planet about the sun

                // planet
                if (i == 0) {
                    glEnable(GL_LIGHT2);
                    glMaterialf(GL_FRONT, GL_SHININESS, 3);
                } else {
                    glMaterialf(GL_FRONT, GL_SHININESS, 50);
                }
                glColor3f(color[3*i], color[3*i+1], color[3*i+2]);
                glEnable(GL_TEXTURE_2D);
                glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D, txId[13+i]);

                glPushMatrix();
                    glTranslatef(orbit[i], 0, 0);              // move the planet into orbit
                    glRotatef(tilt[i], 1, 0, 0);            // tilt the axis
                    if (i==2 || i==7) {
                        glRotatef(-orreryAngle*0.3, 0, 1, 0);        // venus and uranus rotate the opposite way
                    } else {
                        glRotatef(orreryAngle*0.3, 0, 1, 0);        // rotate the planet about its axis
                    }
                    glRotatef(90, 1, 0, 0);
                    gluSphere(q, size[i], 36, 18);

                    // saturn's rings
                    if (i == 6) {
                        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
                        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
                        glRotatef(90, 1, 0, 0);
                        glBindTexture(GL_TEXTURE_2D, txId[22]);
                        float vx[37] = {0};     float wx[37] = {0};
                        float vy[37] = {0};     float wy[37] = {0};
                        float vz[37] = {0};     float wz[37] = {0};
                        for (int a = 0; a <= 36; a++) {
                            vx[a] = (size[i] + 1.5)*sin(a*10*M_PI/180.0);
                            vz[a] = (size[i] + 1.5)*cos(a*10*M_PI/180.0);
                            wx[a] = (2*size[i])*sin(a*10*M_PI/180.0);
                            wz[a] = (2*size[i])*cos(a*10*M_PI/180.0);
                        }
                        glBegin(GL_QUAD_STRIP);
                        for (int a = 0; a <= 36; a++) {
                            glTexCoord2f( 0, (float)a/(float)(36));
                            glVertex3f(vx[a], vy[a], vz[a]);
                            if(a > 0) normal(wx[a-1], wy[a-1], wz[a-1], vx[a], vy[a], vz[a], wx[a], wy[a], wz[a]);
                            glTexCoord2f( 1, (float)a/(float)(36));
                            glVertex3f(wx[a], wy[a], wz[a]);
                        }
                        glEnd();
                        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
                    }
                glPopMatrix();
                if (i == 0) {
                    glDisable(GL_LIGHT2);
                }
                glDisable(GL_TEXTURE_2D);

                // rotators
//                glMaterialf(GL_FRONT, GL_SHININESS, 100);
//                glColor3f(218.0/255, 165.0/255, 32.0/255);
//                glPushMatrix();
//                    glRotatef(90.0, 1., 0., 0.);
//                    gluCylinder(q, 0.5, 0.5, baseSize*(30 + 3*i), 20, 5);
//                glPopMatrix();
//                glPushMatrix();
//                    glTranslatef(0, -1*baseSize*(30 + 3*i), 0);
//                    glRotatef(90.0, 0., 1., 0.);
//                    gluCylinder(q, 0.2, 0.2, orbit[i], 20, 5);
//                glPopMatrix();
//                glPushMatrix();
//                    glRotatef(90.0, 1., 0., 0.);
//                    glTranslatef(orbit[i], 0, 0);
//                    gluCylinder(q, 0.2, 0.2, baseSize*(30 + 3*i), 20, 5);
//                glPopMatrix();
            glPopMatrix();
        }

        // stand
//        glPushMatrix();
//            glRotatef(90.0, 1., 0., 0.);
//            gluCylinder(q, 0.5, 0.5, baseSize*70, 20, 5);
//        glPopMatrix();
//        glPushMatrix();
//            glTranslatef(0, baseSize*(-70), 0);
//            glRotatef(-90.0, 1., 0., 0.);
//            gluDisk(q, 0.0, 10.0, 20, 3);
//            glRotatef(180.0, 1., 0., 0.);
//            gluCylinder(q, 10, 10, 1, 20, 5);
//        glPopMatrix();
//        glPushMatrix();
//            glTranslatef(0, baseSize*(-71), 0);
//            glRotatef(-90.0, 1., 0., 0.);
//            gluDisk(q, 0.0, 30.0, 20, 3);
//            glRotatef(180.0, 1., 0., 0.);
//            gluCylinder(q, 30, 30, 0.5, 20, 5);
//        glPopMatrix();

        // comet
        glColor3f(133.0/255, 94.0/255, 66.0/255);
        glMaterialfv(GL_FRONT, GL_SPECULAR, black);
        glPushMatrix();
            glRotatef(-20.0, 0., 1., 1.);
            glRotatef(90.0, 1., 0., 0.);
            glTranslatef(cometx, comety, 0);
            glutSolidSphere(baseSize*2, 36, 18);
        glPopMatrix();
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glPopMatrix();

    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);

    glMaterialf(GL_FRONT, GL_SHININESS, 50);
}

//void molecule()
//{
//    glPushMatrix();
//        glTranslatef(300, 25, 0);
//        glRotatef(moleculeAngle, moleculeAxis[0], moleculeAxis[1], moleculeAxis[2]);
//        glutSolidCube(20);
//    glPopMatrix();
//}

void makeFrontBook(float bookParameter)
{
    glColor3f(1, 0.8, 0.8);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    // front spine
    glBindTexture(GL_TEXTURE_2D, txId[9]);
    float origx[] = { -35, -5, -5, -3, -1, 0, 1, 3, 5, 5, 35 }; // book lying flat
    float origy[] = { 0, 0, 0, -1.4, -1.9, -2, -1.9, -1.4, 0, 0, 0 };
    float vx[11] = {0};
    float vy[11] = {0};
    float vz[11] = {0};
    float wx[11] = {0};
    float wy[11] = {0};
    float wz[11] = {0};
    float theta = 0;
    if (bookParameter < 0) {
        theta = -1* bookParameter * M_PI/2.0;
        vx[10] = origx[10];     vy[10] = origy[10];
        wx[10] = vx[10];        wy[10] = vy[10] + 0.5;
        vx[9] = origx[9];       vy[9] = origy[9];
        wx[9] = vx[9];          wy[9] = vy[9] + 0.5;
        for (int i = 2; i < 9; i++) {
            vx[i] = 5 + (origx[i]-5) * cos(theta) + origy[i] * sin(theta);
            vy[i] = -1*(origx[i]-5) * sin(theta) + origy[i] * cos(theta);
            wx[i] = 5 + (origx[i]-5) * cos(theta) + (origy[i]+0.5) * sin(theta);
            wy[i] = -1*(origx[i]-5) * sin(theta) + (origy[i]+0.5) * cos(theta);
        }
        vx[1] = vx[2];          vy[1] = vy[2];
        wx[1] = vx[1] + 0.5 * sin(2 * theta);   wy[1] = vy[1] + 0.5 * cos(2 * theta);
        vx[0] = vx[1] - 30 * cos(2 * theta);    vy[0] = vy[1] + 30 * sin(2 * theta);
        wx[0] = wx[1] - 30 * cos(2 * theta);    wy[0] = wy[1] + 30 * sin(2 * theta);
    } else {
        theta = bookParameter * M_PI/2.0 + 2*M_PI;
        vx[0] = origx[0];       vy[0] = origy[0];
        wx[0] = vx[0];          wy[0] = vy[0] + 0.5;
        vx[1] = origx[1];       vy[1] = origy[1];
        wx[1] = vx[1];          wy[1] = vy[1] + 0.5;
        for (int i = 2; i < 9; i++) {
            vx[i] = -5 + (origx[i]+5) * cos(theta) - origy[i] * sin(theta);
            vy[i] = (origx[i]+5) * sin(theta) + origy[i] * cos(theta);
            wx[i] = -5 + (origx[i]+5) * cos(theta) - (origy[i]+0.5) * sin(theta);
            wy[i] = (origx[i]+5) * sin(theta) + (origy[i]+0.5) * cos(theta);
        }
        vx[9] = vx[8];          vy[9] = vy[8];
        wx[9] = vx[9] - 0.5 * sin(2 * theta);   wy[9] = vy[9] + 0.5 * cos(2 * theta);
        vx[10] = vx[9] + 30 * cos(2 * theta);   vy[10] = vy[9] + 30 * sin(2 * theta);
        wx[10] = wx[9] + 30 * cos(2 * theta);   wy[10] = wy[9] + 30 * sin(2 * theta);
    }

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 0, 1);
    for (int i = 0; i < 2; i++) {
        glTexCoord2f( 0.6 + 0.39*(1-i), 0.01);   glVertex3f(vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.6 + 0.39*(1-i), 0.02);   glVertex3f(wx[i], wy[i], wz[i]);
    }
    for (int i = 2; i < 9; i++) {
        glTexCoord2f( 0.58 - 0.21*(i-1)/8.0, 0.01);   glVertex3f(vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.58 - 0.21*(i-1)/8.0, 0.02);   glVertex3f(wx[i], wy[i], wz[i]);
    }
    for (int i = 9; i < 11; i++) {
        glTexCoord2f( 0.05 + 0.39*(i-9), 0.01);   glVertex3f(vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.05 + 0.39*(i-9), 0.02);   glVertex3f(wx[i], wy[i], wz[i]);
    }
    glEnd();

    // pivet point
    float pivetrx;
    float pivetry;
    float pivetlx;
    float pivetly;
    float pivetTheta = M_PI * abs(bookParameter) / 2.0;
    float pivetHeight = 3 - 3 * abs(bookParameter);
    pivetrx = -5 * cos(pivetTheta) + pivetHeight * sin(pivetTheta) + 5;
    pivetry = 5 * sin(pivetTheta) + pivetHeight * cos(pivetTheta);
    pivetlx = 5 * cos(pivetTheta) - pivetHeight * sin(pivetTheta) - 5;
    pivetly = 5 * sin(pivetTheta) + pivetHeight * cos(pivetTheta);

    //front pages
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, txId[12]);
    const int n = 13;
    float a = M_PI / 8.0;

    // front pages right
    float ux[] = {5, pivetrx, 0, 0, 0, 0, 25, 27, 29, 31, 33, 35, 5};
    float uy[] = {0.5, pivetry, 0, 0, 0, 0, 5.5, 3.2, 1.9, 1.1, 0.7, 0.5, 0.5};
    float uz[n] = {0};
    if (abs(bookParameter) != 1) {
        for (int i = 2; i < 6; i++) {
            ux[i] = (pivetrx-5) * cos(a*(i-2)) + 5;
            uy[i] = ((5.5-pivetry) / (5-pivetrx)) * (-1 * (pivetrx-5) * sin(a*(i-2))) + pivetry;
        }
    } else {
        for (int i = 2; i < 6; i++) {
            ux[i] = ux[1];
            uy[i] = uy[1];
        }
    }
    for (int i=6; i<11; i++) {
        ux[i] = ux[i] + abs(bookParameter)*(35-ux[i]);
    }

    // front pages left
    float tx[] = {-5, pivetlx, 0, 0, 0, 0, -25, -27, -29, -31, -33, -35, -5};
    float ty[] = {0.5, pivetly, 0, 0, 0, 0, 5.5, 3.2, 1.9, 1.1, 0.7, 0.5, 0.5};
    float tz[n] = {0};
    if (abs(bookParameter) != 1) {
        for (int i = 2; i < 6; i++) {
            tx[i] = (pivetlx + 5) * cos(a * (i-2)) - 5;
            ty[i] = ((5.5-pivetly) / (-5-pivetlx)) * (-1 * (pivetlx+5) * sin(a*(i-2))) + pivetly;
        }
    } else {
        for (int i = 2; i < 6; i++) {
            tx[i] = tx[1];
            ty[i] = ty[1];
        }
    }
    for (int i = 6; i < 11; i++) {
        tx[i] = tx[i] - abs(bookParameter) * (35 + tx[i]);
    }

    if (bookParameter < 0) {
        // draw right pages
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 0, 1);
        glTexCoord2f( 0.3, 0.35);           glVertex3f(8, 3, 0);
        for (int i = 0; i < 1; i++) {
            glTexCoord2f(0.03, 0.1);     glVertex3f(ux[i], uy[i], uz[i]);
        }
        for (int i = 1; i < 6; i++) {
            glTexCoord2f(0.03 + 0.3*(i-1)/4.0, 0.5);     glVertex3f(ux[i], uy[i], uz[i]);
        }
        for (int i = 6; i < 12; i++) {
            glTexCoord2f(0.9, 0.5 - 0.4*(i-6)/5.0);     glVertex3f(ux[i], uy[i], uz[i]);
        }
        for (int i = 12; i < n; i++) {
            glTexCoord2f(0.03, 0.1);     glVertex3f(ux[i], uy[i], uz[i]);
        }
        glEnd();

        // draw left pages
        glPushMatrix();
            glTranslatef(5-10*cos(theta), 10*sin(theta), 0);
            glRotatef(-2*(theta*180/M_PI), 0, 0, 1);
            glTranslatef(5, 0, 0);

            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0, 0, 1);
            glTexCoord2f( 0.7, 0.65);           glVertex3f(-8, 3, 0);
            for (int i = 0; i < 1; i++) {
                glTexCoord2f(0.03, 0.9);     glVertex3f(tx[i], ty[i], tz[i]);
            }
            for (int i = 1; i < 6; i++) {
                glTexCoord2f(0.03 + 0.3*(i-1)/4.0, 0.5);     glVertex3f(tx[i], ty[i], tz[i]);
            }
            for (int i = 6; i < 12; i++) {
                glTexCoord2f(0.9, 0.5 + 0.4*(i-6)/5.0);     glVertex3f(tx[i], ty[i], tz[i]);
            }
            for (int i = 12; i < n; i++) {
                glTexCoord2f(0.03, 0.9);     glVertex3f(tx[i], ty[i], tz[i]);
            }
            glEnd();
        glPopMatrix();
    } else {
        // draw left pages
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 0, 1);
        glTexCoord2f( 0.7, 0.65);           glVertex3f(-8, 3, 0);
        for (int i = 0; i < 1; i++) {
            glTexCoord2f(0.03, 0.9);     glVertex3f(tx[i], ty[i], tz[i]);
        }
        for (int i = 1; i < 6; i++) {
            glTexCoord2f(0.03 + 0.3*(i-1)/4.0, 0.5);     glVertex3f(tx[i], ty[i], tz[i]);
        }
        for (int i = 6; i < 12; i++) {
            glTexCoord2f(0.9, 0.5 + 0.4*(i-6)/5.0);     glVertex3f(tx[i], ty[i], tz[i]);
        }
        for (int i = 12; i < n; i++) {
            glTexCoord2f(0.03, 0.9);     glVertex3f(tx[i], ty[i], tz[i]);
        }
        glEnd();

        // draw right pages
        glPushMatrix();
            glTranslatef(-5+10*cos(theta), 10*sin(theta), 0);
            glRotatef(2*(theta*180/M_PI), 0, 0, 1);
            glTranslatef(-5, 0, 0);

            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0, 0, 1);
            glTexCoord2f( 0.3, 0.35);           glVertex3f(8, 3, 0);
            for (int i = 0; i < 1; i++) {
                glTexCoord2f(0.03, 0.1);     glVertex3f(ux[i], uy[i], uz[i]);
            }
            for (int i = 1; i < 6; i++) {
                glTexCoord2f(0.03 + 0.3*(i-1)/4.0, 0.9);     glVertex3f(ux[i], uy[i], uz[i]);
            }
            for (int i = 6; i < 12; i++) {
                glTexCoord2f(0.9, 0.5 - 0.4*(i-6)/5.0);     glVertex3f(ux[i], uy[i], uz[i]);
            }
            for (int i = 12; i < n; i++) {
                glTexCoord2f(0.03, 0.1);     glVertex3f(ux[i], uy[i], uz[i]);
            }
            glEnd();
        glPopMatrix();
    }

    // front black hole
    glDisable(GL_TEXTURE_2D);
    glColor3f(0, 0, 0);

    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    glVertex3f(wx[1], wy[1], wz[1]);
    for (int i = 3; i < 8; i++) {
        glVertex3f(wx[i], wy[i], wz[i]);
    }
    glVertex3f(wx[9], wy[9], wz[9]);
    if (bookParameter < 0) {
        glVertex3f(pivetrx, pivetry, 0.0);
    } else {
        glVertex3f(pivetlx, pivetly, 0.0);
    }
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void makeSidesBook(float bookParameter)
{
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    // front pages right
    float ux[] = {25, 27, 29, 31, 33, 35};
    float uy[] = {5.5, 3.2, 1.9, 1.1, 0.7, 0.5};
    float uz[6] = {0};
    float wx[] = { 25, 27, 29, 31, 33, 35 };
    float wy[] = { 5.5, 3.2, 1.9, 1.1, 0.7, 0.5 };
    float wz[] = { -50, -50, -50, -50, -50, -50 };
    for (int i=0; i<6; i++) {
        ux[i] = ux[i] + abs(bookParameter)*(35-ux[i]);
        wx[i] = ux[i];
    }

    // front pages left
    float tx[] = {-25, -27, -29, -31, -33, -35};
    float ty[] = {5.5, 3.2, 1.9, 1.1, 0.7, 0.5};
    float tz[6] = {0};
    float sx[] = { 25, 27, 29, 31, 33, 35 };
    float sy[] = { 5.5, 3.2, 1.9, 1.1, 0.7, 0.5 };
    float sz[] = { -50, -50, -50, -50, -50, -50 };
    for (int i = 0; i < 6; i++) {
        tx[i] = tx[i] - abs(bookParameter) * (35 + tx[i]);
        sx[i] = tx[i];
    }

    if (bookParameter < 0) {
        float theta = -1* bookParameter * M_PI/2.0;

        // draw right pages
        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, txId[11]);
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < 6; i++) {
            glTexCoord2f( 0.1, 0.1+ 0.4*i/5.0);      glVertex3f(ux[i], uy[i], uz[i]);
            if (i>0) normal(wx[i-1], wy[i-1], wz[i-1],ux[i], uy[i], uz[i], wx[i], wy[i], wz[i]);
            glTexCoord2f( 0.9, 0.1+ 0.4*i/5.0);      glVertex3f(wx[i], wy[i], wz[i]);
        }
        glEnd();

        // draw right spine
        glColor3f(1, 0.8, 0.8);
        glBindTexture(GL_TEXTURE_2D, txId[9]);
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(0.01, 0.01);      glVertex3f(35, 0.5, 0);
        glTexCoord2f(0.02, 0.01);     glVertex3f(35, 0, 0);
        glTexCoord2f(0.02, 0.99);     glVertex3f(35, 0, -50);
        glTexCoord2f(0.01, 0.99);      glVertex3f(35, 0.5, -50);
        glEnd();

        glPushMatrix();
            glTranslatef(5-10*cos(theta), 10*sin(theta), 0);
            glRotatef(-2*(theta*180/M_PI), 0, 0, 1);
            glTranslatef(5, 0, 0);

            // draw left pages
            glColor3f(1, 1, 1);
            glBindTexture(GL_TEXTURE_2D, txId[11]);
            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i < 6; i++) {
                glTexCoord2f( 0.1, 0.5+ 0.4*i/5.0);     glVertex3f(tx[i], ty[i], tz[i]);
                if (i>0) normal(sx[i-1], sy[i-1], sz[i-1], sx[i], sy[i], sz[i], tx[i], ty[i], tz[i]);
                glTexCoord2f( 0.9, 0.5+ 0.4*i/5.0);     glVertex3f(sx[i], sy[i], sz[i]);
            }
            glEnd();

            // draw left spine
            glColor3f(1, 0.8, 0.8);
            glBindTexture(GL_TEXTURE_2D, txId[9]);
            glBegin(GL_QUADS);
            glNormal3f(-1, 0, 0);
            glTexCoord2f(0.98, 0.01);     glVertex3f(-35, 0.5, 0);
            glTexCoord2f(0.99, 0.01);     glVertex3f(-35, 0, 0);
            glTexCoord2f(0.99, 0.99);    glVertex3f(-35, 0, -50);
            glTexCoord2f(0.98, 0.99);    glVertex3f(-35, 0.5, -50);
            glEnd();
        glPopMatrix();
    } else {
        float theta = bookParameter * M_PI/2.0 + 2*M_PI;

        // draw left pages
        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, txId[11]);
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < 6; i++) {
            glTexCoord2f( 0.1, 0.5+ 0.4*i/5.0);    glVertex3f(tx[i], ty[i], tz[i]);
            if (i>0) normal(sx[i-1], sy[i-1], sz[i-1], sx[i], sy[i], sz[i], tx[i], ty[i], tz[i]);
            glTexCoord2f( 0.9, 0.5+ 0.4*i/5.0);    glVertex3f(sx[i], sy[i], sz[i]);
        }
        glEnd();

        // draw left spine
        glColor3f(1, 0.8, 0.8);
        glBindTexture(GL_TEXTURE_2D, txId[9]);
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0.98, 0.01);     glVertex3f(-35, 0.5, 0);
        glTexCoord2f(0.99, 0.01);     glVertex3f(-35, 0, 0);
        glTexCoord2f(0.99, 0.99);    glVertex3f(-35, 0, -50);
        glTexCoord2f(0.98, 0.99);    glVertex3f(-35, 0.5, -50);
        glEnd();

        glPushMatrix();
            glTranslatef(-5+10*cos(theta), 10*sin(theta), 0);
            glRotatef(2*(theta*180/M_PI), 0, 0, 1);
            glTranslatef(-5, 0, 0);

            // draw right pages
            glColor3f(1, 1, 1);
            glBindTexture(GL_TEXTURE_2D, txId[11]);
            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i < 6; i++) {
                glTexCoord2f( 0.1, 0.1+ 0.4*i/5.0);    glVertex3f(ux[i], uy[i], uz[i]);
                if (i>0) normal(wx[i-1], wy[i-1], wz[i-1],ux[i], uy[i], uz[i], wx[i], wy[i], wz[i]);
                glTexCoord2f( 0.9, 0.1+ 0.4*i/5.0);    glVertex3f(wx[i], wy[i], wz[i]);
            }
            glEnd();

            // draw right spine
            glColor3f(1, 0.8, 0.8);
            glBindTexture(GL_TEXTURE_2D, txId[9]);
            glBegin(GL_QUADS);
            glNormal3f(1, 0, 0);
            glTexCoord2f(0.01, 0.01);      glVertex3f(35, 0.5, 0);
            glTexCoord2f(0.02, 0.01);     glVertex3f(35, 0, 0);
            glTexCoord2f(0.02, 0.99);     glVertex3f(35, 0, -50);
            glTexCoord2f(0.01, 0.99);      glVertex3f(35, 0.5, -50);
            glEnd();
        glPopMatrix();
    }
}

void makePages(float bookParameter)
{
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, txId[10]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    const int n = 5;
    float a = M_PI / 8.0;

    // pivet point
    float pivetrx;
    float pivetry;
    float pivetlx;
    float pivetly;
    float pivetTheta = M_PI * abs(bookParameter) / 2.0;
    float pivetHeight = 3 - 3 * abs(bookParameter);
    pivetrx = -5 * cos(pivetTheta) + pivetHeight * sin(pivetTheta) + 5;
    pivetry = 5 * sin(pivetTheta) + pivetHeight * cos(pivetTheta);
    pivetlx = 5 * cos(pivetTheta) - pivetHeight * sin(pivetTheta) - 5;
    pivetly = 5 * sin(pivetTheta) + pivetHeight * cos(pivetTheta);

    // pages right
    float ux[] = {0, 0, 0, 0, 25};
    float uy[] = {0, 0, 0, 0, 5.5};
    float uz[n] = {0};
    float vz[] = {-50, -50, -50, -50, -50};
    if (abs(bookParameter) != 1) {
        for (int i = 0; i < 4; i++) {
            ux[i] = (pivetrx-5) * cos(a*i) + 5;
            uy[i] = ((5.5-pivetry) / (5-pivetrx)) * (-1 * (pivetrx-5) * sin(a*i)) + pivetry;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            ux[i] = pivetrx;
            uy[i] = pivetry;
        }
    }
    ux[4] = ux[4] + abs(bookParameter)*(35-ux[4]);

    // pages left
    float sx[] = {0, 0, 0, 0, -25};
    float sy[] = {0, 0, 0, 0, 5.5};
    float sz[n] = {0};
    float tz[] = {-50, -50, -50, -50, -50};
    if (abs(bookParameter) != 1) {
        for (int i = 0; i < 4; i++) {
            sx[i] = (pivetlx + 5) * cos(a * i) - 5;
            sy[i] = ((5.5-pivetly) / (-5-pivetlx)) * (-1 * (pivetlx+5) * sin(a*i)) + pivetly;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            sx[i] = pivetlx;
            sy[i] = pivetly;
        }
    }
    sx[4] = sx[4] - abs(bookParameter)*(35+sx[4]);

    if (bookParameter < 0) {
        float theta = -1* bookParameter * M_PI/2.0;

        // draw left pages
        glPushMatrix();
        glTranslatef(5-10*cos(theta), 10*sin(theta), 0);
        glRotatef(-2*(theta*180/M_PI), 0, 0, 1);
        glTranslatef(5, 0, 0);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < 4; i++) {
            glTexCoord2f( 0.5 - 0.1*i/3.0, 0.01);        glVertex3f(sx[i], sy[i], sz[i]);
            if (i>0) normal(sx[i-1], sy[i-1], tz[i-1], sx[i], sy[i], tz[i], sx[i], sy[i], sz[i]);
            glTexCoord2f( 0.5 - 0.1*i/3.0, 0.99);        glVertex3f(sx[i], sy[i], tz[i]);
        }
        for (int i = 4; i < 5; i++) {
            glTexCoord2f( 0.01, 0.01);        glVertex3f(sx[i], sy[i], sz[i]);
            if (i>0) normal(sx[i-1], sy[i-1], tz[i-1], sx[i], sy[i], tz[i], sx[i], sy[i], sz[i]);
            glTexCoord2f( 0.01, 0.99);        glVertex3f(sx[i], sy[i], tz[i]);
        }
        glEnd();
        glPopMatrix();

        // draw right pages
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < 4; i++) {
            glTexCoord2f( 0.5 + 0.1*i/3.0, 0.01);        glVertex3f(ux[i], uy[i], uz[i]);
            if (i>0) normal(ux[i-1], uy[i-1], vz[i-1], ux[i], uy[i], uz[i], ux[i], uy[i], vz[i]);
            glTexCoord2f( 0.5 + 0.1*i/3.0, 0.99);        glVertex3f(ux[i], uy[i], vz[i]);
        }
        for (int i = 4; i < 5; i++) {
            glTexCoord2f( 0.99, 0.01);        glVertex3f(ux[i], uy[i], uz[i]);
            if (i>0) normal(ux[i-1], uy[i-1], vz[i-1], ux[i], uy[i], uz[i], ux[i], uy[i], vz[i]);
            glTexCoord2f( 0.99, 0.99);        glVertex3f(ux[i], uy[i], vz[i]);
        }
        glEnd();
    } else {
        float theta = bookParameter * M_PI/2.0 + 2*M_PI;

        // draw left pages
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < 4; i++) {
            glTexCoord2f( 0.5 - 0.1*i/3.0, 0.01);        glVertex3f(sx[i], sy[i], sz[i]);
            if (i>0) normal(sx[i-1], sy[i-1], tz[i-1], sx[i], sy[i], tz[i], sx[i], sy[i], sz[i]);
            glTexCoord2f( 0.5 - 0.1*i/3.0, 0.99);        glVertex3f(sx[i], sy[i], tz[i]);
        }
        for (int i = 4; i < 5; i++) {
            glTexCoord2f( 0.01, 0.01);        glVertex3f(sx[i], sy[i], sz[i]);
            if (i>0) normal(sx[i-1], sy[i-1], tz[i-1], sx[i], sy[i], tz[i], sx[i], sy[i], sz[i]);
            glTexCoord2f( 0.01, 0.99);        glVertex3f(sx[i], sy[i], tz[i]);
        }
        glEnd();

        // draw right pages
        glPushMatrix();
            glTranslatef(-5+10*cos(theta), 10*sin(theta), 0);
            glRotatef(2*(theta*180/M_PI), 0, 0, 1);
            glTranslatef(-5, 0, 0);

            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i < 4; i++) {
                glTexCoord2f( 0.5 + 0.1*i/3.0, 0.01);    glVertex3f(ux[i], uy[i], uz[i]);
                if (i>0) normal(ux[i-1], uy[i-1], vz[i-1], ux[i], uy[i], uz[i], ux[i], uy[i], vz[i]);
                glTexCoord2f( 0.5 + 0.1*i/3.0, 0.99);    glVertex3f(ux[i], uy[i], vz[i]);
            }
            for (int i = 4; i < 5; i++) {
                glTexCoord2f( 0.99, 0.01);               glVertex3f(ux[i], uy[i], uz[i]);
                if (i>0) normal(ux[i-1], uy[i-1], vz[i-1], ux[i], uy[i], uz[i], ux[i], uy[i], vz[i]);
                glTexCoord2f( 0.99, 0.99);               glVertex3f(ux[i], uy[i], vz[i]);
            }
            glEnd();
        glPopMatrix();
    }
}

void makeCover(float bookParameter)
{
    glColor3f(1, 0.8, 0.8);
    glBindTexture(GL_TEXTURE_2D, txId[9]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    float origx[] = { -35, -5, -3, -1, 0, 1, 3, 5, 35 }; // book lying flat
    float origy[] = { 0, 0, -1.4, -1.9, -2, -1.9, -1.4, 0, 0 };
    float vx[9] = {0};
    float vy[9] = {0};
    float vz[9] = {0};
    float wz[] = {-50, -50, -50, -50, -50, -50, -50, -50, -50};
    if (bookParameter < 0) {
        float theta = -1* bookParameter * M_PI/2.0;
        vx[8] = origx[8];
        vy[8] = origy[8];
        for (int i = 1; i < 8; i++) {
            vx[i] = 5 + (origx[i]-5) * cos(theta) + origy[i] * sin(theta);
            vy[i] = -1*(origx[i]-5) * sin(theta) + origy[i] * cos(theta);
        }
        vx[0] = vx[1] - 30 * cos(2 * theta);
        vy[0] = vy[1] + 30 * sin(2 * theta);
    } else {
        float theta = bookParameter * M_PI/2.0 + 2*M_PI;
        vx[0] = origx[0];
        vy[0] = origy[0];
        for (int i = 1; i < 8; i++) {
            vx[i] = -5 + (origx[i]+5) * cos(theta) - origy[i] * sin(theta);
            vy[i] = (origx[i]+5) * sin(theta) + origy[i] * cos(theta);
        }
        vx[8] = vx[7] + 30 * cos(2 * theta);
        vy[8] = vy[7] + 30 * sin(2 * theta);
    }

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 1; i++) {
        glTexCoord2f( 0.99, 0.01);    glVertex3f(vx[i], vy[i], vz[i]);
        if (i>0) normal(vx[i-1], vy[i-1], wz[i-1],vx[i], vy[i], wz[i], vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.99, 0.99);    glVertex3f(vx[i], vy[i], wz[i]);
    }
    for (int i = 1; i < 8; i++) {
        glTexCoord2f( 0.58 - 0.21*(i-1)/8.0, 0.01);    glVertex3f(vx[i], vy[i], vz[i]);
        if (i>0) normal(vx[i-1], vy[i-1], wz[i-1],vx[i], vy[i], wz[i], vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.58 - 0.21*(i-1)/8.0, 0.99);    glVertex3f(vx[i], vy[i], wz[i]);
    }
    for (int i = 8; i < 9; i++) {
        glTexCoord2f( 0.01, 0.01);    glVertex3f(vx[i], vy[i], vz[i]);
        if (i>0) normal(vx[i-1], vy[i-1], wz[i-1],vx[i], vy[i], wz[i], vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.01, 0.99);    glVertex3f(vx[i], vy[i], wz[i]);
    }
    glEnd();
}

void makePage(float pageParameter)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, txId[10]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    float vx[6] = { 0 };
    float vy[6] = { 0 };
    float vz[6] = { 0 };
    if (pageParameter > 0) {
        for (int i = 0; i < 5; i++) {
            float theta = M_PI / 2 * (pageParameter * i / 4);
            vx[i] = 5 * cos(theta) - 5;
            vy[i] = 3 + 2.5 * sin(theta);
        }
        vx[5] = vx[4] - (20+(5-5*pageParameter)) * sin(pageParameter * M_PI / 2);
        vy[5] = vy[4] + (20+(5-5*pageParameter)) * cos(pageParameter * M_PI / 2);
    } else {
        for (int i = 0; i < 5; i++) {
            float theta = M_PI / 2 * (pageParameter * i / 4);
            vx[i] = -5 * cos(theta) + 5;
            vy[i] = 3 - 2.5 * sin(theta);
        }
        vx[5] = vx[4] - (20+(5-5*abs(pageParameter))) * sin(pageParameter * M_PI / 2);
        vy[5] = vy[4] + (20+(5-5*abs(pageParameter))) * cos(pageParameter * M_PI / 2);
    }
    float wz[] = { -50, -50, -50, -50, -50, -50, -50, -50, -50 };

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 5; i++) {
        glTexCoord2f( 0.5 + 0.1*i/4.0, 0.01);       glVertex3f(vx[i], vy[i], vz[i]);
        if (i>0) normal(vx[i-1], vy[i-1], wz[i-1], vx[i], vy[i], wz[i], vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.5 + 0.1*i/4.0, 0.99);       glVertex3f(vx[i], vy[i], wz[i]);
    }
    for (int i = 5; i < 6; i++) {
        glTexCoord2f( 0.99, 0.01);    glVertex3f(vx[i], vy[i], vz[i]);
        if (i>0) normal(vx[i-1], vy[i-1], wz[i-1], vx[i], vy[i], wz[i], vx[i], vy[i], vz[i]);
        glTexCoord2f( 0.99, 0.99);
        glVertex3f(vx[i], vy[i], wz[i]);
    }
    glEnd();
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void bookPodium()
{
    glColor3f(0.7, 0.7, 0.7);
    glPushMatrix();
        glTranslatef(115, 0, 180);
        glPushMatrix();
            glTranslatef(0, 14, 0);     //top
            glScalef(35, 1, 60);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 6, 0);      //middle
            glScalef(30, 15, 55);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 0.5, 0);      //bottom
            glScalef(35, 1, 60);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(6, 23, 0);      //angled section
            glRotatef(30, 0,0, 1);
            glScalef(40, 2, 55);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

void book()
{
    glEnable(GL_TEXTURE_2D);

    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glPushMatrix();
        glTranslatef(100, 16, 180);
        glRotatef(-90, 0, 1, 0);
        glRotatef(30, 1, 0, 0);
        glScalef(0.7, 0.7, 0.7);

        makeSidesBook(bookParameter);                       //side of book
        makeFrontBook(bookParameter);                       //front of book
        glPushMatrix();
            glTranslatef(0,0,-50);
            glRotatef(180, 0, 1, 0);
            makeFrontBook(-1*bookParameter);  //back of book
        glPopMatrix();
        makePages(bookParameter);
        makeCover(bookParameter);                           //front of book
        glPushMatrix();
            glTranslatef(0,0.1,0);
            if (showPage) makePage(pageParameter);
        glPopMatrix();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);

    bookPodium();
}

typedef struct vertex {
    float x, y, z;
} Vertex;

Vertex midpoint(Vertex A, Vertex B)
{
    return {(A.x + B.x)/2.0f, (A.y + B.y)/2.0f, (A.z + B.z)/2.0f};
}

void pyramid(Vertex A, Vertex B, Vertex C, Vertex D)
{
    glBegin(GL_TRIANGLES);
        normal(A.x, A.y, A.z, C.x, C.y, C.z, B.x, B.y, B.z);
        glVertex3f(A.x, A.y, A.z);
        glVertex3f(B.x, B.y, B.z);
        glVertex3f(C.x, C.y, C.z);

        normal(A.x, A.y, A.z, D.x, D.y, D.z, B.x, B.y, B.z);
        glVertex3f(A.x, A.y, A.z);
        glVertex3f(B.x, B.y, B.z);
        glVertex3f(D.x, D.y, D.z);

        normal(B.x, B.y, B.z, D.x, D.y, D.z, C.x, C.y, C.z);
        glVertex3f(B.x, B.y, B.z);
        glVertex3f(C.x, C.y, C.z);
        glVertex3f(D.x, D.y, D.z);

        normal(A.x, A.y, A.z, D.x, D.y, D.z, C.x, C.y, C.z);
        glVertex3f(A.x, A.y, A.z);
        glVertex3f(C.x, C.y, C.z);
        glVertex3f(D.x, D.y, D.z);
    glEnd();
}

void multiply(Vertex A, Vertex B, Vertex C, Vertex D, int level)
{
    if (level == 1) {
        pyramid(A, midpoint(A, B), midpoint(A, C), midpoint(A, D));
        pyramid(midpoint(A, B), B, midpoint(B, C), midpoint(B, D));
        pyramid(midpoint(A, C), midpoint(B, C), C, midpoint(C, D));
        pyramid(midpoint(A, D), midpoint(B, D), midpoint(C, D), D);
    } else {
        multiply(A, midpoint(A, B), midpoint(A, C), midpoint(A, D), level - 1);
        multiply(midpoint(A, B), B, midpoint(B, C), midpoint(B, D), level - 1);
        multiply(midpoint(A, C), midpoint(B, C), C, midpoint(C, D), level - 1);
        multiply(midpoint(A, D), midpoint(B, D), midpoint(C, D), D, level - 1);
    }
}

void fractalPodium()
{
    glColor3f(0.7, 0.7, 0.7);
    glPushMatrix();
        glTranslatef(-110, 0, 180);
        glPushMatrix();
            glTranslatef(0, 14, 0);     //top
            glScalef(50, 1, 50);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 6, 0);      //middle
            glScalef(45, 15, 45);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 0.5, 0);      //bottom
            glScalef(50, 1, 50);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

void fractal()
{
    Vertex A = {sqrt(8.0f/9.0f), 0.0, -1.0/3.0};
    Vertex B = {-sqrt(2.0f/9.0f), -sqrt(2.0f/3.0f), -1.0/3.0};
    Vertex C = {-sqrt(2.0f/9.0f), sqrt(2.0f/3.0f), -1.0/3.0};
    Vertex D = {0.0, 0.0, 1.0};

    glColor3f(218.0/255, 165.0/255, 32.0/255); // gold
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glDisable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    glPushMatrix();
        glTranslatef(-110, 32 + fractalHeight, 179);
        glRotatef(fractalAngle, 0, 1, 0);
        glRotatef(-90, 1, 0, 0);
        glScalef(30,30,30);
        multiply(A, B, C, D, 6);
    glPopMatrix();

    fractalPodium();

    glEnable(GL_LIGHT3);
    glDisable(GL_LIGHT4);
}

void museum()
{
    float width = 300;
    float innerWidth = 238;
    float depth = 250;
    float height = 75;

    //outside
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glVertex3f(-width/2.0, height, depth);  //front
    glVertex3f(-width/2.0, 0, depth);
    glVertex3f(width/2.0, 0, depth);
    glVertex3f(width/2.0, height, depth);

    glNormal3f(-1,0,0);
    glVertex3f(-width/2.0, height, depth);  //left
    glVertex3f(-width/2.0, 0, depth);
    glVertex3f(-width/2.0, 0, 50);
    glVertex3f(-width/2.0, height, 50);

    glNormal3f(1,0,0);
    glVertex3f(width/2.0, height, depth);   //right
    glVertex3f(width/2.0, 0, depth);
    glVertex3f(width/2.0, 0, 50);
    glVertex3f(width/2.0, height, 50);

    glNormal3f(0,0,-1);
    glVertex3f(-width/2.0, height, 50);  //back left
    glVertex3f(-width/2.0, 0, 50);
    glVertex3f(-innerWidth/2.0, 0, 50);
    glVertex3f(-innerWidth/2.0, height, 50);

    glNormal3f(0,0,-1);
    glVertex3f(innerWidth/2.0, height, 50);  //back right
    glVertex3f(innerWidth/2.0, 0, 50);
    glVertex3f(width/2.0, 0, 50);
    glVertex3f(width/2.0, height, 50);

    glNormal3f(0,1,0);
    glVertex3f(-width/2.0, height, 50);  //roof
    glVertex3f(-width/2.0, height, depth);
    glVertex3f(width/2.0, height, depth);
    glVertex3f(width/2.0, height, 50);
    glEnd();

    //inside
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT3);
    glColor3f(133.0/255, 94.0/255, 66.0/255); // brown
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glVertex3f(-width/2.0 +1, height -1, depth -1);  //front
    glVertex3f(-width/2.0 +1, 0, depth -1);
    glVertex3f(width/2.0 -1, 0, depth -1);
    glVertex3f(width/2.0 -1, height -1, depth -1);

    glNormal3f(1,0,0);
    glVertex3f(-width/2.0 +1, height -1, depth -1);  //left
    glVertex3f(-width/2.0 +1, 0, depth -1);
    glVertex3f(-width/2.0 +1, 0, 50 +1);
    glVertex3f(-width/2.0 +1, height -1, 50 +1);

    glNormal3f(-1,0,0);
    glVertex3f(width/2.0 -1, height -1, depth -1);   //right
    glVertex3f(width/2.0 -1, 0, depth -1);
    glVertex3f(width/2.0 -1, 0, 50 +1);
    glVertex3f(width/2.0 -1, height -1, 50 +1);

    glNormal3f(0,0,1);
    glVertex3f(-width/2.0 +1, height -1, 50 +1);  //back left
    glVertex3f(-width/2.0 +1, 0, 50 +1);
    glVertex3f(-innerWidth/2.0, 0, 50 +1);
    glVertex3f(-innerWidth/2.0, height -1, 50 +1);

    glNormal3f(0,0,1);
    glVertex3f(innerWidth/2.0, height -1, 50 +1);  //back right
    glVertex3f(innerWidth/2.0, 0, 50 +1);
    glVertex3f(width/2.0 -1, 0, 50 +1);
    glVertex3f(width/2.0 -1, height -1, 50 +1);

    glNormal3f(0,-1,0);
    glVertex3f(-width/2.0 +1, height -1, 50 +1);  //roof
    glVertex3f(-width/2.0 +1, height -1, depth -1);
    glVertex3f(width/2.0 -1, height -1, depth -1);
    glVertex3f(width/2.0 -1, height -1, 50 +1);
    glEnd();

    // door
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glColor3f(0.8, 0.7, 0.3);   //replace with a texture
    glBindTexture(GL_TEXTURE_2D, txId[8]);
    glBegin(GL_QUADS);
    glTexCoord2f(0., 0.);     glVertex3f(-30, 65, depth+1);
    glTexCoord2f(0., 1.);     glVertex3f(-30, 0, depth+1);
    glTexCoord2f(1, 1.);    glVertex3f(30, 0, depth+1);
    glTexCoord2f(1, 0.);    glVertex3f(30, 65, depth+1);

    glTexCoord2f(0., 0.);     glVertex3f(-30, 65, depth-2);
    glTexCoord2f(0., 1.);     glVertex3f(-30, 0, depth-2);
    glTexCoord2f(1, 1.);    glVertex3f(30, 0, depth-2);
    glTexCoord2f(1, 0.);    glVertex3f(30, 65, depth-2);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    book();
    fractal();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT3);
}

void shadows()
{
    float orrerySize = 130;
    float width = 300;
    float innerWidth = 238;
    float depth = 250;
    float height = 75;

    glDisable(GL_LIGHTING);
    glEnable(GL_STENCIL_TEST);
    glColorMask(false, false, false, false);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    //draw circle
    glPushMatrix();
    glTranslatef(0, 0.1, 0);
    glRotatef(-90.0, 1., 0., 0.);
    gluDisk(q, 0.0, orrerySize, 20, 3);
    glPopMatrix();

    glColorMask(true, true, true, true);
    glStencilFunc(GL_NOTEQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // draw orrery shadow
    glPushMatrix();
        glTranslatef(0,0.1,0);
        glMultMatrixf(shadowMat);
        glColor4f(0.2, 0.2, 0.2, 1.0);

        glPushMatrix();
            glRotatef(-90.0, 1., 0., 0.);
            gluCylinder(q, orrerySize + 1, orrerySize + 1, 100, 20, 5);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0, 99, 0);
            glScalef(orrerySize-1, orrerySize-1, orrerySize-1);
            glRotatef(90.0, 0., 0., 1.);
            // draw hemisphere
            for(int indx = 0; indx < nface; indx++)		//draw each face
            {
                if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
                else				glBegin(GL_QUADS);
                glNormal3f(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
                glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
                glNormal3f(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
                glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
                glNormal3f(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
                glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
                if(nv[indx]==4)
                    glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
                glEnd();
            }
        glPopMatrix();

        // draw museum shadow
        glBegin(GL_QUADS);
        glVertex3f(-width/2.0, height, 50);  //back left
        glVertex3f(-width/2.0, 0, 50);
        glVertex3f(-innerWidth/2.0, 0, 50);
        glVertex3f(-innerWidth/2.0, height, 50);

        glVertex3f(innerWidth/2.0, height, 50);  //back right
        glVertex3f(innerWidth/2.0, 0, 50);
        glVertex3f(width/2.0, 0, 50);
        glVertex3f(width/2.0, height, 50);

        glVertex3f(-width/2.0, height, depth);  //left
        glVertex3f(-width/2.0, 0, depth);
        glVertex3f(-width/2.0, 0, 50);
        glVertex3f(-width/2.0, height, 50);
        glEnd();
    glPopMatrix();


    glDisable(GL_STENCIL_TEST);
    glEnable(GL_LIGHTING);
}
//--------------------------------------------------------------------------------

void initialise()
{
    q = gluNewQuadric(); //needed for cylinders
    gluQuadricTexture(q, GL_TRUE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 0);

    //lighting
    glEnable(GL_LIGHTING);

    // normal outside light from the sun
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    // light from the model sun only inside the orrery
    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);

    // spotlight from the camera, only on the sun model, to make the sun always lit
    glLightfv(GL_LIGHT2, GL_AMBIENT, black);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, yellow);
    glLightfv(GL_LIGHT2, GL_SPECULAR, yellow);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 90.0);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT,0.01);

    // light inside the museum main room
    glLightfv(GL_LIGHT3, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT3, GL_SPECULAR, white);

    // light for the fractal
    glLightfv(GL_LIGHT4, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT4, GL_SPECULAR, white);

    loadTexture();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    gluQuadricDrawStyle(q, GLU_FILL);  // needed for cylinders
    glClearColor(0.7, 1., 1., 1.);    //Background colour

    float model_wid, model_hgt;
    loadMeshFile("Hemisphere.off");			//Specify mesh file name here
    computeMinMax();						    //Compute min, max values of x, y coordinates for defining camera frustum
    model_wid = xmax-xmin;						//Model width and height
    model_hgt = ymax-ymin;
    xmin -= 0.2*model_wid;						//Extend minmax window further by 20% of its size.
    xmax += 0.2*model_wid;
    ymin -= 0.2*model_hgt;
    ymax += 0.2*model_hgt;

    // camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-5.0, 5.0, -5.0, 5.0, 10.0, 1000.0); // left, right, bottom, top, near, far
    gluPerspective(70., 1., 1., 7000.); // field of view angle, aspect, near, far
}

//--------------------------------------------------------------------------------

void display()
{
    float lgt_pos[] = {300.0f, 200.0f, 800.0f, 1.0f};
    float sun_pos[] = {0.0f, 35.0f, 0.0f, 1.0f};
    float spotlight_pos[] = {eye_x, height, eye_z, 1.0f};
    float spotlight_dir[] = {look_x - eye_x, 0, look_z - eye_z, 0.0f};  //spotlight direction
    float museum_light_pos[] = {115, 95, 180, 1.0f};
    float fractal_light_pos[] = {-110, 95, 180, 1.0f};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye_x, height, eye_z,  look_x, height, look_z,   0, 1, 0); // position, look vector, up vector
    glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, sun_pos);
    glLightfv(GL_LIGHT2, GL_POSITION, spotlight_pos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotlight_dir);   //spotlight position
    glLightfv(GL_LIGHT3, GL_POSITION, museum_light_pos);
    glLightfv(GL_LIGHT4, GL_POSITION, fractal_light_pos);

    skybox();
    floor();
    museum();
    orrery();
    shadows();
//    molecule();

    glutSwapBuffers();
}

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH | GLUT_MULTISAMPLE ); // display buffers
   glutInitWindowSize (glutGet(GLUT_SCREEN_HEIGHT), glutGet(GLUT_SCREEN_HEIGHT));
   glutInitWindowPosition ((glutGet(GLUT_SCREEN_WIDTH)-glutGet(GLUT_SCREEN_HEIGHT))/2.0, 0);
   glutCreateWindow ("Museum");
   initialise();
   look_x = eye_x + 100*sin(cameraAngle);
   look_z = eye_z - 100*cos(cameraAngle);

   glutDisplayFunc(display);
   glutTimerFunc(50, myTimer, 0);
   glutSpecialFunc(special);
   glutMouseFunc(mouse);

   glutMainLoop();
   return 0;
}
