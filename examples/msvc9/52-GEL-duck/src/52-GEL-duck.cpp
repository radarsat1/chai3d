//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2009 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author    <http://www.chai3d.org>
    \author    Sonny Chan
    \author    Francois Conti
    \version   2.0.0 $Rev: 269 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <set>
#include <utility>
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------
#include "GEL3D.h"
//---------------------------------------------------------------------------
#include "tetgen.h"
#include "geometry.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------
// enable skeleton model
#define USE_SKELETON_MODEL 1

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 1024;
const int WINDOW_SIZE_H         = 768;

// mouse menu options (right button)
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;
const int OPTION_SHOWSKELETON   = 3;
const int OPTION_HIDESKELETON   = 4;

// TetGen switches (note should be const, but tetgen too dumb to handle that)
char TETGEN_SWITCHES[]    = "pq1.414a0.006";
char TETGEN_SWITCHES0[]   = "pq1.414";

//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

// camera position and orientation is spherical coordinates
double cameraAngleH;
double cameraAngleV;
double cameraDistance;
cVector3d cameraPosition;
bool flagCameraInMotion = false;

// mouse position and button status
int mouseX, mouseY;
int mouseButton;

// a light source to illuminate the objects in the virtual scene
cLight *light;

// a little "chai3d" bitmap logo at the bottom of the screen
cBitmap* logo;

// width and height of the current window display
int displayW  = 0;
int displayH  = 0;

// a haptic device handler
cHapticDeviceHandler* handler;

// a haptic device
cGenericHapticDevice* hapticDevice;

// force scale factor
double deviceForceScale;

// scale factor between the device workspace and cursor workspace
double workspaceScaleFactor;

// desired workspace radius of the virtual cursor
double cursorWorkspaceRadius;

// status of the main simulation haptics loop
bool simulationRunning = false;

// simulation clock
cPrecisionClock simClock;
cPrecisionClock startHapticsClock;

// ground level
double level = -0.4;

// root resource path
string resourceRoot;

// has exited haptics simulation thread
bool simulationFinished = false;

//---------------------------------------------------------------------------
// DECLARED MACROS
//---------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//---------------------------------------------------------------------------
// GEL 3D
//---------------------------------------------------------------------------

// deformable world
cGELWorld* defWorld;

// water
cGELMesh* ground;

// object mesh
cGELMesh* defObject;

// haptic device  model
cShapeSphere* device;
double deviceRadius;

// radius of the dynamic model sphere (GEM)
double radius;

// stiffness properties between the haptic device tool and the model (GEM)
double stiffness;


//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// callback when the right mouse button is pressed to select a menu item
void menuSelect(int value);

// update position of virtual camera
void updateCameraPosition();

// mouse click callback
void mouseClick(int button, int state, int x, int y);

// mouse move callback
void mouseMove(int x, int y);

// function called before exiting the application
void close(void);

// main graphics callback
void updateGraphics(void);

// main haptics loop
void updateHaptics(void);

// compute forces between tool and environment
cVector3d computeForce(const cVector3d& a_cursor,
                       double a_cursorRadius,
                       const cVector3d& a_spherePos,
                       double a_radius,
                       double a_stiffness);

// create a spring based model for duck 1
bool createTetGenMesh(cGELMesh *a_object, char *a_filename, char *a_filenameHighRes);

// create a filling sphere skeleton model for duck 2
bool createSkeletonMesh(cGELMesh *a_object, char *a_filename, char *a_filenameHighRes);


//===========================================================================
/*
    DEMO:    GEM_duck.cpp

    This application illustrates the use of the GEM libraries to simulate
    deformable objects. In this example we present two approaches for
    simulating two deformable meshes, respectively a yellow and a green duck.
    The yellow model used a skeleton approach just like the membrane
    example (membrane.cpp). The second mesh (green duck) uses a simpler simple mass-spring
    model which links mesh vertices with linear springs

    In this example we load our polygonal surface model, use the TetGen
    implementations of the tetrahedralization algorithms to compute a solid
    mesh, and build our simulation data structures from the tetrahedralized
    output. In addition, TetGen gives us the capability to control both the
    maximum circumradius to shortest edge ratio and the maximum volume
    of the generated tetrahedra, which is very useful for controlling the
    number of elements created for any given input mesh.

    TetGen (http://tetgen.berlios.de/) is a quality tetrahedral mesh generator
    and threedimensional Delaunay triangulator created by Hang Si at the
    Weierstrass Institute for Applied Analysis and Stochastics. It provides
    the ability to compute an exact 3D Delaunay tetrahedralization and its dual
    Voronoi diagram from a set of 3D points. More importantly, it also has the
    capability to compute both a constrained and a conforming Delaunay
    tetrahedralization from a given piecewise linear complex (of which a
    polygonal surface is a special case).

    Haptic feedback is performed by computing at every integration step the
    interaction between the cursor (large sphere) with each mass-point or
    skeleton node composing the physical models of each respective body.
    The approach is not optimal for large models and will need to be improved
    in the future. However, performing point location on deformable bodies
    is a non trivial problem since the collision structure needs to be
    continuously updated as the body deforms.
*/
//===========================================================================

int main(int argc, char* argv[])
{
    //-----------------------------------------------------------------------
    // INITIALIZATION
    //-----------------------------------------------------------------------

    printf ("\n");
    printf ("-----------------------------------\n");
    printf ("CHAI 3D\n");
    printf ("Demo: 52-GEL-duck\n");
    printf ("Copyright 2003-2009\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
    printf ("[1] - Show GEL Skeleton\n");
    printf ("[2] - Hide GEL Skeleton\n");
    printf ("[x] - Exit application\n");
    printf ("\n\n");

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);


    //-----------------------------------------------------------------------
    // 3D - SCENEGRAPH
    //-----------------------------------------------------------------------

    // create a new world.
    world = new cWorld();

    // set the background color of the environment
    // the color is defined by its (R,G,B) components.
    world->setBackgroundColor(0.0, 0.0, 0.0);
    world->setBackgroundColor(1.0, 1.0, 1.0);

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and oriente the camera
    // define a default position of the camera (described in spherical coordinates)
    cameraAngleH = 0;
    cameraAngleV = 45;
    cameraDistance = 2.5;
    updateCameraPosition();

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // enable higher rendering quality because we are displaying transparent objects
    camera->enableMultipassTransparency(true);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam
    light->setDirectionalLight(false);
    light->m_ambient.set(0.5, 0.5, 0.5);
    light->m_diffuse.set(0.7, 0.7, 0.7);
    light->m_specular.set(0.9, 0.9, 0.9);


    //-----------------------------------------------------------------------
    // 2D - WIDGETS
    //-----------------------------------------------------------------------

    // create a 2D bitmap logo
    logo = new cBitmap();

    // add logo to the front plane
    camera->m_front_2Dscene.addChild(logo);

    // load a "chai3d" bitmap image file
	  bool fileload;
    fileload = logo->m_image.loadFromFile(RESOURCE_PATH("resources/images/chai3d.bmp"));
	  if (!fileload)
	  {
		  #if defined(_MSVC)
		  fileload = logo->m_image.loadFromFile("../../../bin/resources/images/chai3d.bmp");
		  #endif
	  }

    // position the logo at the bottom left of the screen (pixel coordinates)
    logo->setPos(10, 10, 0);

    // scale the logo along its horizontal and vertical axis
    logo->setZoomHV(0.4, 0.4);

    // here we replace all black pixels (0,0,0) of the logo bitmap
    // with transparent black pixels (0, 0, 0, 0). This allows us to make
    // the background of the logo look transparent.
    logo->m_image.replace(
                          cColorb(0, 0, 0),      // original RGB color
                          cColorb(0, 0, 0, 0)    // new RGBA color
                          );

    // enable transparency
    logo->enableTransparency(true);


    //-----------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //-----------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get access to the first available haptic device
    handler->getDevice(hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo info;
    if (hapticDevice)
    {
        hapticDevice->open();
        info = hapticDevice->getSpecifications();
    }

    // desired workspace radius of the cursor
    cursorWorkspaceRadius = 0.8;

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    workspaceScaleFactor = cursorWorkspaceRadius / info.m_workspaceRadius;

    // define a scale factor between the force perceived at the cursor and the
    // forces actually sent to the haptic device
    deviceForceScale = 0.005 * (info.m_maxForceStiffness / workspaceScaleFactor);

    // define a force scale factor for the current haptic device.
    const double FORCE_REFERENCE = 10.0;
    deviceForceScale = info.m_maxForce / FORCE_REFERENCE;

    // create a large sphere that represents the haptic device
    deviceRadius = 0.12;
    device = new cShapeSphere(deviceRadius);
    world->addChild(device);
    device->m_material.m_ambient.set(0.4, 0.4, 0.4, 0.7);
    device->m_material.m_diffuse.set(0.7, 0.7, 0.7, 0.7);
    device->m_material.m_specular.set(1.0, 1.0, 1.0, 0.7);
    device->m_material.setShininess(100);
    stiffness = 40;


    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------
    // create a world which supports deformable object
    defWorld = new cGELWorld();
    world->addChild(defWorld);
    world->setPos(-1.5, 0.0, -0.1);
    world->rotate(cVector3d(0,1,0), cDegToRad(30));


    //-----------------------------------------------------------------------
    // COMPOSE THE WATER
    //-----------------------------------------------------------------------
    ground = new cGELMesh(world);
    ground->m_useMassParticleModel = true;
    defWorld->m_gelMeshes.push_back(ground);

    cGELMassParticle::default_mass = 0.010;
    cGELMassParticle::default_kDampingPos = 0.4;
    cGELMassParticle::default_gravity.set(0,0,0);

    int u,v;
    int RESOLUTION = 15;
    double SIZE = 3.5;
    for (v=0; v<RESOLUTION; v++)
    {
        for (u=0; u<RESOLUTION; u++)
        {
            double px, py, tu, tv;

            // compute the position of the vertex
            px = SIZE / (double)RESOLUTION * (double)u - (SIZE/2.0);
            py = SIZE / (double)RESOLUTION * (double)v - (SIZE/2.0);

            // create new vertex
            unsigned int index = ground->newVertex(px, py, level);
            cVertex* vertex = ground->getVertex(index);

            // compute texture coordinate
            tu = (double)u / (double)RESOLUTION;
            tv = (double)v / (double)RESOLUTION;
            vertex->setTexCoord(tu, tv);
            vertex->setColor(cColorf(1.0, 0.0, 0.1));
        }
    }

    ground->buildVertices();

    for (v=0; v<RESOLUTION; v++)
    {
        for (u=0; u<RESOLUTION; u++)
        {
            if ((u == 0) || (v == 0) || (u == (RESOLUTION-1)) || (v == (RESOLUTION-1)))
            {
                // create new vertex
                unsigned int index = ((v + 0) * RESOLUTION) + (u + 0);
                ground->m_gelVertices[index].m_massParticle->m_fixed = true;
            }
        }
    }

    cGELLinearSpring::default_kSpringElongation = 10.0; // [N/m]

    // Create a triangle based map using the above pixels
     for (v=0; v<(RESOLUTION-1); v++)
    {
        for (u=0; u<(RESOLUTION-1); u++)
        {
            // get the indexing numbers of the next four vertices
            unsigned int index00 = ((v + 0) * RESOLUTION) + (u + 0);
            unsigned int index01 = ((v + 0) * RESOLUTION) + (u + 1);
            unsigned int index10 = ((v + 1) * RESOLUTION) + (u + 0);
            unsigned int index11 = ((v + 1) * RESOLUTION) + (u + 1);

            // create two new triangles
            ground->newTriangle(index00, index01, index10);
            ground->newTriangle(index10, index01, index11);

            cGELMassParticle* m0 = ground->m_gelVertices[index00].m_massParticle;
            cGELMassParticle* m1 = ground->m_gelVertices[index01].m_massParticle;
            cGELMassParticle* m2 = ground->m_gelVertices[index10].m_massParticle;

            cGELLinearSpring* spring0 = new cGELLinearSpring(m0, m1);
            cGELLinearSpring* spring1 = new cGELLinearSpring(m0, m2);
            ground->m_linearSprings.push_back(spring0);
            ground->m_linearSprings.push_back(spring1);
        }
    }

    double transparencyLevel = 0.7;
    ground->setUseMaterial(true);
    ground->m_material.m_ambient.set(0.6, 0.6, 0.6, transparencyLevel);
    ground->m_material.m_diffuse.set(0.8, 0.8, 0.8, transparencyLevel);
    ground->m_material.m_specular.set(0.9, 0.9, 0.9, transparencyLevel);
    ground->setTransparencyLevel(transparencyLevel);
    ground->setUseTransparency(true);
    cTexture2D* textureGround = new cTexture2D();
    ground->setTexture(textureGround);
    ground->setUseTexture(true, true);

    fileload = textureGround->loadFromFile(RESOURCE_PATH("resources/images/aqua.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = textureGround->loadFromFile("../../../bin/resources/images/aqua.bmp" );
        #endif
        if (!fileload)
        {
            printf("Error - 3D Texture failed to load correctly.\n");
            close();
            return (-1);
        }
    }

    //-----------------------------------------------------------------------
    // COMPOSE SOME REFLEXION
    //-----------------------------------------------------------------------

    cGenericObject* reflexion = new cGenericObject();
    world->addChild(reflexion);
    cMatrix3d rot;
    rot.set(1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, -1.0);
    reflexion->setRot(rot);
    reflexion->addChild(defWorld);
    reflexion->setPos(0.0, 0.0, 2.0 * level);
    cGenericObject* reflexionTool = new cGenericObject();
    reflexion->addChild(reflexionTool);
    reflexionTool->addChild(device);
    reflexionTool->setPos(0.0, 0.0, -0.5 * level);

    // Play with thse numbers carefully!
    defWorld->m_integrationTime = 0.001;

    // create a deformable mesh
    defObject = new cGELMesh(world);
    defWorld->m_gelMeshes.push_front(defObject);

    //-----------------------------------------------------------------------
    // BUILD THE DUCK!
    //-----------------------------------------------------------------------

    if (USE_SKELETON_MODEL)
    {
        fileload = createSkeletonMesh(defObject, RESOURCE_PATH("resources/models/ducky/duck-200.off"), RESOURCE_PATH("resources/models/ducky/duck-full.obj"));
	    if (!fileload)
	    {
		    #if defined(_MSVC)
		    fileload = createSkeletonMesh(defObject, "../../../bin/resources/models/ducky/duck-200.off", "../../../bin/resources/models/ducky/duck-full.obj");
            #endif
		    if (!fileload)
		    {
			    printf("Error - 3D Model failed to load correctly.\n");
			    close();
			    return (-1);
		    }
	    }
    }
    else
    {
	    fileload = createTetGenMesh(defObject, RESOURCE_PATH("resources/models/ducky/duck-1200.off"), RESOURCE_PATH("resources/models/ducky/duck-green.obj"));
	    if (!fileload)
	    {
		    #if defined(_MSVC)
		    fileload = createTetGenMesh(defObject, "../../../bin/resources/models/ducky/duck-1200.off", "../../../bin/resources/models/ducky/duck-green.obj");
            #endif
		    if (!fileload)
		    {
			    printf("Error - 3D Model failed to load correctly.\n");
			    close();
			    return (-1);
		    }
	    }
    }

    //-----------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //-----------------------------------------------------------------------

    // initialize GLUT
	  glutInit(&argc, argv);

    // retrieve the resolution of the computer display and estimate the position
    // of the GLUT window so that it is located at the center of the screen
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int windowPosX = (screenW - WINDOW_SIZE_W) / 2;
    int windowPosY = (screenH - WINDOW_SIZE_H) / 2;

    // initialize the OpenGL GLUT window
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(WINDOW_SIZE_W, WINDOW_SIZE_H);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("CS 268 - Sonny Chan & Francois Conti - March 2009");

    // create a mouse menu (right button)
    glutCreateMenu(menuSelect);
    glutAddMenuEntry("full screen", OPTION_FULLSCREEN);
    glutAddMenuEntry("window display", OPTION_WINDOWDISPLAY);
    glutAddMenuEntry("show skeleton", OPTION_SHOWSKELETON);
    glutAddMenuEntry("hide skeleton", OPTION_HIDESKELETON);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    //-----------------------------------------------------------------------
    // START SIMULATION
    //-----------------------------------------------------------------------

    // simulation in now running
    simulationRunning = true;

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

    // start the main graphics rendering loop
    glutMainLoop();

    // close everything
    close();

    // exit
    return (0);
}

//---------------------------------------------------------------------------

void updateCameraPosition()
{
    // check values
    if (cameraDistance < 0.1) { cameraDistance = 0.1; }
    if (cameraAngleV > 89) { cameraAngleV = 89; }
    if (cameraAngleV < 10) { cameraAngleV = 10; }

    // compute position of camera in space
    cVector3d pos = cAdd(
                        cameraPosition,
                        cVector3d(
                            cameraDistance * cCosDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleV)
                        )
                    );

    // compute lookat position
    cVector3d lookat = cameraPosition;

    // define role orientation of camera
    cVector3d up(0.0, 0.0, 1.0);

    // set new position to camera
    camera->set(pos, lookat, up);

    // recompute global positions
    world->computeGlobalPositions(true);
}

//---------------------------------------------------------------------------

void mouseClick(int button, int state, int x, int y)
{
     // mouse button down
    if (state == GLUT_DOWN)
    {
        flagCameraInMotion = true;
        mouseX = x;
        mouseY = y;
        mouseButton = button;
    }

    // mouse button up
    else if (state == GLUT_UP)
    {
        flagCameraInMotion = false;
    }
}

//---------------------------------------------------------------------------

void mouseMove(int x, int y)
{
    if (flagCameraInMotion)
    {
        if (mouseButton == GLUT_RIGHT_BUTTON)
        {
            cameraDistance = cameraDistance - 0.01 * (y - mouseY);
        }

        else if (mouseButton == GLUT_LEFT_BUTTON)
        {
            cameraAngleH = cameraAngleH - (x - mouseX);
            cameraAngleV = cameraAngleV + (y - mouseY);
        }
    }

    updateCameraPosition();

    mouseX = x;
    mouseY = y;
}

//---------------------------------------------------------------------------

bool createTetGenMesh(cGELMesh *a_object, char *a_filename,  char *a_filenameHighRes)
{
    a_object->m_useSkeletonModel = false;
    a_object->m_useMassParticleModel = true;

    cGELMesh* model = new cGELMesh(world);
    model->loadFromFile(a_filenameHighRes);

    tetgenio input;
    if (input.load_off(a_filename))
    {
        // use TetGen to tetrahedralize our mesh
        tetgenio output;
        tetrahedralize(TETGEN_SWITCHES0, &input, &output);

        // create a vertex in the object for each point of the result
        for (int p = 0, pi = 0; p < output.numberofpoints; ++p, pi += 3)
        {
            cVector3d point;
            point.x = output.pointlist[pi+0];
            point.y = output.pointlist[pi+1];
            point.z = output.pointlist[pi+2];
            a_object->newVertex(point);
        }

        // create a triangle for each face on the surface
        set<int> outside;
        for (int t = 0, ti = 0; t < output.numberoftrifaces; ++t, ti += 3)
        {
            cVector3d p[3];
            int vi[3];
            for (int i = 0; i < 3; ++i) {
                int tc = output.trifacelist[ti+i];
                outside.insert(tc);
                vi[i] = tc;
                int pi = tc*3;
                p[i].x = output.pointlist[pi+0];
                p[i].y = output.pointlist[pi+1];
                p[i].z = output.pointlist[pi+2];
            }
            //unsigned int index = a_object->newTriangle(p[1], p[0], p[2]);
            a_object->newTriangle(vi[1], vi[0], vi[2]);
        }

        a_object->computeAllNormals();

        // compute a boundary box
        a_object->computeBoundaryBox(true);

        // get dimensions of object
        double size = cSub(a_object->getBoundaryMax(), a_object->getBoundaryMin()).length();

        // resize object to screen
        if (size > 0)
        {
            model->scale(1.5 / size);
            a_object->scale(1.5 / size);
        }

        // build dynamic vertices
        cGELMassParticle::default_mass = 0.002;
        cGELMassParticle::default_kDampingPos = 4.0;
        cGELMassParticle::default_gravity.set(0,0,-0.1);

        a_object->buildVertices();

        // get all the edges of our tetrahedra
        set< pair<int,int> > springs;
        for (int t = 0, ti = 0; t < output.numberoftetrahedra; ++t, ti += 4)
        {
            // store each edge of the tetrahedron as a pair of indices
            for (int i = 0; i < 4; ++i) {
                int v0 = output.tetrahedronlist[ti+i];
                for (int j = i+1; j < 4; ++j) {
                    int v1 = output.tetrahedronlist[ti+j];
                    springs.insert(pair<int,int>(min(v0,v1), max(v0,v1)));
                }
            }
        }

        // create a spring on each tetrahedral edge we found in the output
        cGELLinearSpring::default_kSpringElongation = 40.0; // 0.55;
        for (set< pair<int,int> >::iterator it = springs.begin(); it != springs.end(); ++it)
        {
            cVertex* v0 = a_object->getVertex(it->first);
            cVertex* v1 = a_object->getVertex(it->second);

            cGELMassParticle* m0 = a_object->m_gelVertices[v0->m_tag].m_massParticle;
            cGELMassParticle* m1 = a_object->m_gelVertices[v1->m_tag].m_massParticle;

            cGELLinearSpring* spring = new cGELLinearSpring(m0, m1);
            a_object->m_linearSprings.push_back(spring);
        }

        // extract texture
        int numModelV = model->getNumVertices(true);
        for (set<int>::iterator it = outside.begin(); it != outside.end(); ++it)
        {
            cVertex *v = a_object->getVertex(*it);
            cVertex *t = 0;
            double closest = 1e10;
            for (int j = 0; j < numModelV; ++j) {
                cVertex *test = model->getVertex(j, true);
                double d = cDistanceSq(v->getPos(), test->getPos());
                if (d < closest) {
                    closest = d;
                    t = test;
                }
            }

            v->setTexCoord(t->getTexCoord());
        }

        cMesh* mesh = (cMesh*)model->getChild(0);
        mesh->m_texture->setWrapMode(GL_CLAMP, GL_CLAMP);
        a_object->setTexture(mesh->m_texture, true);
        a_object->setUseTexture(true, true);

        cMaterial mat;
        mat.m_ambient.set(0.7, 0.7, 0.7);
        mat.m_diffuse.set(0.8, 0.8, 0.8);
        mat.m_specular.set(0.0, 0.0, 0.0);
        a_object->setMaterial(mat, true);

        return (true);
    }
    return (false);
}

//---------------------------------------------------------------------------

bool createSkeletonMesh(cGELMesh *a_object, char *a_filename, char *a_filenameHighRes)
{
    a_object->m_useSkeletonModel = true;
    a_object->m_useMassParticleModel = false;
    a_object->loadFromFile(a_filenameHighRes);

    cGELMesh* model = new cGELMesh(world);

    tetgenio input;
    if (input.load_off(a_filename))
    {
        // use TetGen to tetrahedralize our mesh
        tetgenio output;
        tetrahedralize(TETGEN_SWITCHES, &input, &output);

        // create a vertex in the object for each point of the result
        for (int p = 0, pi = 0; p < output.numberofpoints; ++p, pi += 3)
        {
            cVector3d point;
            point.x = output.pointlist[pi+0];
            point.y = output.pointlist[pi+1];
            point.z = output.pointlist[pi+2];
            model->newVertex(point);
        }

        // create a triangle for each face on the surface
        for (int t = 0, ti = 0; t < output.numberoftrifaces; ++t, ti += 3)
        {
            cVector3d p[3];
            int vi[3];
            for (int i = 0; i < 3; ++i) {
                int tc = output.trifacelist[ti+i];
                vi[i] = tc;
                int pi = tc*3;
                p[i].x = output.pointlist[pi+0];
                p[i].y = output.pointlist[pi+1];
                p[i].z = output.pointlist[pi+2];
            }
            //unsigned int index = a_object->newTriangle(p[1], p[0], p[2]);
            model->newTriangle(vi[1], vi[0], vi[2]);
        }

        // find out exactly which vertices are on the inside and outside
        set<int> inside, outside;
        for (int t = 0; t < output.numberoftrifaces * 3; ++t)
        {
            outside.insert(output.trifacelist[t]);
        }
        for (int p = 0; p < output.numberofpoints; ++p)
        {
            if (outside.find(p) == outside.end())
                inside.insert(p);
        }

        model->computeAllNormals();

        // compute a boundary box
        model->computeBoundaryBox(true);

        // get dimensions of object
        double size = cSub(model->getBoundaryMax(), model->getBoundaryMin()).length();

        // resize object to screen
        if (size > 0)
        {
            model->scale( 1.5 / size);
            a_object->scale( 1.5 / size);
        }

        // setup default values for nodes
        cGELSkeletonNode::default_radius        = 0.05;
        cGELSkeletonNode::default_kDampingPos   = 0.3;
        cGELSkeletonNode::default_kDampingRot   = 0.1;
        cGELSkeletonNode::default_mass          = 0.002;  // [kg]
        cGELSkeletonNode::default_showFrame     = false;
        cGELSkeletonNode::default_color.set(1.0, 0.6, 0.6);
        cGELSkeletonNode::default_useGravity    = true;
        cGELSkeletonNode::default_gravity.set(0.00, 0.00, -3.45);
        radius = cGELSkeletonNode::default_radius;

        a_object->buildVertices();
        model->buildVertices();

        vector<cGELSkeletonNode*> nodes;
        int i=0;
        for (set<int>::iterator it = inside.begin(); it != inside.end(); ++it)
        {
            cGELSkeletonNode* newNode = new cGELSkeletonNode();
            a_object->m_nodes.push_front(newNode);
            cVertex* vertex = model->getVertex(*it);
            newNode->m_pos = vertex->getPos();
            newNode->m_rot.identity();
            newNode->m_radius = 0.1;
            newNode->m_fixed = false;
            vertex->m_tag = i;
            i++;
            nodes.push_back(newNode);
        }

        // get all the edges of our tetrahedra
        set< pair<int,int> > springs;
        for (int t = 0, ti = 0; t < output.numberoftetrahedra; ++t, ti += 4)
        {
            // store each edge of the tetrahedron as a pair of indices
            for (int i = 0; i < 4; ++i) {
                int v0 = output.tetrahedronlist[ti+i];
                for (int j = i+1; j < 4; ++j) {
                    int v1 = output.tetrahedronlist[ti+j];

                    // connect only if both points are inside
                    if (inside.find(v0) != inside.end() && inside.find(v1) != inside.end())
                        springs.insert(pair<int,int>(min(v0,v1), max(v0,v1)));
                }
            }
        }

        // setup default values for links
        cGELSkeletonLink::default_kSpringElongation = 100.0; // [N/m]
        cGELSkeletonLink::default_kSpringFlexion    = 0.1;   // [Nm/RAD]
        cGELSkeletonLink::default_kSpringTorsion    = 0.1;   // [Nm/RAD]
        cGELSkeletonLink::default_color.set(0.2, 0.2, 1.0);

        for (set< pair<int,int> >::iterator it = springs.begin(); it != springs.end(); ++it)
        {
            cVertex* v0 = model->getVertex(it->first);
            cVertex* v1 = model->getVertex(it->second);
            cGELSkeletonNode* n0 = nodes[v0->m_tag];
            cGELSkeletonNode* n1 = nodes[v1->m_tag];
            cGELSkeletonLink* newLink = new cGELSkeletonLink(n0, n1);
            a_object->m_links.push_front(newLink);
        }

        a_object->connectVerticesToSkeleton(false);

        cMaterial mat;
        mat.m_ambient.set(0.7, 0.7, 0.7);
        mat.m_diffuse.set(0.8, 0.8, 0.8);
        mat.m_specular.set(0.0, 0.0, 0.0);
        a_object->setMaterial(mat, true);

        return (true);
    }
    return (false);
}

//---------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
    // update the size of the viewport
    displayW = w;
    displayH = h;
    glViewport(0, 0, displayW, displayH);
}

//---------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
    // escape key
    if ((key == 27) || (key == 'x'))
    {
        // close everything
        close();

        // exit application
        exit(0);
    }

    // option 1:
    if (key == '1')
    {
        // show skeleton
        defObject->m_showSkeletonModel = true;
        defObject->setWireMode(true, true);
    }

    // option 2:
    if (key == '2')
    {
        // hide skeleton
        defObject->m_showSkeletonModel = false;
        defObject->setWireMode(false, true);
    }
}

//---------------------------------------------------------------------------

void menuSelect(int value)
{
    switch (value)
    {
        // enable full screen display
        case OPTION_FULLSCREEN:
            glutFullScreen();
            break;

        // reshape window to original size
        case OPTION_WINDOWDISPLAY:
            glutReshapeWindow(WINDOW_SIZE_W, WINDOW_SIZE_H);
            break;

        // show the dynamic skeleton of the object
        case OPTION_SHOWSKELETON:
			if(USE_SKELETON_MODEL)
            {
                defObject->m_showSkeletonModel = true;
                defObject->setWireMode(true, true);
            }
            else
            {
			    defObject->m_showMassParticleModel = true;
            }

            break;

        // hide the dynamic skeleton of the object
        case OPTION_HIDESKELETON:
			if(USE_SKELETON_MODEL)
            {
                defObject->m_showSkeletonModel = false;
                defObject->setWireMode(false, true);
            }
			else
            {
                defObject->m_showMassParticleModel = false;
            }

            defObject->setTransparencyLevel(1.0, true, true);
            break;
    }
}

//---------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close haptic device
    hapticDevice->close();
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    // update mesh of deformable model
    defWorld->updateSkins();
    defObject->computeAllNormals(true);
    ground->computeAllNormals(true);

    // render world
    camera->renderView(displayW, displayH);

    // Swap buffers
    glutSwapBuffers();

    // check for any OpenGL errors
    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

    // inform the GLUT window to call updateGraphics again (next frame)
    if (simulationRunning)
    {
        glutPostRedisplay();
    }
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
	// reset simulation clock
	simClock.reset();
	simClock.start();

    // main haptic simulation loop
    while(simulationRunning)
    {
        // read position from haptic device
        cVector3d pos;
        hapticDevice->getPosition(pos);
        pos.mul(workspaceScaleFactor);
        device->setPos(pos);

        // init temp variable
        cVector3d force;
        force.zero();

        // compute reaction forces
        list<cGELMesh*>::iterator i;

        // model water level
        for(i = defWorld->m_gelMeshes.begin(); i != defWorld->m_gelMeshes.end(); ++i)
        {
            cGELMesh *nextItem = *i;

            if (nextItem->m_useMassParticleModel)
            {
                int numVertices = nextItem->m_gelVertices.size();
                for (int i=0; i<numVertices; i++)
                {
                   cVector3d nodePos = nextItem->m_gelVertices[i].m_massParticle->m_pos;
                   cVector3d force = cVector3d(-0.002 * nodePos.x, -0.002 * nodePos.y, 0.0);
                   if (nodePos.z < level)
                   {
                        double depth = nodePos.z - level;
                        force.add(cVector3d(0,0,-100*depth));
                   }
                   nextItem->m_gelVertices[i].m_massParticle->setExternalForce(force);
                }
            }

            if (nextItem->m_useSkeletonModel)
            {
                list<cGELSkeletonNode*>::iterator i;
                for(i = nextItem->m_nodes.begin(); i != nextItem->m_nodes.end(); ++i)
                {
                    cGELSkeletonNode* node = *i;
                    cVector3d nodePos = node->m_pos;
                    double radius = node->m_radius;
                    cVector3d force = cVector3d(-0.01 * nodePos.x, -0.01 * (nodePos.y), 0.0);
                    if ((nodePos.z-radius) < level)
                    {
                        double depth = (nodePos.z-radius) - level;
                        force.add(cVector3d(0,0,-1.0 * depth));
                        node->m_vel.mul(0.95);
                    }
                    node->setExternalForce(force);
                }
            }
        }


        // compute haptic feedback
		for(i = defWorld->m_gelMeshes.begin(); i != defWorld->m_gelMeshes.end(); ++i)
		{
			cGELMesh *nextItem = *i;

			if (nextItem->m_useMassParticleModel)
			{
				int numVertices = nextItem->m_gelVertices.size();
				for (int i=0; i<numVertices; i++)
				{
				cVector3d nodePos = nextItem->m_gelVertices[i].m_massParticle->m_pos;
				cVector3d f = computeForce(pos, deviceRadius, nodePos, radius, stiffness);
				if (f.lengthsq() > 0)
				{
					cVector3d tmpfrc = cNegate(f);
					nextItem->m_gelVertices[i].m_massParticle->setExternalForce(tmpfrc);
				}
				force.add(cMul(1.0, f));
				}
			}

			if (nextItem->m_useSkeletonModel)
			{
				list<cGELSkeletonNode*>::iterator i;
				for(i = nextItem->m_nodes.begin(); i != nextItem->m_nodes.end(); ++i)
				{
					cGELSkeletonNode* node = *i;
					cVector3d nodePos = node->m_pos;
					double radius = node->m_radius;
					cVector3d f = computeForce(pos, deviceRadius, nodePos, radius, stiffness);
					if (f.lengthsq() > 0)
					{
						cVector3d tmpfrc = cNegate(f);
						node->setExternalForce(tmpfrc);
					}
					force.add(cMul(4.0, f));
				}
			}
		}

        // integrate dynamics
		double interval = simClock.stop();
		simClock.reset();
		simClock.start();

        if (interval > 0.001) { interval = 0.001; }
        defWorld->updateDynamics(interval);

        // scale force
        force.mul(0.6 * deviceForceScale);

        // water viscosity
        if ((pos.z - deviceRadius) < level)
        {
            // read damping properties of haptic device
            cHapticDeviceInfo info = hapticDevice->getSpecifications();
            double Kv = 0.8 * info.m_maxLinearDamping;

            // read device velocity
            cVector3d linearVelocity;
            hapticDevice->getLinearVelocity(linearVelocity);

            // compute a scale factor [0,1] proportional to percentage
            // of tool volume immersed in the water
            double val = (level - (pos.z - deviceRadius)) / (2.0 * deviceRadius);
            double scale = cClamp(val, 0.1, 1.0);

            // compute force
            cVector3d forceDamping = cMul(-Kv * scale, linearVelocity);
            force.add(forceDamping);
        }

        // send forces to haptic device
        hapticDevice->setForce(force);
    }

    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------

cVector3d computeForce(const cVector3d& a_cursor,
                       double a_cursorRadius,
                       const cVector3d& a_spherePos,
                       double a_radius,
                       double a_stiffness)
{

    // compute the reaction forces between the tool and the ith sphere.
    cVector3d force;
    force.zero();
    cVector3d vSphereCursor = a_cursor - a_spherePos;

    // check if both objects are intersecting
    if (vSphereCursor.length() < 0.0000001)
    {
        return (force);
    }

    if (vSphereCursor.length() > (a_cursorRadius + a_radius))
    {
        return (force);
    }

    // compute penetration distance between tool and surface of sphere
    double penetrationDistance = (a_cursorRadius + a_radius) - vSphereCursor.length();
    cVector3d forceDirection = cNormalize(vSphereCursor);
    force = cMul( penetrationDistance * a_stiffness, forceDirection);

    // return result
    return (force);
}
