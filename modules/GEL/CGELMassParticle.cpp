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
#include "CGELMassParticle.h"
//---------------------------------------------------------------------------

//===========================================================================
// DEFINITION - DEFAULT VALUES:
//===========================================================================

// Physical properties:
double cGELMassParticle::default_kDampingPos   = 5.00;
double cGELMassParticle::default_mass          = 0.1;  // [kg]

// Graphic properties:
cColorf cGELMassParticle::default_color(1.0, 1.0, 0.4);

// Gravity field:
bool cGELMassParticle::default_useGravity      = true;
cVector3d cGELMassParticle::default_gravity(0.00, 0.00, -9.81);

// Default rendering mode
bool cGELMassParticle::show_forces = true;
bool cGELMassParticle::scale_force_vector_display = 0.01;


//===========================================================================
/*!
    Constructor of cGELMassParticle.

    \fn       cGELMassParticle::cGELMassParticle()
*/
//===========================================================================
cGELMassParticle::cGELMassParticle()
{
    m_pos.zero();
    m_color         = default_color;
    m_externalForce.zero();
    m_force.zero();
    m_acc.zero();
    m_vel.zero();
    m_kDampingPos   = default_kDampingPos;
    m_gravity       = default_gravity;
    m_useGravity    = default_useGravity;
    m_fixed         = false;
    setMass(default_mass);
}


//===========================================================================
/*!
    Destructor of cGELMassParticle.

    \fn       cGELMassParticle::~cGELMassParticle()
*/
//===========================================================================
cGELMassParticle::~cGELMassParticle()
{

}


//===========================================================================
/*!
    Define the mass of the mass particle

    \fn       void cGELMassParticle::render()
*/
//===========================================================================
void cGELMassParticle::setMass(double a_mass)
{
    m_mass = a_mass;
}


