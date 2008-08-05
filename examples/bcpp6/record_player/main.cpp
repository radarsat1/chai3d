//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2004 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author:    <http://www.chai3d.org>
    \author:    fed
    \version    1.1
    \date       12/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TForm1 *Form1;
void HapticLoop(void* param);
//---------------------------------------------------------------------------

const double MESH_SCALE_SIZE = 1.0;

// Global variables for the audio stream
HSTREAM stream;
BASS_CHANNELINFO info;
QWORD stream_length;
char *data;
int record_direction = 1;
unsigned int pos = 0;


// Load an audio file in .wav format
bool LoadWaveFile(LPSTR szFileName)
{
  // Load the data from the specified file
  HSTREAM file_stream = BASS_StreamCreateFile(FALSE,szFileName,0,0,BASS_STREAM_DECODE);

  // Get the length and header info from the loaded file
  stream_length=BASS_StreamGetLength(file_stream);
  BASS_ChannelGetInfo(file_stream, &info);

  // Get the audio samples from the loaded file
  data = new char[(unsigned int)stream_length];
  BASS_ChannelGetData(file_stream, data, (unsigned int)stream_length);
	
  // Set playing to begin at the beginning of the loaded data
  pos = 0;

  return false;
}


// Write the requested data from the loaded buffer to the sound card
DWORD CALLBACK MyStreamWriter(HSTREAM handle, void *buf, DWORD len, DWORD user)
{
  // Cast the buffer to a character array
  char *d=(char*)buf;

  // Loop the file when it reaches the beginning or end
  if ((pos >= stream_length) && (record_direction == 1))
    pos = 0;
  if ((pos <= 0) && (record_direction == -1))
    pos = (unsigned int)stream_length;
		
  // If record is spinning in positive direction, write requested
  // amount of data from current position forwards
  if (record_direction == 1)
  {
    int up = len + pos;
    if (up > stream_length)
      up = (unsigned int)stream_length;

    for (int i=pos; i<up; i+=1) 
      d[(i-pos)] = data[i];

    int amt = (up-pos);
    pos += amt;
    return amt;
  }

  // If record is spinning in negative direction, write requested 
  // amount of data from current position backwards
  if (record_direction == -1)
  {
    int up = pos - len;

    if (up < 0)
      up = 0;

    int cnt = 0;
    for (int i=pos; i>up; i-=1) 
      d[cnt++] = data[i];

    int amt = cnt;
    pos -= amt;

    return amt;
  }
  return 0;
}

//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // create a new world
    world = new cWorld();

    // set background color
    world->setBackgroundColor(0.3f,0.3f,0.35f);

    // Create a camera
    camera = new cCamera(world);
    world->addChild(camera);

    // Create a light source and attach it to camera
    light = new cLight(world);
    light->setEnabled(true);
    light->setPos(0,0,0);
    light->setDir(-1,0,0);
    light->m_ambient.set(0.5, 0.5, 0.5);
    light->m_diffuse.set(0.9, 0.9, 0.9);
    light->m_ambient.set(0.9, 0.9, 0.9);
    camera->addChild(light);

    camera->set(cVector3d(1.5,0,0.8), cVector3d(0,0,0), cVector3d(0,0,1));

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, true);
    viewport->setStereoOn(false);

    // create a mesh - we will build a simple cube, and later let the
    // user load 3d models
    object = new cMesh(world);
    world->addChild(object);

    // init var
    m_rotPos = 0;
    m_rotVel = 0;
    m_inertia = 0.04;
    m_clock.initialize();
    m_lastGoodPosition = 0.0;

    m_reduction = 5.4*10;
    m_cpt = 120*4;
    m_desiredPos = 0.0;
    m_P = 0.4;
    m_I = 0.0014;
    m_integratorVal = 0.0;
    m_D = 0.02;

    m_lastAngle = 0.0;

    m_velocity = 0;
    m_velocityOld = 0;

    m_RFDInitialAngle = 0.0;
    m_inContact = false;

    haptics_enabled = false;

    // Initialize sound device and create audio stream
    if (!BASS_Init(1,44100,0,0,NULL))
	    Application->MessageBox("Initialization Error", "Error");

    // Load a record onto the record player
    load_player();
    object->translate(0,0,-0.3);
    load_record(0);

    RecordSelect->Items->Add("Italy");
    RecordSelect->Items->Add("Switzerland");
    RecordSelect->Items->Add("USA");
    RecordSelect->ItemIndex = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    // stop simulation
    // stop graphic rendering
    Timer1->Enabled = false;
    timer.stop();

    // cleanup memory
    delete world;
    delete viewport;
}

//---------------------------------------------------------------------------

void TForm1::updateWorldSettings()
{
    // set stiffness
    double stiffness = (double)StiffnessSlider->Position;
    object->setStiffness(stiffness, true);
    if (m_movingObject)
        m_movingObject->setStiffness(stiffness, true);

    // set static and dynamic friction
    double staticFriction = (double)StaticFrictionSlider->Position / 100.0;
    double dynamicFriction = (double)DynamicFrictionSlider->Position / 100.0;
    if (m_movingObject)
        m_movingObject->setFriction(staticFriction, dynamicFriction, true);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    // update options
    updateWorldSettings();

    // render world in display
    if (viewport != NULL)
    {
        viewport->render();
    }
}
//---------------------------------------------------------------------------

void HapticLoop(void* param)
{
    // simulation in now ON

    // read position from haptic device
    Form1->tool->updatePose();

    // compute forces
    Form1->tool->computeForces();

    // get last interaction force in global coordinate frame
    Form1->m_interactionForce = cMul(cTrans(Form1->object->getRot()),
         cSub(Form1->tool->m_lastComputedGlobalForce, Form1->object->getPos()));
    //Form1->tool->m_lastComputedGlobalForce;

    Form1->tool->applyForces();

    // figure out if we're touching the record
    cProxyPointForceAlgo * algo = Form1->tool->getProxy();
    if (algo->getContactObject() == Form1->m_movingObject)
    {
      if (! Form1->m_inContact)
      {
        Form1->m_inContact = true;
        Form1->m_RFDInitialAngle = Form1->m_rotPos - Form1->m_lastGoodPosition*CHAI_PI/180;
      }
      Form1->animateObject(Form1->m_interactionForce);

    }
    else
    {
      Form1->animateObject(cVector3d(0.0, 0.0, 0.0));
      Form1->m_inContact = false;
    }
}

//---------------------------------------------------------------------------


void __fastcall TForm1::ToggleHapticsButtonClick(TObject *Sender)
{
    if (!haptics_enabled)
    {
      if (tool == NULL)
      {
        tool = new cMeta3dofPointer(world, 0);
        world->addChild(tool);
      }

      tool->setPos(0.0, 0.0, 0.0);

      // turn on dynamic proxy
      cProxyPointForceAlgo* forceAlgo = tool->getProxy();
      forceAlgo->enableDynamicProxy(true);

      // set up a nice-looking workspace for the phantom so
      // it fits nicely with our models
      tool->setWorkspace(2.0,2.0,2.0);

      // Rotate the tool so its axes align with our opengl-like axes
      tool->setRadius(0.01);
      tool->setRenderingMode(0);

      // set up the device
      tool->initialize();

      // open communication to the device
      tool->start();

      // I need to call this so the tool can update its internal
      // transformations before performing collision detection, etc.
      tool->computeGlobalPositions();
      tool->setForcesON();

      timer.set(0,HapticLoop,this);
      haptics_enabled = true;

      // intialize clock
      m_time = new cPrecisionClock();
      m_time->initialize();

    }
    else
    {
      if (stream) BASS_ChannelStop(stream);
      timer.stop();
      haptics_enabled = false;
    }
}

//---------------------------------------------------------------------------

void TForm1::load_player()
{
    // create a new mesh
  cMesh* new_object = new cMesh(world);

  // load 3d object file
  int result = new_object->loadFromFile("resources\\models\\turntable.obj");

  // I'm going to scale the object so his maximum axis has a
  // size of MESH_SCALE_SIZE. This will make him fit nicely in
  // our viewing area.

  // Tell him to compute a bounding box...
  new_object->computeBoundaryBox(true);

  cVector3d min = new_object->getBoundaryMin();
  cVector3d max = new_object->getBoundaryMax();

  // This is the "size" of the object
  cVector3d span = max;
  span.sub(min);

  // Find his maximum dimension
  float max_size = span.x;
  if (span.y > max_size) max_size = span.y;
  if (span.z > max_size) max_size = span.z;

  // We'll center all vertices, then multiply by this amount,
  // to scale to the desired size.
  float scale_factor = MESH_SCALE_SIZE / max_size;

  // To center vertices, we add this amount (-1 times the
  // center of the object's bounding box)
  cVector3d offset = max;
  offset.add(min);
  offset.div(2.0);
  offset.negate();

  // Now we need to actually scale all the vertices.  However, the
  // vertices might not actually be in this object; they might
  // be in children or grand-children of this mesh (depending on how the 
  // model was defined in the file).
  // 
  // So we find all the sub-meshes we loaded from this file, by descending
  // through all available children.

  // This will hold all the meshes we need to operate on... we'll fill
  // it up as we find more children.
  std::list<cMesh*> meshes_to_scale;

  // This will hold all the parents we're still searching...
  std::list<cMesh*> meshes_to_descend;
  meshes_to_descend.push_front(new_object);

  // Keep track of how many meshes we've found, just to print
  // it out for the user
  int total_meshes = 0;

  // While there are still parent meshes to process
  while(meshes_to_descend.empty() == 0) {

    total_meshes++;

    // Grab the next parent
    cMesh* cur_mesh = meshes_to_descend.front();
    meshes_to_descend.pop_front();
    meshes_to_scale.push_back(cur_mesh);

    // Put all his children on the list of parents to process
    for(unsigned int i=0; i<cur_mesh->getNumChildren(); i++) {

      cGenericObject* cur_object = cur_mesh->getChild(i);

      // Only process cMesh children
      cMesh* cur_mesh = dynamic_cast<cMesh*>(cur_object);
      if (cur_mesh) meshes_to_descend.push_back(cur_mesh);
    }
  }
  
  std::list<cMesh*>::iterator mesh_iter;

  //_cprintf("Offset: %f %f %f\n", offset.x, offset.y, offset.z);

  // Now loop over _all_ the meshes we found...
  for(mesh_iter = meshes_to_scale.begin(); mesh_iter != meshes_to_scale.end(); mesh_iter++) {

    cMesh* cur_mesh = *mesh_iter;
    vector<cVertex>* vertices = cur_mesh->pVertices();
    int num_vertices = cur_mesh->getNumVertices(false);
    cVertex* cur_vertex = (cVertex*)(vertices);

    // Move and scale each vertex in this mesh...
    for(int i=0; i<num_vertices; i++) {
      cur_vertex = cur_mesh->getVertex(i);
      cVector3d pos = cur_vertex->getPos();      
      pos.add(offset);
      pos.mul(scale_factor);
      cur_vertex->setPos(pos);
      cur_vertex++;
    }
    cur_mesh->computeGlobalPositions(false);
  }

  int size = new_object->pTriangles()->size();

  // Re-compute a bounding box
  new_object->computeBoundaryBox(true);

  // Build a nice collision-detector for this object, so
  // the proxy will work nicely when haptics are enabled.
  new_object->computeGlobalPositions(false);
  // new_object->createSphereTreeCollisionDetector(true,true);
  new_object->createAABBCollisionDetector(true,true);

  new_object->computeGlobalPositions();
  object = new_object;
  world->addChild(object);
}
//---------------------------------------------------------------------------

// Load a record onto the record player
void TForm1::load_record(int a_index)
{
  int restart_haptics = 0;
  if (haptics_enabled) {
    restart_haptics = 1;
    ToggleHapticsButtonClick(this);
  }
  if (stream) { BASS_ChannelStop(stream); Sleep(1000); }
  pos = 0;

  switch (a_index) {
    case 0 : LoadWaveFile("resources\\sounds\\italy.mp3"); break;
    case 1 : LoadWaveFile("resources\\sounds\\switzerland.mp3"); break;
    case 2 : LoadWaveFile("resources\\sounds\\usa.mp3"); break;
    default : return;
  }

  stream=BASS_StreamCreate(info.freq,info.chans,0,&MyStreamWriter,0);
        
  // delete existing record
  if (m_movingObject) { world->removeChild(m_movingObject); delete m_movingObject; }

  // create new record
  m_movingObject = new cMesh(world);
  createTexCoords(m_movingObject, 0.33);
  cTexture2D *record = new cTexture2D();

  switch (a_index) {
    case 0 : record->loadFromFile("resources\\images\\italy.bmp"); break;
    case 1 : record->loadFromFile("resources\\images\\switzerland.bmp"); break;
    case 2 : record->loadFromFile("resources\\images\\usa.bmp"); break;
    default : return;
  }

  m_movingObject->setTexture(record);
  m_movingObject->useTexture(true, true);

  // compute size of object again
  m_movingObject->computeBoundaryBox(true);

  // Build a collision-detector for this object, so
  // the proxy will work nicely when haptics are enabled.
  m_movingObject->createAABBCollisionDetector(true, false);

  // set size of frame
  m_movingObject->setFrameSize(0.3, true);

  // update global position
  m_movingObject->computeGlobalPositions();

  // add object to world and translate
  world->addChild(m_movingObject);
  m_movingObject->translate(-0.1, 0, -0.21);

  // The stiffness will actualy be set by the GUI
  double stiffness = (double)0;
  if (m_movingObject)
    m_movingObject->setStiffness(stiffness, true);

  // set static and dynamic friction
  double staticFriction = (double)100 / 100.0;
  double dynamicFriction = (double)100 / 100.0;
  if (m_movingObject)
    m_movingObject->setFriction(staticFriction, dynamicFriction, true);

  m_rotVel = 0.0;
  if (restart_haptics) ToggleHapticsButtonClick(this);
}
//---------------------------------------------------------------------------

void TForm1::animateObject(cVector3d force)
{
    // do stuff with force: relate force to torque applied about the disc axis

    // integrate torque
    cVector3d planeForce;
    planeForce.set(force.x, force.y, 0);

    // get position of proxy on plane, get vector from center to application point
    cProxyPointForceAlgo* forceAlgo = tool->getProxy();
    m_proxyPos = forceAlgo->getProxyGlobalPosition();
    cVector3d radius;
    radius.set(m_proxyPos.x, m_proxyPos.y, 0);

    // do cross product
    cVector3d torqueVec;
    planeForce.crossr(radius, torqueVec);

    // call that the torque applied to the disc and compute new position
    m_torque = torqueVec.z;

    // do the math for how the rotational friction may affect the whole motion
    double time_step = 0.001;
    m_rotVel = m_rotVel + m_torque/m_inertia * time_step;
    m_rotPos = m_rotPos + m_rotVel * time_step;

    // set audio direction and frequency based on rotational velocity
    if (haptics_enabled && (fabs(m_rotVel)) > 0.0)
    {
      if (m_rotVel < 0.0) record_direction = 1;
      else record_direction = -1;
      BASS_ChannelSetAttributes(stream, (int)(info.freq*fabs(m_rotVel)/6.5), -1, -1);
      if (!(BASS_ChannelPlay(stream,FALSE)))
        Application->MessageBox("Playback Error", "Error");
    }
    else
    {
      BASS_ChannelStop(stream);
    }

    // rotate object
    m_movingObject->rotate(cVector3d(0,0,1), m_rotVel * time_step);
    m_movingObject->computeGlobalPositions();

}
//---------------------------------------------------------------------------

// Define the texture coordinates for the record
void createTexCoords(cMesh *a_mesh, double radius) {

  double vectorY1, originY, vectorX1, originX;
  vectorY1 = 0; originY = 0;
  vectorX1 = 0; originX = 0;
  cVector3d vector1, vector2, origin;
  origin.set(originX, originY, 0);
  vector2.set(originX, originY, 0);

  int num = 28;
  double divisore = num / (2*CHAI_PI);

  for(int i=0; i<=num; i++)
  {
    double angle=(float)(((double)i)/divisore);
    vector1.set(originX+(radius*(float)sin((double)angle)), originY+(radius*(float)cos((double)angle)), 0);
    a_mesh->newTriangle(origin, vector1, vector2);
    vector2.set(vector1.x, vector1.y, vector1.z);
  }

  for(unsigned int n=0; n<a_mesh->getNumVertices(); n++)
  {
    cVertex* curVertex = a_mesh->getVertex(n);
    curVertex->setTexCoord(
        (curVertex->getPos().x + radius) / (2.0 * radius),
        (curVertex->getPos().y + radius) / (2.0 * radius)
    );
    curVertex->setNormal(0,0,1);
  }

  // compute normals
  a_mesh->computeAllNormals(true);

  // compute boudary box
  a_mesh->computeBoundaryBox(true);
}



void __fastcall TForm1::RecordSelectChange(TObject *Sender)
{
  if (RecordSelect->ItemIndex >= 0)
    load_record(RecordSelect->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button1Click(TObject *Sender)
{
    exit(0);    
}
//---------------------------------------------------------------------------

