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
    \author:    Francois Conti
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CDraw3D.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      Draw an X-Y-Z Frame. Red arrow corresponds to X-Axis, Green to Y-Axis
      and Blue to Z-Axis. The Scale parameter defines the size of the arrows.

      \fn       void cDrawFrame(const double a_scale)
      \param    a_scale  Length of each arrow (x, Y and Z)
*/
//===========================================================================
void cDrawFrame(const double a_scale)
{
  static double Vertices[25][3] =
  {{ 0.0, 0.010000, 0.0 },
  { 0.007000, 0.007000, -0.007000 },
  {  0.010000, 0.0, 0.0 },
  {  0.007000, -0.007000, -0.007000 },
  {  0.0, -0.010000, 0.0 },
  {  -0.007000, -0.007000, 0.007000 },
  {  -0.010000, 0.0, 0.0 },
  {  -0.007000, 0.007000, -0.007000 },
  {  0.0, 0.010000, -0.800000 },
  {  0.007000, 0.007000, -0.800000 },
  {  0.010000, 0.0, -0.800000 },
  {  0.007000, -0.007000, -0.800000 },
  {  0.0, -0.010000, -0.800000 },
  {  -0.007000, -0.007000, -0.800000 },
  {  -0.010000, 0.0, -0.800000 },
  {  -0.007000, 0.007000, -0.800000 },
  {  0.0, 0.040000, -0.800000 },
  {  0.028284, 0.028284, -0.800000 },
  {  0.040000, 0.0, -0.800000 },
  {  0.028284, -0.028284, -0.800000 },
  {  0.0, -0.040000, -0.800000 },
  {  -0.028284, -0.028284, -0.800000 },
  {  -0.040000, 0.0, -0.800000 },
  {  -0.028284, 0.028284, -0.800000 },
  {  0.0, 0.0, -1.0 } };

  static double Normals[25][3] =
  {{  -1.0, 0.0, 0.0 },
  {  -0.980581, 0.0, -0.196116 },
  { -0.862856, -0.357407, -0.357407 },
  {  -0.862856, 0.357407, -0.357407 },
  {  -0.707107, -0.707107, 0.0 },
  {  -0.707107, 0.707107, 0.0 },
  {   -0.693375, -0.693375, -0.196116 },
  {   -0.693375, 0.693375, -0.196116 },
  {   -0.357407, -0.862856, -0.357407 },
  {   -0.357407, 0.862856, -0.357407 },
  {   0.0, 0.980581, -0.196116 },
  {   0.0, -1.0, 0.0 },
  {   0.0, 0.0, 1.0 },
  {   0.0, 1.0, 0.0 },
  {   0.0, -0.980581, -0.196116 },
  {   0.357407, 0.862856, -0.357407 },
  {   0.357407,-0.862856, -0.357407 },
  {   0.693375, -0.693375, -0.196116 },
  {   0.693375, 0.693375, -0.196116 },
  {   0.707107, -0.707107, 0.0 },
  {   0.707107, 0.707107, 0.0 },
  {   0.862856, -0.357407, -0.357407 },
  {   0.862856, 0.357407, -0.357407 },
  {   0.980581, 0.0, -0.196116 },
  {   1.0, 0.0, 0.0 }};

  static int Face3[8][6] =
  {{  16,10,17,18,24,15 },
  {  17,18,18,23,24,22 },
  {  18,23,19,17,24,21 },
  {  19,17,20,14,24,16 },
  {  20,14,21,6,24,8 },
  {  21,6,22,1,24,2 },
  {  22,1,23,7,24,3 },
  {  23,7,16,10,24,9 }};

  static int Face4[16][8] =
  {{ 0,13,1,20,9,20,8,13 },
  {  4,11,5,4,13,4,12,11 },
  {  5,4,6,0,14,0,13,4 },
  {  6,0,7,5,15,5,14,0 },
  {  7,5,0,13,8,13,15,5},
  {  1,20,2,24,10,24,9,20 },
  {  2,24,3,19,11,19,10,24 },
  {  3,19,4,11,12,11,11,19 },
  {  15,12,23,12,22,12,14,12 },
  {  14,12,22,12,21,12,13,12 },
  {  13,12,21,12,20,12,12,12 },
  {  12,12,20,12,19,12,11,12 },
  {  19,12,18,12,10,12,11,12 },
  {  18,12,17,12,9,12,10,12 },
  {  9,12,17,12,16,12,8,12 },
  {  8,12,16,12,23,12,15,12 }};

  // declare temps variables.
  int i;

  // Set rotation matrices for arrows X, Y and Z:
  cMatrix3d rotationX;
  rotationX.identity();
  rotationX.rotate( cVector3d(0, 1, 0), -CHAI_PI / 2.0);

  cMatrix3d rotationY;
  rotationY.identity();
  rotationY.rotate( cVector3d(1, 0, 0), CHAI_PI / 2.0);

  cMatrix3d rotationZ;
  rotationZ.identity();
  rotationZ.rotate( cVector3d(1, 0, 0), CHAI_PI);

  // Set OpenGL material mode
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // DRAW AXIS X:
  // Set pose
  cMatrixGL matAxis;
  matAxis.set(rotationX);
  matAxis.glMatrixPushMultiply();

  // Set color (red)
  glColor3f(1.0, 0.0, 0.0);

  // render polygons
  for (i=0; i<8; i++)
  {
    glBegin(GL_TRIANGLES);
      //glNormal3dv((const double *)&Normals[Face3[i][1]][0]);
      glNormal3d( Normals[Face3[i][1]][0],
                  Normals[Face3[i][1]][1],
                  Normals[Face3[i][1]][2]);

      glVertex3d((a_scale * Vertices[Face3[i][0]][0]),
                 (a_scale * Vertices[Face3[i][0]][1]),
                 (a_scale * Vertices[Face3[i][0]][2]));
      //glNormal3dv((const double *)&Normals[Face3[i][3]][0]);
      glNormal3d( Normals[Face3[i][3]][0],
                  Normals[Face3[i][3]][1],
                  Normals[Face3[i][3]][2]);

      glVertex3d((a_scale * Vertices[Face3[i][2]][0]),
                 (a_scale * Vertices[Face3[i][2]][1]),
                 (a_scale * Vertices[Face3[i][2]][2]));
      //glNormal3dv((const double *)&Normals[Face3[i][5]][0]);
      glNormal3d( Normals[Face3[i][5]][0],
                  Normals[Face3[i][5]][1],
                  Normals[Face3[i][5]][2]);

      glVertex3d((a_scale * Vertices[Face3[i][4]][0]),
                 (a_scale * Vertices[Face3[i][4]][1]),
                 (a_scale * Vertices[Face3[i][4]][2]));
    glEnd();
  }

  for (i=0; i<16; i++)
  {
    glBegin(GL_POLYGON);
      glNormal3dv((const double *)&(Normals[Face4[i][1]][0]));
      glVertex3d((a_scale * Vertices[Face4[i][0]][0]),
                 (a_scale * Vertices[Face4[i][0]][1]),
                 (a_scale * Vertices[Face4[i][0]][2]));
      glNormal3dv((const double *)&(Normals[Face4[i][3]][0]));
      glVertex3d((a_scale * Vertices[Face4[i][2]][0]),
                 (a_scale * Vertices[Face4[i][2]][1]),
                 (a_scale * Vertices[Face4[i][2]][2]));
      glNormal3dv((const double *)&(Normals[Face4[i][5]][0]));
      glVertex3d((a_scale * Vertices[Face4[i][4]][0]),
                 (a_scale * Vertices[Face4[i][4]][1]),
                 (a_scale * Vertices[Face4[i][4]][2]));
      glNormal3dv((const double *)&(Normals[Face4[i][7]][0]));
      glVertex3d((a_scale * Vertices[Face4[i][6]][0]),
                 (a_scale * Vertices[Face4[i][6]][1]),
                 (a_scale * Vertices[Face4[i][6]][2]));
    glEnd();
  }

  matAxis.glMatrixPop(); // Rotation X.


  // DRAW AXIS Y:
  // Set pose
  matAxis.set(rotationY);
  matAxis.glMatrixPushMultiply();

  // Set color (green)
  glColor3f(0.0, 1.0, 0.0);

  // render polygons
  for (i=0; i<8; i++)
  {
    glBegin(GL_TRIANGLES);
      glNormal3dv((const double *)&Normals[Face3[i][1]]);
      glVertex3d((a_scale * Vertices[Face3[i][0]][0]),
                 (a_scale * Vertices[Face3[i][0]][1]),
                 (a_scale * Vertices[Face3[i][0]][2]));
      glNormal3dv((const double *)&Normals[Face3[i][3]]);
      glVertex3d((a_scale * Vertices[Face3[i][2]][0]),
                 (a_scale * Vertices[Face3[i][2]][1]),
                 (a_scale * Vertices[Face3[i][2]][2]));
      glNormal3dv((const double *)&Normals[Face3[i][5]]);
      glVertex3d((a_scale * Vertices[Face3[i][4]][0]),
                 (a_scale * Vertices[Face3[i][4]][1]),
                 (a_scale * Vertices[Face3[i][4]][2]));
    glEnd();
  }

  for (i=0; i<16; i++)
  {
    glBegin(GL_POLYGON);
      glNormal3dv((const double *)&Normals[Face4[i][1]]);
      glVertex3d((a_scale * Vertices[Face4[i][0]][0]),
                 (a_scale * Vertices[Face4[i][0]][1]),
                 (a_scale * Vertices[Face4[i][0]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][3]]);
      glVertex3d((a_scale * Vertices[Face4[i][2]][0]),
                 (a_scale * Vertices[Face4[i][2]][1]),
                 (a_scale * Vertices[Face4[i][2]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][5]]);
      glVertex3d((a_scale * Vertices[Face4[i][4]][0]),
                 (a_scale * Vertices[Face4[i][4]][1]),
                 (a_scale * Vertices[Face4[i][4]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][7]]);
      glVertex3d((a_scale * Vertices[Face4[i][6]][0]),
                 (a_scale * Vertices[Face4[i][6]][1]),
                 (a_scale * Vertices[Face4[i][6]][2]));
    glEnd();
  }

  matAxis.glMatrixPop(); // Rotation Y.

  // DRAW AXIS Z:
  // set pose
  matAxis.set(rotationZ);
  matAxis.glMatrixPushMultiply();

  // set color (blue)
  glColor3f(0.0, 0.0, 1.0);

  // render polygons
  for (i=0; i<8; i++)
  {
    glBegin(GL_TRIANGLES);
      glNormal3dv((const double *)&Normals[Face3[i][1]]);
      glVertex3d((a_scale * Vertices[Face3[i][0]][0]),
                 (a_scale * Vertices[Face3[i][0]][1]),
                 (a_scale * Vertices[Face3[i][0]][2]));
      glNormal3dv((const double *)&Normals[Face3[i][3]]);
      glVertex3d((a_scale * Vertices[Face3[i][2]][0]),
                 (a_scale * Vertices[Face3[i][2]][1]),
                 (a_scale * Vertices[Face3[i][2]][2]));
      glNormal3dv((const double *)&Normals[Face3[i][5]]);
      glVertex3d((a_scale * Vertices[Face3[i][4]][0]),
                 (a_scale * Vertices[Face3[i][4]][1]),
                 (a_scale * Vertices[Face3[i][4]][2]));
    glEnd();
  }

  for (i=0; i<16; i++)
  {
    glBegin(GL_POLYGON);
      glNormal3dv((const double *)&Normals[Face4[i][1]]);
      glVertex3d((a_scale * Vertices[Face4[i][0]][0]),
                 (a_scale * Vertices[Face4[i][0]][1]),
                 (a_scale * Vertices[Face4[i][0]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][3]]);
      glVertex3d((a_scale * Vertices[Face4[i][2]][0]),
                 (a_scale * Vertices[Face4[i][2]][1]),
                 (a_scale * Vertices[Face4[i][2]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][5]]);
      glVertex3d((a_scale * Vertices[Face4[i][4]][0]),
                 (a_scale * Vertices[Face4[i][4]][1]),
                 (a_scale * Vertices[Face4[i][4]][2]));
      glNormal3dv((const double *)&Normals[Face4[i][7]]);
      glVertex3d((a_scale * Vertices[Face4[i][6]][0]),
                 (a_scale * Vertices[Face4[i][6]][1]),
                 (a_scale * Vertices[Face4[i][6]][2]));
    glEnd();
  }

  matAxis.glMatrixPop();
}


//===========================================================================
/*!
      Draw a line based box with sides parallel to the x-y-z axis.

      \fn       void cDrawWireBox(const double a_xMin, const double a_xMax,
                const double a_yMin, const double a_yMax,
                const double a_zMin, const double a_zMax)
      \param    a_xMin  Box side x min position.
      \param    a_xMax  Box side x max position.
      \param    a_yMax  Box side y min position.
      \param    a_yMax  Box side y max position.
      \param    a_zMax  Box side z min position.
      \param    a_zMax  Box side z max position.
*/
//===========================================================================
void cDrawWireBox(const double a_xMin, const double a_xMax,
                  const double a_yMin, const double a_yMax,
                  const double a_zMin, const double a_zMax)
{
    // render lines for each edge of the box
    glBegin(GL_LINES);
        glVertex3d(a_xMin, a_yMin, a_zMin);
        glVertex3d(a_xMax, a_yMin, a_zMin);
        glVertex3d(a_xMin, a_yMax, a_zMin);
        glVertex3d(a_xMax, a_yMax, a_zMin);
        glVertex3d(a_xMin, a_yMin, a_zMax);
        glVertex3d(a_xMax, a_yMin, a_zMax);
        glVertex3d(a_xMin, a_yMax, a_zMax);
        glVertex3d(a_xMax, a_yMax, a_zMax);

        glVertex3d(a_xMin, a_yMin, a_zMin);
        glVertex3d(a_xMin, a_yMax, a_zMin);
        glVertex3d(a_xMax, a_yMin, a_zMin);
        glVertex3d(a_xMax, a_yMax, a_zMin);
        glVertex3d(a_xMin, a_yMin, a_zMax);
        glVertex3d(a_xMin, a_yMax, a_zMax);
        glVertex3d(a_xMax, a_yMin, a_zMax);
        glVertex3d(a_xMax, a_yMax, a_zMax);

        glVertex3d(a_xMin, a_yMin, a_zMin);
        glVertex3d(a_xMin, a_yMin, a_zMax);
        glVertex3d(a_xMax, a_yMin, a_zMin);
        glVertex3d(a_xMax, a_yMin, a_zMax);
        glVertex3d(a_xMin, a_yMax, a_zMin);
        glVertex3d(a_xMin, a_yMax, a_zMax);
        glVertex3d(a_xMax, a_yMax, a_zMin);
        glVertex3d(a_xMax, a_yMax, a_zMax);
    glEnd();
}


//===========================================================================
/*!
      Render an sphere given a radius.

      \fn       void cDrawSphere(const double a_radius,
                const unsigned int a_numSlices, const unsigned int a_numStacks)
      \param    a_radius  Radius of sphere.
      \param    a_numSlices  Specifies the number of subdivisions around the
                             z axis (similar to lines of longitude).
      \param    a_numStacks  Specifies the number of subdivisions along the
                             z axis (similar to lines of latitude).
*/
//===========================================================================
void cDrawSphere(const double a_radius,
                 const unsigned int a_numSlices, const unsigned int a_numStacks)
{
    // allocate new object in OpenGL to render a sphere
    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric ();

    // set rendering style
    gluQuadricDrawStyle (quadObj, GLU_FILL);

    // set normals rendering mode
    gluQuadricNormals (quadObj, GLU_SMOOTH);

    // render sphere
    gluSphere(quadObj, a_radius, a_numSlices, a_numStacks);

    // delete object
    gluDeleteQuadric(quadObj);
}


