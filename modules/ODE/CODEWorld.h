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
    \version   2.0.0 $Rev: 251 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CODEWorldH
#define CODEWorldH
//---------------------------------------------------------------------------
#include "chai3d.h"
#include "CODEGenericBody.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CODEWorld.h

    \brief 
    <b> ODE Module </b> \n 
    ODE World.
*/
//===========================================================================


//===========================================================================
/*!
    \class      cODEWorld
    \ingroup    ODE

    \brief      
    cODEWorld implements a virtual world to handle ODE based objects 
    (cODEGenericBody).
*/
//===========================================================================
class cODEWorld : public cGenericObject
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cODEWorld.
    cODEWorld(cWorld* a_parentWorld);

    //! Destructor of cODEWorld.
    ~cODEWorld();

    // PROPERTIES:
    //! List of ODE dynamic bodies.
    list<cODEGenericBody*> m_bodies;


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Set gravity field.
    void setGravity(cVector3d a_gravity);

    //! Read gravity field.
    cVector3d getGravity();

    //! Set linear damping.
    void setLinearDamping(double a_value) { dWorldSetLinearDamping(m_ode_world, a_value); }

    //! Set angular damping.
    void setAngularDamping(double a_value) { dWorldSetAngularDamping(m_ode_world, a_value); } 

    //! Set max angular speed.
    void setMaxAngularSpeed(double a_value) { dWorldSetMaxAngularSpeed(m_ode_world, a_value); }

    //! compute dynamic simulation.
    void updateDynamics(double a_interval);

    //! update position and orientation from ode models to chai models.
    void updateBodyPositions(void);

    // update global position frames.
    void updateGlobalPositions(const bool a_frameOnly);


	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! ODE dynamics world.
    dWorldID m_ode_world;

    //! ODE collision space.
    dSpaceID m_ode_space;

	//! ODE contact group.
	dJointGroupID m_ode_contactgroup;


  private:

	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! current time of simulation.
    double m_simulationTime;

    //! Parent chai3d world.
	cWorld* m_parentWorld;


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

	//! ODE collision callback.
	static void nearCallback (void *data, dGeomID o1, dGeomID o2);

    //! Render deformable mesh (OpenGL).
    virtual void render(const int a_renderMode=CHAI_RENDER_MODE_RENDER_ALL);
};


//===========================================================================
/*!
      \class      cODEWorldCollision
      \brief      cODEWorldCollision provides a collision detection model
                  to support ODE objects modeled under the CHAI framework
*/
//===========================================================================
class cODEWorldCollision : public cGenericCollision
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cODEWorldCollision.
    cODEWorldCollision(cODEWorld* a_ODEWorld) {m_ODEWorld = a_ODEWorld;}

    //! Destructor of cODEWorldCollision.
    virtual ~cODEWorldCollision() {};


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Do any necessary initialization, such as building trees.
    virtual void initialize(double a_radius) {};

    //! Provide a visual representation of the method.
    virtual void render() {};

    //! Return the nearest triangle intersected by the given segment, if any.
    virtual bool computeCollision(cVector3d& a_segmentPointA,
                                  cVector3d& a_segmentPointB,
                                  cCollisionRecorder& a_recorder,
                                  cCollisionSettings& a_settings);


  private:

	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! ODE world.
    cODEWorld* m_ODEWorld;
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
