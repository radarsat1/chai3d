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

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 600;
const int WINDOW_SIZE_H         = 600;

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

// a virtual tool representing the haptic device in the scene
cGeneric3dofPointer* tool;

// a few spherical objects
cShapeSphere* object0;
cShapeSphere* object1;
cShapeSphere* object2;
cShapeSphere* object3;

// status of the main simulation haptics loop
bool simulationRunning = false;

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


//===========================================================================
/*
    DEMO:    effects.cpp

    In this example illustrates the use of haptic effects. The application
    begins by creating four spheres with different graphical (
    material and texture) properties. For each sphere, one or more effects
    are programmed to obtain the desired haptic illusion. Parameters to
    adjust each effect are located in the cMaterial class.

    If you wish to develop your own shapes and force effect, take a look at
    the example located in "src/effects". You can also implement your own
    shapes by following the approach presented in the cShapeSphere,
    cShapeTorus or cShapeLine classes for instance.

    In the main haptics loop function  "updateHaptics()" , the position
    of the haptic device is retrieved at each simulation iteration.
    The interaction forces are then computed and sent to the device.
    Finally, a simple dynamics model is used to simulate the behavior
    of the o-ring like object.
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
    printf ("Demo: 11-effects\n");
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
    camera->set( cVector3d (3.0, 0.0, 0.0),    // camera position (eye)
                 cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
                 cVector3d (0.0, 0.0, 1.0));   // direction of the "up" vector

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // enable higher quality rendering for transparent objects
    camera->enableMultipassTransparency(true);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam


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
    cGenericHapticDevice* hapticDevice;
    handler->getDevice(hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo info;
    if (hapticDevice)
    {
        info = hapticDevice->getSpecifications();
    }

    // create a 3D tool and add it to the world
    tool = new cGeneric3dofPointer(world);
    world->addChild(tool);

    // connect the haptic device to the tool
    tool->setHapticDevice(hapticDevice);

    // initialize tool by connecting to haptic device
    tool->start();

    // map the physical workspace of the haptic device to a larger virtual workspace.
    tool->setWorkspaceRadius(1.0);

    // define a radius for the tool
    tool->setRadius(0.03);

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // define a maximum stiffness that can be handled by the current
    // haptic device. The value is scaled to take into account the
    // workspace scale factor
    double stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;
    double forceMax = info.m_maxForce;

    // define the maximum damping factor that can be handled by the
    // current haptic device. The The value is scaled to take into account the
    // workspace scale factor
    double dampingMax = info.m_maxLinearDamping / workspaceScaleFactor; 


    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------

    // temp variable
    cGenericEffect* newEffect;


    /////////////////////////////////////////////////////////////////////////
    // OBJECT 0: "MAGNET"
    /////////////////////////////////////////////////////////////////////////

    // create a sphere and define its radius
    object0 = new cShapeSphere(0.3);

    // add object to world
    world->addChild(object0);

    // set the position of the object at the center of the world
    object0->setPos(0.0, -0.5, 0.0);

    // set graphic properties
    object0->m_texture = new cTexture2D();
    fileload = object0->m_texture->loadFromFile(RESOURCE_PATH("resources/images/chrome.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = object0->m_texture->loadFromFile("../../../bin/resources/images/chrome.bmp");
        #endif
    }
    if (!fileload)
    {
        printf("Error - Texture image failed to load correctly.\n");
        close();
        return (-1);
    }

    object0->m_texture->setSphericalMappingEnabled(true);
    object0->m_material.m_ambient.set(1.0, 1.0, 1.0);
    object0->m_material.m_diffuse.set(1.0, 1.0, 1.0);
    object0->m_material.m_specular.set(1.0, 1.0, 1.0);
    object0->m_material.setShininess(100);
    object0->setUseTexture(true);

    // set haptic properties
    object0->m_material.setStiffness(0.4 * stiffnessMax);
    object0->m_material.setMagnetMaxForce(0.4 * forceMax);
    object0->m_material.setMagnetMaxDistance(0.12);
    object0->m_material.setViscosity(0.2 * dampingMax);

    // create a haptic surface effect
    newEffect = new cEffectSurface(object0);
    object0->addEffect(newEffect);

    // create a haptic magnetic effect
    newEffect = new cEffectMagnet(object0);
    object0->addEffect(newEffect);


    /////////////////////////////////////////////////////////////////////////
    // OBJECT 1: "FLUID"
    ////////////////////////////////////////////////////////////////////////

    // create a sphere and define its radius
    object1 = new cShapeSphere(0.3);

    // add object to world
    world->addChild(object1);

    // set the position of the object at the center of the world
    object1->setPos(0.0, 0.5, 0.0);

    // set graphic properties
    object1->m_texture = new cTexture2D();
		fileload = object1->m_texture->loadFromFile(RESOURCE_PATH("resources/images/water.bmp"));
		if (!fileload)
		{
				#if defined(_MSVC)
				fileload = object1->m_texture->loadFromFile("../../../bin/resources/images/water.bmp");
				#endif
		}
		if (!fileload)
		{
				printf("Error - Texture image failed to load correctly.\n");
				close();
				return (-1);
		}

    object1->m_material.m_ambient.set(0.1, 0.1, 0.6, 0.5);
    object1->m_material.m_diffuse.set(0.3, 0.3, 0.9, 0.5);
    object1->m_material.m_specular.set(1.0, 1.0, 1.0, 0.5);
    object1->setTransparencyLevel(0.5);
    object1->setUseTexture(true);

    // set haptic properties
    object1->m_material.setViscosity(dampingMax);

    // create a haptic viscous effect
    newEffect = new cEffectViscosity(object1);
    object1->addEffect(newEffect);


    /////////////////////////////////////////////////////////////////////////
    // OBJECT 2: "STICK-SLIP"
    /////////////////////////////////////////////////////////////////////////

    // create a sphere and define its radius
    object2 = new cShapeSphere(0.3);

    // add object to world
    world->addChild(object2);

    // set the position of the object at the center of the world
    object2->setPos(0.0, 0.0, 0.5);

    // set graphic properties
    object2->m_texture = new cTexture2D();
    fileload = object2->m_texture->loadFromFile(RESOURCE_PATH("resources/images/stone.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = object2->m_texture->loadFromFile("../../../bin/resources/images/stone.bmp");
        #endif
    }
    if (!fileload)
    {
        printf("Error - Texture image failed to load correctly.\n");
        close();
        return (-1);
    }

    object2->m_material.m_ambient.set(0.1, 0.1, 0.6, 0.5);
    object2->m_material.m_diffuse.set(0.3, 0.3, 0.9, 0.5);
    object2->m_material.m_specular.set(1.0, 1.0, 1.0, 0.5);
    object2->setTransparencyLevel(0.5);
    object2->setUseTexture(true);

    // set haptic properties
    object2->m_material.setStickSlipForceMax(0.5 * forceMax);
    object2->m_material.setStickSlipStiffness(0.7 * stiffnessMax);

    // create a haptic stick-slip effect
    newEffect = new cEffectStickSlip(object2);
    object2->addEffect(newEffect);


    /////////////////////////////////////////////////////////////////////////
    // OBJECT 3: "VIBRATIONS"
    ////////////////////////////////////////////////////////////////////////

    // create a sphere and define its radius
    object3 = new cShapeSphere(0.3);

    // add object to world
    world->addChild(object3);

    // set the position of the object at the center of the world
    object3->setPos(0.0, 0.0, -0.5);

    // set graphic properties
    object3->m_texture = new cTexture2D();
    fileload = object3->m_texture->loadFromFile(RESOURCE_PATH("resources/images/plastic.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = object3->m_texture->loadFromFile("../../../bin/resources/images/plastic.bmp");
        #endif
    }
    if (!fileload)
    {
        printf("Error - Texture image failed to load correctly.\n");
        close();
        return (-1);
    }

    object3->m_texture->setSphericalMappingEnabled(true);
    object3->m_material.m_ambient.set(0.6, 0.1, 0.1, 0.5);
    object3->m_material.m_diffuse.set(0.9, 0.3, 0.3, 0.5);
    object3->m_material.m_specular.set(1.0, 1.0, 1.0, 0.5);
    object3->setTransparencyLevel(0.8);
    object3->setUseTexture(true);

    // set haptic properties
    object3->m_material.setVibrationFrequency(50);
    object3->m_material.setVibrationAmplitude(0.1 * forceMax);
    object3->m_material.setStiffness(0.1 * stiffnessMax);

    // create a haptic viscous effect
    newEffect = new cEffectVibration(object3);
    object3->addEffect(newEffect);

    newEffect = new cEffectSurface(object3);
    object3->addEffect(newEffect);


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
    tool->stop();
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
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

    // rotate the following objcts to create some animation
    object1->rotate(cVector3d(0,0,1),  0.02);
    object3->rotate(cVector3d(1,1,1), -0.01);
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
    // main haptic simulation loop
    while(simulationRunning)
    {
        // compute global reference frames for each object
        world->computeGlobalPositions(true);

        // update position and orientation of tool
        tool->updatePose();

        // compute interaction forces
        tool->computeInteractionForces();

        // send forces to device
        tool->applyForces();
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------
