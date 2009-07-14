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
    \version   2.0.0 $Rev: 265 $
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

// maximum number of haptic devices supported in this demo
const int MAX_DEVICES           = 8;


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

// a table containing pointers to all haptic devices detected on this computer
cGenericHapticDevice* hapticDevices[MAX_DEVICES];

// a table containing pointers to label which display the position of
// each haptic device
cLabel* labels[MAX_DEVICES];
cGenericObject* rootLabels;

// number of haptic devices detected
int numHapticDevices = 0;

// table containing a list of 3D cursors for each haptic device
cShapeSphere* cursors[MAX_DEVICES];

// table containing a list of lines to display velocity
cShapeLine* velocityVectors[MAX_DEVICES];

// material properties used to render the color of the cursors
cMaterial matCursorButtonON;
cMaterial matCursorButtonOFF;

// status of the main simulation haptics loop
bool simulationRunning = false;

// root resource path
string resourceRoot;

// damping mode ON/OFF
bool useDamping = false;

// force field mode ON/OFF
bool useForceField = true;

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
    DEMO:    device.cpp

    This application illustrates the use of the haptic device handler
    "cHapticDevicehandler" to access all of the haptic devices
    "cGenericHapticDevice" connected to the computer.

    In this example the application opens an OpenGL window and displays a
    3D cursor for each device. Each cursor (sphere + reference frame)
    represents the position and orientation of its respective device.
    If the operator presses the device user button (if available), the color
    of the cursor changes accordingly.

    In the main haptics loop function  "updateHaptics()" , the position,
    orientation and user switch status of each device are retrieved at
    each simulation iteration. The information is then used to update the
    position, orientation and color of the cursor. A force is then commanded
    to the haptic device to attract the end-effector towards the device origin.
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
    printf ("Demo: 01-devices\n");
    printf ("Copyright 2003-2009\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
    printf ("[1] - Render attraction force\n");
    printf ("[2] - Render viscous environment\n");
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
    camera->set( cVector3d (0.5, 0.0, 0.0),    // camera position (eye)
                 cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
                 cVector3d (0.0, 0.0, 1.0));   // direction of the "up" vector

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

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

    // read the number of haptic devices currently connected to the computer
    numHapticDevices = handler->getNumDevices();

    // limit the number of devices to MAX_DEVICES
    numHapticDevices = cMin(numHapticDevices, MAX_DEVICES);

    // create a node on which we will attach small labels that display the
    // position of each haptic device
    rootLabels = new cGenericObject();
    camera->m_front_2Dscene.addChild(rootLabels);

    // create a small label as title
    cLabel* titleLabel = new cLabel();
    rootLabels->addChild(titleLabel);

    // define its position, color and string message
    titleLabel->setPos(0, 30, 0);
    titleLabel->m_fontColor.set(1.0, 1.0, 1.0);
    titleLabel->m_string = "Haptic Device Pos [mm]:";

    // for each available haptic device, create a 3D cursor
    // and a small line to show velocity
    int i = 0;
    while (i < numHapticDevices)
    {
        // get a handle to the next haptic device
        cGenericHapticDevice* newHapticDevice;
        handler->getDevice(newHapticDevice, i);

        // open connection to haptic device
        newHapticDevice->open();

		// initialize haptic device
		newHapticDevice->initialize();

        // store the handle in the haptic device table
        hapticDevices[i] = newHapticDevice;

        // retrieve information about the current haptic device
        cHapticDeviceInfo info = newHapticDevice->getSpecifications();

        // create a cursor by setting its radius
        cShapeSphere* newCursor = new cShapeSphere(0.01);

        // add cursor to the world
        world->addChild(newCursor);

        // add cursor to the cursor table
        cursors[i] = newCursor;

        // create a small line to illustrate velocity
        cShapeLine* newLine = new cShapeLine(cVector3d(0,0,0), cVector3d(0,0,0));
        velocityVectors[i] = newLine;

        // add line to the world
        world->addChild(newLine);

        // create a string that concatenates the device number and model name.
        string strID;
        cStr(strID, i);
        string strDevice = "#" + strID + " - " +info.m_modelName;

        // attach a small label next to the cursor to indicate device information
        cLabel* newLabel = new cLabel();
        newCursor->addChild(newLabel);
        newLabel->m_string = strDevice;
        newLabel->setPos(0.00, 0.02, 0.00);
        newLabel->m_fontColor.set(1.0, 1.0, 1.0);

        // if the device provided orientation sensing (stylus), a reference
        // frame is displayed
        if (info.m_sensedRotation == true)
        {
            // display a reference frame
            newCursor->setShowFrame(true);

            // set the size of the reference frame
            newCursor->setFrameSize(0.05, 0.05);
        }

        // crate a small label to indicate the position of the device
        cLabel* newPosLabel = new cLabel();
        rootLabels->addChild(newPosLabel);
        newPosLabel->setPos(0, -20 * i, 0);
        newPosLabel->m_fontColor.set(0.6, 0.6, 0.6);
        labels[i] = newPosLabel;

        // increment counter
        i++;
    }

    // here we define the material properties of the cursor when the
    // user button of the device end-effector is engaged (ON) or released (OFF)

    // a light orange material color
    matCursorButtonOFF.m_ambient.set(0.5, 0.2, 0.0);
    matCursorButtonOFF.m_diffuse.set(1.0, 0.5, 0.0);
    matCursorButtonOFF.m_specular.set(1.0, 1.0, 1.0);

    // a blue material color
    matCursorButtonON.m_ambient.set(0.1, 0.1, 0.4);
    matCursorButtonON.m_diffuse.set(0.3, 0.3, 0.8);
    matCursorButtonON.m_specular.set(1.0, 1.0, 1.0);


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

    // update position of labels
    rootLabels->setPos(10, displayH-70, 0);
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
        useForceField = !useForceField;
        if (useForceField)
        {
            printf ("- Enable force field\n");
        }
        else
        {
            printf ("- Disable force field\n");
        }
    }

    // option 2:
    if (key == '2')
    {
        useDamping = !useDamping;
        if (useDamping)
        {
            printf ("- Enable viscosity\n");
        }
        else
        {
            printf ("- Disable viscosity\n");
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

    // close all haptic devices
    int i=0;
    while (i < numHapticDevices)
    {
        hapticDevices[i]->close();
        i++;
    }
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    // update content of position label
    for (int i=0; i<numHapticDevices; i++)
    {
        // read position of device an convert into millimeters
        cVector3d pos;
        hapticDevices[i]->getPosition(pos);
        pos.mul(1000);

        // create a string that concatenates the device number and its position.
        string strID;
        cStr(strID, i);
        string strLabel = "#" + strID + "  x: ";

        cStr(strLabel, pos.x, 2);
        strLabel = strLabel + "   y: ";
        cStr(strLabel, pos.y, 2);
        strLabel = strLabel + "  z: ";
        cStr(strLabel, pos.z, 2);

        labels[i]->m_string = strLabel;
    }

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
        // for each device
        int i=0;
        while (i < numHapticDevices)
        {
            // read position of haptic device
            cVector3d newPosition;
            hapticDevices[i]->getPosition(newPosition);

            // read orientation of haptic device
            cMatrix3d newRotation;
            hapticDevices[i]->getRotation(newRotation);

            // update position and orientation of cursor
            cursors[i]->setPos(newPosition);
            cursors[i]->setRot(newRotation);

            // read linear velocity from device
            cVector3d linearVelocity;
            hapticDevices[i]->getLinearVelocity(linearVelocity);

            // update arrow
            velocityVectors[i]->m_pointA = newPosition;
            velocityVectors[i]->m_pointB = cAdd(newPosition, linearVelocity);

            // read user button status
            bool buttonStatus;
            hapticDevices[i]->getUserSwitch(0, buttonStatus);

            // adjustthe  color of the cursor according to the status of
            // the user switch (ON = TRUE / OFF = FALSE)
            if (buttonStatus)
            {
                cursors[i]->m_material = matCursorButtonON;
            }
            else
            {
                cursors[i]->m_material = matCursorButtonOFF;
            }

            // compute a reaction force
            cVector3d newForce (0,0,0);

            // apply force field
            if (useForceField)
            {
                double Kp = 20.0; // [N/m]
                cVector3d force = cMul(-Kp, newPosition);
                newForce.add(force);
            }
        
            // apply viscosity
            if (useDamping)
            {
                cHapticDeviceInfo info = hapticDevices[i]->getSpecifications();
                double Kv = info.m_maxLinearDamping;
                cVector3d force = cMul(-Kv, linearVelocity);
                newForce.add(force);
            }

            // send computed force to haptic device
            hapticDevices[i]->setForce(newForce);

            // increment counter
            i++;
        }
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------
