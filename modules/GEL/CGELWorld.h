//===========================================================================
/*
    This file is part of the GEL dynamics engine.
    Copyright (C) 2003-2009 by Francois Conti, Stanford University.
    All rights reserved.

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   2.0.0 $Rev: 251 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CGELWorldH
#define CGELWorldH
//---------------------------------------------------------------------------
#include "chai3d.h"
#include "CGELMesh.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CGELWorld.h

    \brief 
    <b> GEL Module </b> \n 
    Virtual World.
*/
//===========================================================================

//===========================================================================
/*!
    \class      cGELWorld
    \ingroup    GEL

    \brief      
    cGELWorld implements a world to handle deformable objects within CHAI 3D.
*/
//===========================================================================
class cGELWorld : public cGenericObject
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cGELWorld.
    cGELWorld();

    //! Destructor of cGELWorld.
    ~cGELWorld();


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Compute dynamic simulation.
    void updateDynamics(double a_time);

    //! Clear external forces on all objects.
    void clearExternalForces();

    //! Update vertices of all objects.
    void updateSkins();


	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! List of deformable solids.
    list<cGELMesh*> m_gelMeshes;

    //! Current time of simulation.
    double m_simulationTime;

    //! Integration time.
    double m_integrationTime;

    //! Gravity constant.
    cVector3d m_gravity;


  private:

	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Render deformable mesh.
    virtual void render(const int a_renderMode=CHAI_RENDER_MODE_RENDER_ALL);
};


//===========================================================================
/*!
    \class      cGELWorldCollision
    \ingroup    GEL

    \brief      
    cGELWorldCollision provides a collision detection model to support 
    deformable objects.
*/
//===========================================================================
class cGELWorldCollision : public cGenericCollision
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cGELWorldCollision.
    cGELWorldCollision(cGELWorld* a_gelWorld) {m_gelWorld = a_gelWorld;}

    //! Destructor of cGELWorldCollision.
    virtual ~cGELWorldCollision() {};


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Do any necessary initialization, such as building trees.
    virtual void initialize() {};

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

    //! Deformable world
    cGELWorld* m_gelWorld;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


