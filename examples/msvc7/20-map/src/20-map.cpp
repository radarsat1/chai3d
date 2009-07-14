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

// size of map
const double MESH_SCALE_SIZE = 2.0;


//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

// a mesh object used to create the height map
cMesh* object;

// a small magnetic line used to constrain the tool alon the vertical axis
cShapeLine* magneticLine;

// two sphere position at the end of the magnetic line
cShapeSphere* sphereA;
cShapeSphere* sphereB;

// a light source to illuminate the objects in the virtual scene
cLight *light;

// a little "chai3d" bitmap logo at the bottom of the screen
cBitmap* logo;

// width and height of the current window display
int displayW  = 0;
int displayH  = 0;

// a haptic device handler
cHapticDeviceHandler* handler;

// a 3D cursor which represents the haptic device
cGeneric3dofPointer* tool;

// radius of the tool proxy
double proxyRadius;

// status of the main simulation haptics loop
bool simulationRunning = false;

// update camera settings
void updateCameraPosition();

// camera position and orientation is spherical coordinates
double cameraAngleH;
double cameraAngleV;
double cameraDistance;
cVector3d cameraPosition;

// camera status
bool flagCameraInMotion;

// mouse position and button status
int mouseX, mouseY;
int mouseButton;

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

// callback to handle mouse click
void mouseClick(int button, int state, int x, int y);

// callback to handle mouse motion
void mouseMove(int x, int y);

// function called before exiting the application
void close(void);

// main graphics callback
void updateGraphics(void);

// main haptics loop
void updateHaptics(void);

// loads a bitmap file and create 3D height map based on pixel color
int loadHeightMap();


//===========================================================================
/*
    DEMO:    map.cpp

    This example illustrates the construction a triangle based object.
    The application first loads a bitmap texture image. For each pixel,
    we then define a height by computing the gray-scale value. A vertex
    is created for each pixel and triangles are then generated to connect
    the array of vertices together. This example also demonstrates the
    use of mouse callback commands to allow the operator to control the
    position of the virtual camera. The operator can also use the haptic
    device (user switch command) to move the camera or grasp a point on the 
    surface and deform the terrain.

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
    printf ("Demo: 20-map\n");
    printf ("Copyright 2003-2009\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Instructions:\n\n");
    printf ("- Use left mouse button to rotate camera view. \n");
    printf ("- Use right mouse button to control camera zoom. \n");
    printf ("- Use haptic device and user switch to rotate \n");
    printf ("  camera or deform terrain. \n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
    printf ("[1] - Texture   (ON/OFF)\n");
    printf ("[2] - Wireframe (ON/OFF)\n");
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
    world->setBackgroundColor(0.1, 0.1, 0.1);

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // define a default position of the camera (described in spherical coordinates)
    cameraAngleH = 0;
    cameraAngleV = 45;
    cameraDistance = 1.8 * MESH_SCALE_SIZE;
    updateCameraPosition();

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d( 0.0, 0.3, 0.3));  // position the light source
    light->setDir(cVector3d(-1.0,-0.1, -0.1));  // define the direction of the light beam
    light->m_ambient.set(0.5, 0.5, 0.5);
    light->m_diffuse.set(0.8, 0.8, 0.8);
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

    // attach tool to camera
    camera->addChild(tool);

    // position tool workspace in front of camera (x-axis of camera reference pointing towards the viewer)
    tool->setPos(-cameraDistance, 0.0, 0.0);

    // connect the haptic device to the tool
    tool->setHapticDevice(hapticDevice);

    // initialize tool by connecting to haptic device
    tool->start();

    // map the physical workspace of the haptic device to a larger virtual workspace.
    tool->setWorkspaceRadius(1.0);

    // define a radius for the tool (graphical display)
    tool->setRadius(0.03);

    // hide the device sphere. only show proxy.
    tool->m_deviceSphere->setShowEnabled(false);

    // set the physical readius of the proxy.
    proxyRadius = 0.0;
    tool->m_proxyPointForceModel->setProxyRadius(proxyRadius);
	tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = false;


    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------
    // create a new mesh to display a height map
    object = new cMesh(world);
    world->addChild(object);

    // load default map
    loadHeightMap();

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // define a maximum stiffness that can be handled by the current
    // haptic device. The value is scaled to take into account the
    // workspace scale factor
    double stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;
    object->setStiffness(0.5 * stiffnessMax, true);

    // create a small vertical white magnetic line that will be activated when the
    // user deforms the mesh.
    magneticLine = new cShapeLine(cVector3d(0,0,0), cVector3d(0,0,0));
    world->addChild(magneticLine);
    magneticLine->m_ColorPointA.set(0.6, 0.6, 0.6);
    magneticLine->m_ColorPointB.set(0.6, 0.6, 0.6);
    magneticLine->setShowEnabled(false);

    // set haptic properties
    magneticLine->m_material.setStiffness(0.05 * stiffnessMax);
    magneticLine->m_material.setMagnetMaxForce(0.2 * info.m_maxForce);
    magneticLine->m_material.setMagnetMaxDistance(0.25);
    magneticLine->m_material.setViscosity(0.05 * info.m_maxLinearDamping);

    // create a haptic magnetic effect
    cEffectMagnet* newEffect = new cEffectMagnet(magneticLine);
    magneticLine->addEffect(newEffect);

    // disable haptic feedback for now
    magneticLine->setHapticEnabled(false);

    // create two sphere that will be added at both ends of the line
    sphereA = new cShapeSphere(0.02);
    sphereB = new cShapeSphere(0.02);
    world->addChild(sphereA);
    world->addChild(sphereB);
    sphereA->setShowEnabled(false);
    sphereB->setShowEnabled(false);

    // define some material properties for spheres
    cMaterial matSphere;
    matSphere.m_ambient.set(0.5, 0.2, 0.2);
    matSphere.m_diffuse.set(0.8, 0.4, 0.4);
    matSphere.m_specular.set(1.0, 1.0, 1.0);
    sphereA->setMaterial(matSphere);
    sphereB->setMaterial(matSphere);


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
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("CHAI 3D");


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
        bool useTexture = object->getUseTexture();
        object->setUseTexture(!useTexture);
    }

    // option 2:
    if (key == '2')
    {
        bool useWireMode = object->getWireMode();
        object->setWireMode(!useWireMode);
    }
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
    // update object normals
    object->computeAllNormals(true);

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
    // state machine
    const int STATE_IDLE            = 1;
    const int STATE_MODIFY_MAP      = 2;
    const int STATE_MOVE_CAMERA     = 3;
    int state = STATE_IDLE;  

    // current tool position
    cVector3d toolGlobalPos;        // global world coordinates
    cVector3d toolLocalPos;         // local coordinates

    // previous tool position
    cVector3d prevToolGlobalPos;    // global world coordinates
    cVector3d prevToolLocalPos;     // local coordinates

    // main haptic simulation loop
    while(simulationRunning)
    {
        // compute global reference frames for each object
        world->computeGlobalPositions(true);

        // update position and orientation of tool
        tool->updatePose();

        // compute interaction forces
        tool->computeInteractionForces();

        // read user switch
        bool userSwitch = tool->getUserSwitch(0);

        // update tool position
        toolGlobalPos = tool->getDeviceGlobalPos();
        toolLocalPos  = tool->getDeviceLocalPos();

        if ((state == STATE_MOVE_CAMERA) && (!userSwitch))
        {
            state = STATE_IDLE;

            // enable haptic interaction with map
            object->setHapticEnabled(true, true);
        }

        else if (((state == STATE_MODIFY_MAP) && (!userSwitch)) ||
            ((state == STATE_MODIFY_MAP) && (!tool->isInContact(magneticLine))))
        {
            state = STATE_IDLE;

            // disable magnetic line
            magneticLine->setHapticEnabled(false);
            magneticLine->setShowEnabled(false);

            // disable spheres
            sphereA->setShowEnabled(false);
            sphereB->setShowEnabled(false);

            // enable haptic interaction with map
            object->setHapticEnabled(true, true);

            // disable forces
            tool->setForcesOFF();

            // update bounding box (can take a little time)
            object->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
            
            // enable forces again
            tool->setForcesON();
        }

        // user clicks with the mouse
        else if ((state == STATE_IDLE) && (userSwitch))
        {
            // start deforming object
            if (tool->isInContact(object))
            {
                state = STATE_MODIFY_MAP;

                // update position of line
                cVector3d posA = toolGlobalPos;
                posA.z =-0.7;

                cVector3d posB = toolGlobalPos;
                posB.z = 0.7;

                magneticLine->m_pointA = posA;
                magneticLine->m_pointB = posB;

                // update position of spheres
                sphereA->setPos(posA);
                sphereB->setPos(posB);

                // enable spheres
                sphereA->setShowEnabled(true);
                sphereB->setShowEnabled(true);

                // enable magnetic line
                magneticLine->setHapticEnabled(true);
                magneticLine->setShowEnabled(true);

                // disable haptic interaction with map
                object->setHapticEnabled(false, true);
            }

            // start moving camera
            else
            {
                state = STATE_MOVE_CAMERA;
                
                // disable haptic interaction with map
                object->setHapticEnabled(false, true);
            }
        }

        // modify map
        else if (state == STATE_MODIFY_MAP)
        {
            // compute tool offset
            cVector3d offset = toolGlobalPos - prevToolGlobalPos;

            // map offset on z axis
            double offsetHeight = offset.z;

            // apply offset to all vertices through a weighted function
            int numVertices = object->getNumVertices(true);
            for (int i=0; i<numVertices; i++)
            {
                // get next vertex
                cVertex* vertex = object->getVertex(i, true);

                // compute distance between vertex and tool
                cVector3d posTool = tool->getProxyGlobalPos();
                cVector3d posVertex = vertex->getPos();
                double distance = cDistance(posTool, posVertex);

                // compute factor
                double RADIUS = 0.4;
                double relativeDistance = distance / RADIUS;
                double clampedRelativeDistance = cClamp01(relativeDistance);
                double w = 0.5 + 0.5 * cos(clampedRelativeDistance * CHAI_PI);

                // apply offset
                double offsetVertexHeight = w * offsetHeight;
                posVertex.z = posVertex.z + offsetVertexHeight;
                vertex->setPos(posVertex);
            }
        }

        // move camera
        else if (state == STATE_MOVE_CAMERA)
        {
            // compute tool offset
            cVector3d offset = toolLocalPos - prevToolLocalPos;

            // apply camera motion
            cameraDistance = cameraDistance - 2 * offset.x;
            cameraAngleH = cameraAngleH - 40 * offset.y;
            cameraAngleV = cameraAngleV - 40 * offset.z;

            updateCameraPosition();   
        }

        // store tool position
        prevToolLocalPos  = toolLocalPos;
        prevToolGlobalPos = toolGlobalPos;

        // send forces to device
        tool->applyForces();
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------

int loadHeightMap()
{
    // create a texture file
    cTexture2D* newTexture = new cTexture2D();
    world->addTexture(newTexture);

    // texture 2D
    bool fileload = newTexture->loadFromFile(RESOURCE_PATH("resources/images/map.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = newTexture->loadFromFile("../../../bin/resources/images/map.bmp");
        #endif
    }
    if (!fileload)
    {
        printf("Error - Texture image failed to load correctly.\n");
        close();
        return (-1);
    }

    // get the size of the texture image (U and V)
    int texSizeU = newTexture->m_image.getWidth();
    int texSizeV = newTexture->m_image.getHeight();

    // check size of image
    if ((texSizeU < 1) || (texSizeV < 1)) { return (false); }

    // we look for the largest side
    int largestSide;
    if (texSizeU > texSizeV)
    {
        largestSide = texSizeU;
    }
    else
    {
        largestSide = texSizeV;
    }

    // The largest side of the map has a length of 1.0
    // we now compute the respective size for 1 pixel of the image in world space.
    double size = 1.0 / (double)largestSide;

    // we will create an triangle based object. For centering puposes we
    // compute an offset for axis X and Y corresponding to the half size
    // of the image map.
    double offsetU = 0.5 * (double)texSizeU * size;
    double offsetV = 0.5 * (double)texSizeV * size;

    // For each pixel of the image, create a vertex
    int u,v;
    for (v=0; v<texSizeV; v++)
    {
        for (u=0; u<texSizeU; u++)
        {
            double px, py, tu, tv;

            // compute the height of the vertex
            cColorb color = newTexture->m_image.getPixelColor(u,v);
            double height = 0.1 * ((double)color.getR() + (double)color.getG() + (double)color.getB()) / (3.0 * 255.0);

            // compute the position of the vertex
            px = size * (double)u - offsetU;
            py = size * (double)v - offsetV;

            // create new vertex
            unsigned int index = object->newVertex(px, py, height);
            cVertex* vertex = object->getVertex(index);

            // compute texture coordinate
            tu = (double)u / texSizeU;
            tv = (double)v / texSizeV;
            vertex->setTexCoord(tu, tv);
        }
    }

    // Create a triangle based map using the above pixels
     for (v=0; v<(texSizeV-1); v++)
    {
        for (u=0; u<(texSizeU-1); u++)
        {
            // get the indexing numbers of the next four vertices
            unsigned int index00 = ((v + 0) * texSizeU) + (u + 0);
            unsigned int index01 = ((v + 0) * texSizeU) + (u + 1);
            unsigned int index10 = ((v + 1) * texSizeU) + (u + 0);
            unsigned int index11 = ((v + 1) * texSizeU) + (u + 1);

            // create two new triangles
            object->newTriangle(index00, index01, index10);
            object->newTriangle(index10, index01, index11);
        }
    }

    // apply texture to object
    object->setTexture(newTexture);
    object->setUseTexture(true);

    // compute normals
    object->computeAllNormals(true);

    // compute size of object
    object->computeBoundaryBox(true);

    cVector3d min = object->getBoundaryMin();
    cVector3d max = object->getBoundaryMax();

    // This is the "size" of the object
    cVector3d span = cSub(max, min);
    size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    double scaleFactor = MESH_SCALE_SIZE / size;
    object->scale(scaleFactor);

    // compute size of object again
    object->computeBoundaryBox(true);

    // Build a collision-detector for this object, so
    // the proxy will work nicely when haptics are enabled.
    object->createAABBCollisionDetector(1.01 * proxyRadius, true, false);

    // set size of frame
    object->setFrameSize(0.2, true);

    // set size of normals
    object->setNormalsProperties(0.01, cColorf(1.0, 0.0, 0.0, 1.0), true);

    // render graphically both sides of triangles
    object->setUseCulling(false);

    // update global position
    object->computeGlobalPositions();

    // success
    return (0);
}

//---------------------------------------------------------------------------

void updateCameraPosition()
{
    // check values
    if (cameraDistance < 0.1) { cameraDistance = 0.1; }
    if (cameraAngleV > 89) { cameraAngleV = 89; }
    if (cameraAngleV < -89) { cameraAngleV = -89; }

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

    // update tool position
    if (tool != NULL)
    tool->setPos(-cameraDistance, 0.0, 0.0);
}

//---------------------------------------------------------------------------
