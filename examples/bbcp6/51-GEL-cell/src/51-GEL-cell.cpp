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
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------
#include "GEL3D.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 700;
const int WINDOW_SIZE_H         = 700;

// mouse menu options (right button)
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;


//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

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

// object mesh
cGELMesh* defObject;

// dynamic nodes
cGELSkeletonNode* nodes[10][10];

// device  model
cShapeSphere* device;
double deviceRadius;

// radius of the dynamic model sphere (GEM)
double radius;


//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// callback when the right mouse button is pressed to select a menu item
void menuSelect(int value);

// function called before exiting the application
void close(void);

// main graphics callback
void updateGraphics(void);

// main haptics loop
void updateHaptics(void);

// compute forces between cursor and environment
cVector3d computeForce(const cVector3d& a_cursor,
                       double a_cursorRadius,
                       const cVector3d& a_spherePos,
                       double a_radius,
                       double a_stiffness);

// Build deformable model of huge cell
void BuildDynamicModel();

//===========================================================================
/*
    DEMO:    GEM_cell.cpp

    This application illustrates the use of the GEM libraries to simulate
    deformable object. In this example we load a large mesh object and
    build a dynamic skeleton composed of volumetric spheres and 3 dimensional
    springs which model torsion, flexion and elongation properties.
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
    printf ("Demo: 41-GEL-cell\n");
    printf ("Copyright 2003-2009\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
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

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and oriente the camera
    camera->set( cVector3d (1.5, 0.6, 0.2),    // camera position (eye)
                 cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
                 cVector3d (0.0, 0.0, 1.0));   // direction of the "up" vector

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // enable higher rendering quality because we are displaying transparent objects
    camera->enableMultipassTransparency(true);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d(0.0,0.5,0.7));     // position the light source
    light->setDir(cVector3d(-1.0,0.0,0.0));    // set the direction of the light beam


    // adjust light source components
    light->m_ambient.set(0.9, 0.9, 0.9);
    light->m_diffuse.set(0.9, 0.9, 0.9);
    light->m_specular.set(1.0, 1.0, 1.0);

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
    cursorWorkspaceRadius = 0.5;

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    workspaceScaleFactor = cursorWorkspaceRadius / info.m_workspaceRadius;

    // define a scale factor between the force perceived at the cursor and the
    // forces actually sent to the haptic device
    deviceForceScale = 5.0 * info.m_maxForce;

    // create a large sphere that represents the haptic device
    deviceRadius = 0.08;
    device = new cShapeSphere(deviceRadius);
    world->addChild(device);
    device->m_material.m_ambient.set(1.0, 0.4, 0.4, 0.5);
    device->m_material.m_diffuse.set(1.0, 0.7, 0.7, 0.5);
    device->m_material.m_specular.set(1.0, 1.0, 1.0, 0.5);
    device->m_material.setShininess(100);
    device->setUseTransparency(true);


    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------
    // create a world which supports deformable object
    defWorld = new cGELWorld();
    world->addChild(defWorld);
    //world->setPos(-1.5, 0.0, -0.1);
    world->rotate(cVector3d(0,1,0), cDegToRad(50));

    // Play with thse numbers carefully!
	cGELSkeletonNode::default_kDampingPos = 0.1;
    cGELSkeletonNode::default_kDampingRot = 0.1;
    defWorld->m_integrationTime = 1.0/400.0;

    // create a deformable mesh
    defObject = new cGELMesh(world);
    defWorld->m_gelMeshes.push_front(defObject);
    fileload = defObject->loadFromFile(RESOURCE_PATH("resources/models/cell/cell.obj"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = defObject->loadFromFile("../../../bin/resources/models/cell/cell.obj");
        #endif
    }
    if (!fileload)
    {
        printf("Error - 3D Model failed to load correctly.\n");
        close();
        return (-1);
    }

    // compute size of object
    defObject->computeBoundaryBox(true);
    cVector3d min = defObject->getBoundaryMin();
    cVector3d max = defObject->getBoundaryMax();

    // This is the "size" of the object
    cVector3d span = cSub(max, min);
    double size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    if (size > 0)
    {
        double scaleFactor = 1.0 / size;
        defObject->scale(scaleFactor);
    }

    // compute size of object again
    defObject->computeBoundaryBox(true);

    // setup default values for nodes
    cGELSkeletonNode::default_radius        = 0.04;//3;
    cGELSkeletonNode::default_kDampingPos   = 0.4;
    cGELSkeletonNode::default_kDampingRot   = 0.1;
    cGELSkeletonNode::default_mass          = 0.06;  // [kg]
    cGELSkeletonNode::default_showFrame     = false;
    cGELSkeletonNode::default_color.set(0.6, 0.6, 0.0);
    cGELSkeletonNode::default_useGravity      = false;
    cGELSkeletonNode::default_gravity.set(0.00, 0.00, -1.00);
    radius = cGELSkeletonNode::default_radius;

    // setup default values for links
    cGELSkeletonLink::default_kSpringElongation = 100.0; // [N/m]
    cGELSkeletonLink::default_kSpringFlexion    = 0.5;   // [Nm/RAD]
    cGELSkeletonLink::default_kSpringTorsion    = 0.1;   // [Nm/RAD]
    cGELSkeletonLink::default_color.set(0.2, 0.2, 1.0);

    // build dynamic vertices
    defObject->buildVertices();
    defObject->m_useSkeletonModel = true;

    // create dynamic model (GEM)
    BuildDynamicModel();

    // connect skin to skeleton
    defObject->connectVerticesToSkeleton(true);

    // show/hide underlying dynamic skeleton model
    defObject->m_showSkeletonModel = false;

    // define the integration time constant of the dynamics model
    defWorld->m_integrationTime = 0.001;

    // create anchors
    cGELSkeletonLink::default_kSpringElongation = 5.0; // [N/m]
    list<cGELSkeletonNode*>::iterator i;
    int num = 0;
    for(i = defObject->m_nodes.begin(); i != defObject->m_nodes.end(); ++i)
    {
        num++;
    }

    int counter1 = 0;
    int counter2 = 0;
    for(i = defObject->m_nodes.begin(); i != defObject->m_nodes.end(); ++i)
    {
        if (counter1 <= num)
        {
            if (counter2 > 3)
            {
                cGELSkeletonNode* nextItem = *i;
                cGELSkeletonNode* newNode = new cGELSkeletonNode();
                newNode->m_fixed = true;
                newNode->m_pos = nextItem->m_pos;
                cGELSkeletonLink* newLink = new cGELSkeletonLink(nextItem, newNode); defObject->m_links.push_front(newLink);
                newLink->m_kSpringElongation = 5;
                counter2 = 0;
            }
            counter2 ++;
            counter1++;
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
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("CHAI 3D");

    // create a mouse menu (right button)
    glutCreateMenu(menuSelect);
    glutAddMenuEntry("full screen", OPTION_FULLSCREEN);
    glutAddMenuEntry("window display", OPTION_WINDOWDISPLAY);
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
    // reset clock
    simClock.reset();

    // main haptic simulation loop
    while(simulationRunning)
    {
        // stop the simulation clock
        simClock.stop();

        // read the time increment in seconds
        double timeInterval = simClock.getCurrentTimeSeconds();
        if (timeInterval > 0.005) { timeInterval = 0.005; }

        // restart the simulation clock
        simClock.reset();
        simClock.start();

        // read position from haptic device
        cVector3d pos;
        hapticDevice->getPosition(pos);
        pos.mul(workspaceScaleFactor);
        device->setPos(pos);

        // init temp variable
        cVector3d force;
        force.zero();

        // clear all external forces
        defWorld->clearExternalForces();

        // compute reaction forces
        list<cGELSkeletonNode*>::iterator i;
        for(i = defObject->m_nodes.begin(); i != defObject->m_nodes.end(); ++i)
        {
            cGELSkeletonNode* nextItem = *i;

            cVector3d nodePos = nextItem->m_pos;
            cVector3d forcec = computeForce(pos, 0, nodePos, deviceRadius+nextItem->m_radius, 1.0);
            force.add(forcec);

            if (forcec.length() > 0)
            {
              cVector3d tmpfrc = -12.0 * forcec;
                nextItem->setExternalForce(tmpfrc);
            }
        }

        // update dynamics
        defWorld->updateDynamics(timeInterval);

        // scale force
        force.mul(deviceForceScale);

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

    // In the following we compute the reaction forces between the tool and the
    // sphere.
    cVector3d force;
    force.zero();

    cVector3d vSphereCursor = a_cursor - a_spherePos;


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

    return (force);
}

//---------------------------------------------------------------------------

void BuildDynamicModel()
{
    cGELSkeletonNode* newNode;
    cGELSkeletonNode* prevNode;
    cGELSkeletonLink* newLink;

    //-----------------------------------
    newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.296,0.280,-0.213);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.286,0.213,-0.254);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.241,0.148,-0.274);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.200,0.084,-0.267);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.043,-0.266,-0.226);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.114,-0.281,-0.151);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.120,-0.306,-0.073);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.085,-0.343,-0.023);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.013,-0.372,-0.006);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.030,-0.392,0.053);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.087,-0.399,0.094);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.129,-0.388,0.127);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.192,-0.357,0.119);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.386,-0.220,0.174);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.340,-0.267,0.161);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.308,-0.304,0.122);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.300,-0.329,0.059);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.319,-0.331,-0.018);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.363,-0.325,-0.080);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.386,-0.299,-0.117);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.380,-0.266,-0.162);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.375,-0.232,-0.204);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.374,-0.217,-0.249);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.418,-0.145,-0.266);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.362,-0.182,-0.277);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.309,-0.227,-0.277);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.255,-0.259,-0.286);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.199,-0.285,-0.284);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.123,-0.299,-0.271);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.023,-0.279,-0.245);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.029,-0.228,-0.286);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.145,-0.214,-0.336);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.197,-0.177,-0.329);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.243,-0.115,-0.310);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.198,0.011,-0.247);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.155,0.033,-0.161);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.080,-0.015,-0.105);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.042,-0.064,-0.035);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.030,-0.078,0.038);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.005,-0.111,0.099);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.097,0.015,-0.021);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.020,-0.003,-0.041);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.003,-0.065,-0.072);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.007,-0.122,-0.102);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.061,-0.151,-0.103);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.118,-0.171,-0.051);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.277,-0.175,-0.029);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.336,-0.148,-0.094);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.395,-0.107,-0.138);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.415,-0.059,-0.181);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.524,-0.003,-0.106);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.494,-0.035,-0.072);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.435,-0.052,-0.037);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.377,-0.048,0.004);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.328,0.003,0.069);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.288,0.059,0.004);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.236,0.095,-0.048);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.195,0.151,-0.059);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.157,0.184,-0.077);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.097,0.245,-0.062);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.055,0.283,-0.036);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.045,0.318,0.028);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.081,0.359,0.107);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.163,0.371,0.149);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.238,0.358,0.126);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.312,0.344,0.113);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.335,0.301,0.179);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.331,0.247,0.237);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.305,0.193,0.298);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.267,0.145,0.337);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.228,0.101,0.366);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.183,0.059,0.374);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.096,-0.011,0.335);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.003,-0.041,0.273);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.215,-0.124,0.358);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.296,-0.109,0.343);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.374,-0.081,0.322);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.462,-0.073,0.263);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.501,-0.125,0.190);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.491,-0.179,0.133);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.437,-0.232,0.082);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.370,-0.264,0.027);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.313,-0.234,-0.033);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.329,-0.178,-0.008);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.347,-0.095,0.016);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.345,-0.034,0.042);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.314,-0.011,0.104);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.261,-0.035,0.167);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.220,-0.057,0.234);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.172,-0.095,0.306);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.129,-0.131,0.329);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.059,-0.128,0.261);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.030,-0.115,0.199);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.001,-0.112,0.109);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.045,-0.129,0.035);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.055,-0.168,-0.043);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.052,-0.200,-0.157);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.027,-0.210,-0.235);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.035,-0.141,-0.243);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(-0.062,-0.050,-0.269);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.009,-0.032,-0.292);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.104,-0.004,-0.326);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.212,0.026,-0.323);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.292,0.045,-0.258);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.275,0.037,-0.184);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.248,0.069,-0.115);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.229,0.044,-0.048);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.208,-0.017,-0.022);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.173,-0.131,-0.020);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.305,0.366,0.054);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.300,0.399,-0.006);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.274,0.395,-0.079);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.230,0.343,-0.142);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.161,0.280,-0.147);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.161,0.227,-0.121);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.174,0.186,-0.076);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.151,0.194,-0.067);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.124,0.220,-0.059);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.385,0.342,-0.098);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.328,0.370,-0.044);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.258,0.351,0.014);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.193,0.341,0.045);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.139,0.316,0.068);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.109,0.272,0.079);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.082,0.224,0.094);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.087,0.232,0.153);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.152,0.233,0.188);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.209,0.208,0.148);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.210,0.193,0.093);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.191,0.188,0.041);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.163,0.177,-0.004);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.126,0.185,-0.046);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.099,0.205,-0.093);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.112,0.224,-0.131);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.122,0.241,-0.189);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.157,0.229,-0.227);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.226,0.211,-0.240);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.224,0.188,-0.190);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.280,0.178,-0.154);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.394,0.206,-0.105);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.473,0.202,-0.060);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.522,0.173,-0.028);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.542,0.120,0.005);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.546,0.059,0.032);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.544,0.001,0.072);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.525,-0.053,0.094);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.488,-0.144,0.094);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.488,-0.192,0.040);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.519,-0.195,-0.029);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.528,-0.198,-0.116);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.506,-0.187,-0.194);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.477,-0.149,-0.241);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.414,-0.125,-0.256);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.408,-0.069,-0.228);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.436,-0.002,-0.181);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.475,0.066,-0.175);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.482,0.136,-0.177);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.476,0.203,-0.185);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.443,0.272,-0.176);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.402,0.324,-0.133);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.378,0.326,-0.085);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.385,0.262,-0.040);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.369,0.200,-0.004);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.341,0.155,0.018);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.290,0.105,0.040);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.230,0.053,0.054);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.149,0.004,0.092);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.111,-0.018,0.147);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.101,-0.075,0.186);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.089,-0.144,0.192);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.101,-0.183,0.168);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.116,-0.183,0.105);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.121,-0.169,0.028);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.165,-0.189,-0.005);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.225,-0.217,0.008);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.298,-0.205,0.009);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.366,-0.206,0.011);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.439,-0.209,-0.021);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
    //-----------------------------------
    prevNode = newNode; newNode = new cGELSkeletonNode(); defObject->m_nodes.push_front(newNode);
    newNode->m_pos.set(0.508,-0.200,-0.043);
    newLink = new cGELSkeletonLink(prevNode, newNode); defObject->m_links.push_front(newLink);
}



