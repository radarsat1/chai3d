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

// information about the current haptic device
cHapticDeviceInfo info;

// a virtual tool representing the haptic device in the scene
cGeneric3dofPointer* tool;

// radius of the tool proxy
double proxyRadius;

// a virtual tooth mesh
cMesh* tooth;

// transparency level
double transparencyLevel = 0.3;

// virtual drill mesh
cMesh* drill;

// temp variable to store positions and orientations
// of tooth and drill
cVector3d lastPosObject;
cMatrix3d lastRotObject;
cVector3d lastPosDevice;
cMatrix3d lastRotDevice;
cVector3d lastDeviceObjectPos;
cMatrix3d lastDeviceObjectRot;

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
    DEMO:    tooth.cpp

    This demonstration shows how to attach a 3D mesh file to a virtual
    tool. A second mesh (tooth) is loaded in the scene. By pressing the user
    switch of the haptic device it is possible to translate or oriente
    the tooth object accordingly.

    In the main haptics loop function  "updateHaptics()" , the position
    of the haptic device is retrieved at each simulation iteration.
    The interaction forces are then computed and sent to the device.
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
    printf ("Demo: 23-tooth\n");
    printf ("Copyright 2003-2009\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Instructions:\n\n");
    printf ("- Use haptic device and user switch to rotate \n");
    printf ("  rotate and translate tooth. \n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
    printf ("[1] - Wireframe (ON/OFF)\n");
    printf ("[2] - Normals   (ON/OFF)\n");
    printf ("[+] - Increase Opacity\n");
    printf ("[-] - Reduce Opacity\n");
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

    // enable high quality rendering when tooth becomes transparent
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

    // define a radius for the tool (graphical display)
    tool->setRadius(0.01);

    // hide the device sphere. only show proxy.
    tool->m_deviceSphere->setShowEnabled(false);

    // set the physical radius of the proxy to be equal to the radius
    // of the tip of the mesh drill (see drill in the virtual scene section)
    proxyRadius = 0.03;
    tool->m_proxyPointForceModel->setProxyRadius(proxyRadius);

    // informe the finger-proxy force renderer to only check one side of triangles
    tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = false;

    // the environmeny is static, you can set this parameter to "false"
    tool->m_proxyPointForceModel->m_useDynamicProxy = false;

    tool->m_proxyPointForceModel->m_useForceShading = true;

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // define a maximum stiffness that can be handled by the current
    // haptic device. The value is scaled to take into account the
    // workspace scale factor
    double stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;


    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------

    // create a virtual mesh
    tooth = new cMesh(world);

    // add object to world
    world->addChild(tooth);

    // set the position and orientation of the object at the center of the world
    tooth->setPos(0.0, 0.0, 0.0);
    tooth->rotate(cVector3d(0.0, 0.0, 1.0), cDegToRad(-10));
    tooth->rotate(cVector3d(0.0, 1.0, 0.0), cDegToRad(10));

    // load an object file
    fileload = tooth->loadFromFile(RESOURCE_PATH("resources/models/tooth/tooth.3ds"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = tooth->loadFromFile("../../../bin/resources/models/tooth/tooth.3ds");
        #endif
    }
    if (!fileload)
    {
        printf("Error - 3D Model failed to load correctly.\n");
        close();
        return (-1);
    }

    // make the outside of the tooth rendered in wireframe
    ((cMesh*)(tooth->getChild(1)))->setWireMode(true);

    // make the outside of the tooth rendered in semi-transparent
    ((cMesh*)(tooth->getChild(1)))->setUseTransparency(false);
    ((cMesh*)(tooth->getChild(1)))->setTransparencyLevel(transparencyLevel);

    // compute a boundary box
    tooth->computeBoundaryBox(true);

    // resize tooth to screen
    tooth->scale(0.003);

    // compute collision detection algorithm
    tooth->createAABBCollisionDetector(1.01 * proxyRadius, true, false);

    // define a default stiffness for the object
    tooth->setStiffness(0.8 * stiffnessMax, true);

    // create a new mesh.
    drill = new cMesh(world);

    // load a drill like mesh and attach it to the tool
    fileload = drill->loadFromFile(RESOURCE_PATH("resources/models/drill/drill.3ds"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = drill->loadFromFile("../../../bin/resources/models/drill/drill.3ds");
        #endif
    }
    if (!fileload)
    {
        printf("Error - 3D Model failed to load correctly.\n");
        close();
        return (-1);
    }

    // resize tool mesh model
    drill->scale(0.004);

    // remove the collision detector. we do not want to compute any
    // force feedback rendering on the object itself.
    drill->deleteCollisionDetector(true);

    // define a material property for the mesh
    cMaterial mat;
    mat.m_ambient.set(0.5, 0.5, 0.5);
    mat.m_diffuse.set(0.8, 0.8, 0.8);
    mat.m_specular.set(1.0, 1.0, 1.0);
    drill->setMaterial(mat, true);
    drill->computeAllNormals(true);

    // attach drill to tool
    tool->m_proxyMesh->addChild(drill);


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

    // option 1:
    if (key == '1')
    {
        // only enable/disable wireframe of one part of the tooth model
        bool useWireMode = ((cMesh*)(tooth->getChild(1)))->getWireMode();
        ((cMesh*)(tooth->getChild(1)))->setWireMode(!useWireMode);
    }

    // option 2:
    if (key == '2')
    {
        bool showNormals = tooth->getShowNormals();
        tooth->setShowNormals(!showNormals, true);
        tooth->setNormalsProperties(0.05, cColorf(1.0, 0.0, 0.0), true);
    }
    
    // option -:
    if (key == '-')
    {
        // decrease transparency level
        transparencyLevel = transparencyLevel - 0.1;
        if (transparencyLevel < 0.0) { transparencyLevel = 0.0; }

        // apply changes to tooth
        ((cMesh*)(tooth->getChild(1)))->setTransparencyLevel(transparencyLevel);
        ((cMesh*)(tooth->getChild(1)))->setUseTransparency(true);

        // if object is almost transparent, make it invisible
        if (transparencyLevel < 0.1)
        {
            ((cMesh*)(tooth->getChild(1)))->setShowEnabled(false, true);
        }
    }

    // option +:
    if (key == '+')
    {
        // increase transparency level
        transparencyLevel = transparencyLevel + 0.1;
        if (transparencyLevel > 1.0) { transparencyLevel = 1.0; }

        // apply changes to tooth
        ((cMesh*)(tooth->getChild(1)))->setTransparencyLevel(transparencyLevel);

        // make object visible
        if (transparencyLevel >= 0.1)
        {
            ((cMesh*)(tooth->getChild(1)))->setShowEnabled(true, true);
        }

        // disable transparency is transparency level is set to 1.0
        if (transparencyLevel == 1.0)
        {
            ((cMesh*)(tooth->getChild(1)))->setUseTransparency(false);
        }
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
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
    // main haptic simulation loop
    while(simulationRunning)
    {
        // update position and orientation of tool
        tool->updatePose();

        // compute interaction forces
        tool->computeInteractionForces();

        // send forces to device
        tool->applyForces();

        // if the haptic device does track orientations, we automatically
        // oriente the drill to remain perpendicular to the tooth
        cVector3d pos = tool->m_proxyPointForceModel->getProxyGlobalPosition();
        cMatrix3d rot = tool->m_deviceGlobalRot;

        if (info.m_sensedRotation == false)
        {
            cVector3d pos = tool->m_proxyPointForceModel->getProxyGlobalPosition();
            rot.identity();

            cVector3d vx, vy, vz;
            cVector3d zUp (0,0,1);
            cVector3d yUp (0,1,0);
            vx = pos - tooth->getPos();
            if (vx.length() > 0.001)
            {
                vx.normalize();

                if (cAngle(vx,zUp) > 0.001)
                {
                    vy = cCross(zUp, vx);
                    vy.normalize();
                    vz = cCross(vx, vy);
                    vz.normalize();

                }
                else
                {
                    vy = cCross(yUp, vx);
                    vy.normalize();
                    vz = cCross(vx, vy);
                    vz.normalize();
                }

                rot.setCol(vx, vy, vz);
                drill->setRot(rot);
            }
        }


        int button = tool->getUserSwitch(0);
        if (button == 0)
        {
            lastPosDevice = pos;
            lastRotDevice = rot;
            lastPosObject = tooth->getPos();
            lastRotObject = tooth->getRot();
            lastDeviceObjectPos = cTrans(lastRotDevice) * ((lastPosObject - lastPosDevice) + 0.01*cNormalize(lastPosObject - lastPosDevice));
            lastDeviceObjectRot = cMul(cTrans(lastRotDevice), lastRotObject);
            tooth->setHapticEnabled(true, true);
            tool->setShowEnabled(true, true);
            drill->setShowEnabled(true, true);
        }
        else
        {
            tool->setShowEnabled(false, true);
            drill->setShowEnabled(false, true);
            cMatrix3d rotDevice01 = cMul(cTrans(lastRotDevice), rot);
            cMatrix3d newRot =  cMul(rot, lastDeviceObjectRot);
            cVector3d newPos = cAdd(pos, cMul(rot, lastDeviceObjectPos));
            tooth->setPos(newPos);
            tooth->setRot(newRot);
            world->computeGlobalPositions(true);
            tooth->setHapticEnabled(false, true);
        }

        // compute global reference frames for each object
        world->computeGlobalPositions(true);
    }

    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------
