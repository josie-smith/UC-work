/*==================================================================================
* COSC363 Computer Graphics (2020) - Ray Tracer
* Assignment 2
* Josie Smith
*===================================================================================
*/

#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Torus.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>
using namespace std;

const float WIDTH = 20.0;           // width and height of the image plane in world units
const float HEIGHT = 20.0;
const float EDIST = 40.0;           // distance of the image plane from the camera origin
const int NUMDIV = 500;             // number of cells along x and y directions
const int MAX_STEPS = 5;            // number of levels of recursion
const float XMIN = -WIDTH * 0.5;    // boundary values of the image plane in terms of WIDTH and HEIGHT, with view axis passing through the centre
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;
TextureBMP textureBackWall;
TextureBMP textureWall;
TextureBMP textureWitch;
TextureBMP textureFringe;


//---The most important function in a ray tracer! ----------------------------------
// Computes the colour value obtained by tracing a ray and finding its
// closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);				//Background colour = (0,0,0)
    glm::vec3 lightPos1(-30, 30, -100);	//Light's position
    glm::vec3 lightPos2(-40, 15, -70);	//Light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);				//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

    if (ray.index == 0) {                           // Floor is checkered
        int stripeWidth = 5;
        int ix = (ray.hit.x + 100) / stripeWidth;
        int iz = (ray.hit.z) / stripeWidth;
        int j = ix % 2;
        int k = iz % 2;
        if (j == 0) {
            if (k == 0) color = glm::vec3(0.33, 0.06, 0.1);
            else color = glm::vec3(0.36, 0.25, 0.2);
        } else {
            if (k != 0) color = glm::vec3(0.33, 0.06, 0.1);
            else color = glm::vec3(0.36, 0.25, 0.2);
        }
        obj->setColor(color);

    } else if (ray.index == 1)                      //If the ray hits the back wall, set texture
    {
        float texcoords = (ray.hit.x +60)/(120);
        float texcoordt = (ray.hit.y +20)/(60);
        if (texcoords > 0 && texcoords < 1 &&
           texcoordt > 0 && texcoordt < 1)
        {
            color=textureBackWall.getColorAt(texcoords, texcoordt);
            obj->setColor(color);
        }
    } else if (ray.index == 2 || ray.index == 3)    //If the ray hits the left or right wall, set texture
    {
        float texcoords = (ray.hit.z +140)/(100);
        float texcoordt = (ray.hit.y +20)/(60);
        if (texcoords > 0 && texcoords < 1 &&
            texcoordt > 0 && texcoordt < 1)
        {
            color=textureWall.getColorAt(texcoords, texcoordt);
            obj->setColor(color);
        }
    } else if (ray.index == 4 && step != 1)         //If the ray hits the front wall, set texture
    {
        float texcoords = (ray.hit.x + 60) / (120);
        float texcoordt = (ray.hit.y + 20) / (60);
        if (texcoords > 0 && texcoords < 1 &&
            texcoordt > 0 && texcoordt < 1) {
            color = textureWitch.getColorAt(texcoords, texcoordt);
            obj->setColor(color);
        }
    } else if (ray.index == 5)                      //If the ray hits the table, set texture
    {
        if (ray.hit.y < -10) {
            float texcoords = fmod((acos(ray.hit.x * 0.05) / M_PI)*5 , 1.0);
            float texcoordt = (ray.hit.y + 13.5) * 0.33;
            if (texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {
                color = textureFringe.getColorAt(texcoords, texcoordt);
                obj->setColor(color);
            }
        }
    }

    // transparent parts on the fringe, or the front wall is invisible for the first ray only
    if ( (ray.index == 5 && obj->getColor().g < 0.1) || (ray.index == 4 && step == 1) ) {
        Ray passThroughRay(ray.hit, ray.dir);
        glm::vec3 passThroughColor = trace(passThroughRay, step + 1);
        color = passThroughColor;
    } else {
        color = obj->lighting(lightPos1, lightPos2, -ray.dir, ray.hit);
    }

    glm::vec3 lightVec1 = lightPos1 - ray.hit;
    glm::vec3 lightVec2 = lightPos2 - ray.hit;
    Ray shadowRay1(ray.hit, lightVec1);             //Generate shadow ray 1
    Ray shadowRay2(ray.hit, lightVec2);             //Generate shadow ray 2
    shadowRay1.closestPt(sceneObjects);
    shadowRay2.closestPt(sceneObjects);
    if ( !(ray.index == 5 && obj->getColor().g < 0.1) && ray.index != 4) {
        if (shadowRay1.index > -1 && shadowRay1.dist < glm::length(lightVec1)
                && shadowRay2.index > -1 && shadowRay2.dist < glm::length(lightVec2)
                && shadowRay1.index != 4 && shadowRay2.index != 4
                && !(ray.index == 5 && obj->getColor().g < 0.1) ) {
            color = 0.2f * obj->getColor();
            if (sceneObjects[shadowRay1.index]->isTransparent() || sceneObjects[shadowRay1.index]->isRefractive()) {
                float rho1 = sceneObjects[shadowRay1.index]->getTransparencyCoeff();
                if (sceneObjects[shadowRay2.index]->isTransparent() || sceneObjects[shadowRay2.index]->isRefractive()) {
                    float rho2 = sceneObjects[shadowRay2.index]->getTransparencyCoeff();
                    color += (rho2 + rho1)*0.5f * obj->getColor();  //0.2 = ambient scale factor
                } else {
                    color += rho1 * 0.8f * obj->getColor();         //0.2 = ambient scale factor
                }
            } else if (sceneObjects[shadowRay2.index]->isTransparent() || sceneObjects[shadowRay2.index]->isRefractive()) {
                float rho2 = sceneObjects[shadowRay2.index]->getTransparencyCoeff();
                color += rho2 * 0.8f * obj->getColor();             //0.2 = ambient scale factor
            }
        } else if (shadowRay1.index > -1 && shadowRay1.dist < glm::length(lightVec1)
                && shadowRay1.index != 4
                && !(ray.index == 5 && obj->getColor().g < 0.1)) {
            if (sceneObjects[shadowRay1.index]->isTransparent() || sceneObjects[shadowRay1.index]->isRefractive()) {
                float rho = sceneObjects[shadowRay1.index]->getTransparencyCoeff();
                color = rho * 0.8f * obj->getColor() + obj->lighting(lightPos2, -ray.dir, ray.hit);
            } else {
                color = obj->lighting(lightPos2, -ray.dir, ray.hit);
            }
        } else if (shadowRay2.index > -1 && shadowRay2.dist < glm::length(lightVec2)
                && shadowRay2.index != 4
                && !(ray.index == 5 && obj->getColor().g < 0.1) ) {
            if (sceneObjects[shadowRay2.index]->isTransparent() || sceneObjects[shadowRay2.index]->isRefractive()) {
                float rho = sceneObjects[shadowRay2.index]->getTransparencyCoeff();
                color = rho * 0.8f * obj->getColor() + obj->lighting(lightPos1, -ray.dir, ray.hit);
            } else {
                color = obj->lighting(lightPos1, -ray.dir, ray.hit);
            }
        }
    }

    if (obj->isReflective() && step < MAX_STEPS)    //Generate reflections
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }

    if (obj->isTransparent() && step < MAX_STEPS)    //Generate pass through rays
    {
        float rho = obj->getTransparencyCoeff();
        Ray passThroughRay(ray.hit, ray.dir);
        glm::vec3 passThroughColor = trace(passThroughRay, step + 1);
        color = (1-rho) * color + (rho * passThroughColor);
    }

    if (obj->isRefractive() && step < MAX_STEPS)     //Generate pass through rays
    {
        float rho = obj->getRefractionCoeff();
        float nu = obj->getRefractiveIndex();
        glm::vec3 n = obj->normal(ray.hit);

        glm::vec3 g = glm::refract(ray.dir, n, 1/nu);
        Ray refractedRay(ray.hit, g);

        refractedRay.closestPt(sceneObjects);
        glm::vec3 m = obj->normal(refractedRay.hit);
        glm::vec3 h = glm::refract(g, -m, nu);
        Ray passThroughRay(refractedRay.hit, h);

        glm::vec3 refractedColor = trace(passThroughRay, step + 1);
        color = color + (rho * refractedColor);
    }

    return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;                               //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;           //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;           //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);                          //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	            //Scan every cell of the image plane
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir1(xp+0.25*cellX, yp+0.25*cellY, -EDIST);	//direction of the 4 primary rays
            glm::vec3 dir2(xp+0.75*cellX, yp+0.25*cellY, -EDIST);
            glm::vec3 dir3(xp+0.25*cellX, yp+0.75*cellY, -EDIST);
            glm::vec3 dir4(xp+0.75*cellX, yp+0.75*cellY, -EDIST);
		    Ray ray1 = Ray(eye, dir1);
		    glm::vec3 col1 = trace (ray1, 1);   //Trace the primary ray and get the colour value
            Ray ray2 = Ray(eye, dir2);
            glm::vec3 col2 = trace (ray2, 1);   //Trace the primary ray and get the colour value
            Ray ray3 = Ray(eye, dir3);
            glm::vec3 col3 = trace (ray3, 1);   //Trace the primary ray and get the colour value
            Ray ray4 = Ray(eye, dir4);
            glm::vec3 col4 = trace (ray4, 1);   //Trace the primary ray and get the colour value
            glm::vec3 col = (col1 + col2 + col3 + col4)*0.25f;

//            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);
//            Ray ray = Ray(eye, dir);
//            glm::vec3 col = trace (ray, 1);   //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				          //Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


void octahedron(float s, glm::vec3 c)
{
    float coeff = 0.7;
    glm::vec3 color = glm::vec3(0.95, 0.77, 0);
    float height = 1.5;

    Plane *triangle1 = new Plane (glm::vec3(c.x, c.y, c.z + s),
                                  glm::vec3(c.x + s, c.y, c.z),
                                  glm::vec3(c.x, c.y + height*s, c.z));
    triangle1->setColor(color);
    triangle1->setTransparency(true, coeff);
    triangle1->setShininess(1);
    sceneObjects.push_back(triangle1);

    Plane *triangle2 = new Plane (glm::vec3(c.x + s, c.y, c.z),
                                  glm::vec3(c.x, c.y, c.z - s),
                                  glm::vec3(c.x, c.y + height*s, c.z));
    triangle2->setColor(color);
    triangle2->setTransparency(true, coeff);
    triangle2->setShininess(1);
    sceneObjects.push_back(triangle2);

    Plane *triangle3 = new Plane (glm::vec3(c.x, c.y, c.z - s),
                                  glm::vec3(c.x - s, c.y, c.z),
                                  glm::vec3(c.x, c.y + height*s, c.z));
    triangle3->setColor(color);
    triangle3->setTransparency(true, coeff);
    triangle3->setShininess(1);
    sceneObjects.push_back(triangle3);

    Plane *triangle4 = new Plane (glm::vec3(c.x - s, c.y, c.z),
                                  glm::vec3(c.x, c.y, c.z + s),
                                  glm::vec3(c.x, c.y + height*s, c.z));
    triangle4->setColor(color);
    triangle4->setTransparency(true, coeff);
    triangle4->setShininess(1);
    sceneObjects.push_back(triangle4);

    Plane *triangle5 = new Plane (glm::vec3(c.x, c.y, c.z + s),
                                  glm::vec3(c.x, c.y - height*s, c.z),
                                  glm::vec3(c.x + s, c.y, c.z));
    triangle5->setColor(color);
    triangle5->setTransparency(true, coeff);
    triangle5->setShininess(1);
    sceneObjects.push_back(triangle5);

    Plane *triangle6 = new Plane (glm::vec3(c.x + s, c.y, c.z),
                                  glm::vec3(c.x, c.y - height*s, c.z),
                                  glm::vec3(c.x, c.y, c.z - s));
    triangle6->setColor(color);
    triangle6->setTransparency(true, coeff);
    triangle6->setShininess(1);
    sceneObjects.push_back(triangle6);

    Plane *triangle7 = new Plane (glm::vec3(c.x, c.y, c.z - s),
                                  glm::vec3(c.x, c.y - height*s, c.z),
                                  glm::vec3(c.x - s, c.y, c.z));
    triangle7->setColor(color);
    triangle7->setTransparency(true, coeff);
    triangle7->setShininess(1);
    sceneObjects.push_back(triangle7);

    Plane *triangle8 = new Plane (glm::vec3(c.x - s, c.y, c.z),
                                  glm::vec3(c.x, c.y - height*s, c.z),
                                  glm::vec3(c.x, c.y, c.z + s));
    triangle8->setColor(color);
    triangle8->setTransparency(true, coeff);
    triangle8->setShininess(1);
    sceneObjects.push_back(triangle8);
}


//---This function initializes the scene -------------------------------------------
// Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
// and add them to the list of scene objects.
// It also initializes the OpenGL orthographic projection matrix for drawing the
// the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

    Plane *floor = new Plane (glm::vec3(-60., -20, -40),   //Point A
                              glm::vec3(60., -20, -40),    //Point B
                              glm::vec3(60., -20, -140),   //Point C
                              glm::vec3(-60., -20, -140)); //Point D
    floor->setSpecularity(false);
    sceneObjects.push_back(floor);

    Plane *backwall = new Plane (glm::vec3(-60., -20, -140),
                              glm::vec3(60., -20, -140),
                              glm::vec3(60., 40, -140),
                              glm::vec3(-60., 40, -140));
    backwall->setSpecularity(false);
    sceneObjects.push_back(backwall);

    Plane *leftwall = new Plane (glm::vec3(-60., -20, -40),
                                 glm::vec3(-60., -20, -140),
                                 glm::vec3(-60., 40, -140),
                                 glm::vec3(-60., 40, -40));
    leftwall->setSpecularity(false);
    sceneObjects.push_back(leftwall);

    Plane *rightwall = new Plane (glm::vec3(60., -20, -140),
                                  glm::vec3(60., -20, -40),
                                  glm::vec3(60., 40, -40),
                                  glm::vec3(60., 40, -140));
    rightwall->setSpecularity(false);
    sceneObjects.push_back(rightwall);

    Plane *frontwall = new Plane (glm::vec3(60., -20, -69),
                                  glm::vec3(-60., -20, -69),
                                  glm::vec3(-60., 40, -69),
                                  glm::vec3(60., 40, -69));
    frontwall->setSpecularity(false);
    sceneObjects.push_back(frontwall);

    Cylinder *table = new Cylinder (glm::vec3(0, -13.5, -90), 20, 3.5);
    table->setColor(glm::vec3(0.0, 0.27, 0.13));        //Set colour to green
    sceneObjects.push_back(table);

    Cylinder *tableLeg = new Cylinder (glm::vec3(0, -20, -90), 1, 9);
    tableLeg->setColor(glm::vec3(0.70, 0.39, 0.21));    //Set colour to gold
    tableLeg->setShininess(1);
    tableLeg->setReflectivity(true, 0.2);
    sceneObjects.push_back(tableLeg);

    Cone *tableStand = new Cone (glm::vec3(0, -20, -90), 16, 4);
    tableStand->setColor(glm::vec3(0.70, 0.39, 0.21));  //Set colour to gold
    tableStand->setShininess(10);
    tableStand->setReflectivity(true, 0.2);
    sceneObjects.push_back(tableStand);

	Sphere *sphere = new Sphere(glm::vec3(0.0, 3.0, -90.0), 12.0);
	sphere->setColor(glm::vec3(0.05, 0.05, 0.05));      //Set colour to black
    sphere->setReflectivity(true, 0.9);
    sphere->setRefractivity(true, 1, 1.01);
	sceneObjects.push_back(sphere);

    Cylinder *candle1 = new Cylinder (glm::vec3(-16, -12, -85), 2, 10);
    candle1->setColor(glm::vec3(0.87, 0.72, 0.51));     //Set colour to cream
    candle1->setSpecularity(false);
    sceneObjects.push_back(candle1);

    Cylinder *candle2 = new Cylinder (glm::vec3(-13, -12, -92), 1.5, 7);
    candle2->setColor(glm::vec3(0.87, 0.72, 0.51));     //Set colour to cream
    candle2->setSpecularity(false);
    sceneObjects.push_back(candle2);

    octahedron(1.8, glm::vec3(-16, 2, -85));
    octahedron(1.2, glm::vec3(-13, -2, -92));

    Torus *stand = new Torus (glm::vec3(0, -9, -90), 4, 1);
    stand->setColor(glm::vec3(0.50, 0.29, 0.11));       //Set colour to gold
    stand->setReflectivity(true, 0.2);
    sceneObjects.push_back(stand);

    textureBackWall = TextureBMP("curtains-long.bmp");
    textureWall = TextureBMP("curtains.bmp");
    textureWitch = TextureBMP("witch.bmp");
    textureFringe = TextureBMP("fringe.bmp");
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
