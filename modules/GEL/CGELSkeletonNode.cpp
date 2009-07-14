//===========================================================================
/*
    This file is part of the GEL dynamics engine.
    Copyright (C) 2003-2009 by Francois Conti, Stanford University.
    All rights reserved.

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   2.0.0 $Rev: 248 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CGELSkeletonNode.h"
//---------------------------------------------------------------------------

//===========================================================================
// DEFINITION - DEFAULT VALUES:
//===========================================================================

// Physical properties:
double cGELSkeletonNode::default_radius        = 0.05;
double cGELSkeletonNode::default_kDampingPos   = 5.00;
double cGELSkeletonNode::default_kDampingRot   = 0.1;
double cGELSkeletonNode::default_mass          = 0.1;  // [kg]

// Graphic properties:
bool cGELSkeletonNode::default_showFrame       = true;
cColorf cGELSkeletonNode::default_color(1.0, 0.4, 0.4);

// Gravity field:
bool cGELSkeletonNode::default_useGravity      = true;
cVector3d cGELSkeletonNode::default_gravity(0.00, 0.00, -9.81);


//===========================================================================
/*!
    Constructor of cGELSkeletonNode.

    \fn       cGELSkeletonNode::cGELSkeletonNode()
*/
//===========================================================================
cGELSkeletonNode::cGELSkeletonNode()
{
    m_pos.zero();
    m_rot.identity();
    m_radius        = default_radius;
    m_color         = default_color;
    m_externalForce.zero();
    m_force.zero();
    m_torque.zero();
    m_acc.zero();
    m_angAcc.zero();
    m_vel.zero();
    m_angVel.zero();
    m_kDampingPos   = default_kDampingPos;
    m_kDampingRot   = default_kDampingRot;
    m_gravity       = default_gravity;
    m_useGravity    = default_useGravity;
    m_fixed         = false;
    setMass(default_mass);
}


//===========================================================================
/*!
    Destructor of cGELSkeletonNode.

    \fn       cGELSkeletonNode::~cGELSkeletonNode()
*/
//===========================================================================
cGELSkeletonNode::~cGELSkeletonNode()
{

}


//===========================================================================
/*!
    Define the mass of the node. The inertia is computed accordingly.

    \fn       void cGELSkeletonNode::render()
*/
//===========================================================================
void cGELSkeletonNode::setMass(double a_mass)
{
    m_mass = a_mass;
    m_inertia = (2.0 / 5.0) * m_mass * m_radius * m_radius ;
}


