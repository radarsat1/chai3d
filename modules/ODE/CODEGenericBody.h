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
#ifndef CODEGenericBodyH
#define CODEGenericBodyH
//---------------------------------------------------------------------------
#include "ode/ode.h"
#include "chai3d.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CODEGenericBody.h

    \brief 
    <b> ODE Module </b> \n 
    ODE Generic Object.
*/
//===========================================================================

//---------------------------------------------------------------------------
class cODEWorld;
//---------------------------------------------------------------------------
//! ODE geometry used for dynamic collision computation.
enum cODEDynamicModelType
{
    ODE_MODEL_BOX,
    ODE_MODEL_SPHERE,
    ODE_MODEL_CYLINDER,
    ODE_MODEL_PLANE,
    ODE_MODEL_TRIMESH
};
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \class      cODEGenericBody
    \ingroup    ODE

    \brief      
    cODEGenericBody is a base class for modeling any ODE dynamic body.
*/
//===========================================================================
class cODEGenericBody : public cGenericObject
{

  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cODEGenericBody.
		cODEGenericBody(cODEWorld* a_world) { initialize(a_world); }

    //! Destructor of cODEGenericBody.
    virtual ~cODEGenericBody() {};


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Set the position of object.
    void setPosition(cVector3d &a_position);

    //! Set the orientation of object.
    void setRotation(cMatrix3d &a_rotation);

    //! Update position and orientation from ode model to chai model.
    void updateBodyPosition(void);

    //! Apply an external force at a given position decribed in global coordinates.
    void addGlobalForceAtGlobalPos(cVector3d& a_force, cVector3d& a_pos);

    //! Set object mass.
    void setMass(double a_mass);

    //! Read object mass.
    double getMass() { return (m_mass); }

    //! Set a chai3D body image of object.
    void setImageModel(cGenericObject* a_imageModel);

    //! Get body image.
    cGenericObject* getImageModel() { return (m_imageModel); }

    //! Enable object from dynamic update.
    void enableDynamics();

    //! Disable object from dynamic update.
    void disableDynamics();

    //! Is the current object static? (cannot move).
    bool isStatic() { return (m_static); } 

    //! Create a dynamic model of the object.
	virtual void buildDynamicModel() {};

    //! Create a dynamic box representation.
    void createDynamicBoundingBox(bool a_staticObject = false);

	//! Create a dynamic sphere representation.
	void createDynamicSphere(const double a_radius, 
						 	 bool a_staticObject = false,
                             const cVector3d& a_offsetPos = cVector3d(0.0, 0.0, 0.0),
                             const cMatrix3d& a_offsetRot = cIdentity3d());

	//! Create a dynamic box representation.
	void createDynamicBox(const double a_lengthX, const double a_lengthY, const double a_lengthZ,
                          bool a_staticObject = false,
						  const cVector3d& a_offsetPos = cVector3d(0.0, 0.0, 0.0),
                          const cMatrix3d& a_offsetRot = cIdentity3d());

	//! Create a dynamic caped cylinder (capsule) representation.
	void createDynamicCapsule(const double a_radius, const double a_length,
                              bool a_staticObject = false,
						      const cVector3d& a_offsetPos = cVector3d(0.0, 0.0, 0.0),
                              const cMatrix3d& a_offsetRot = cIdentity3d());

	//! Create a static plane representation.
	void createStaticPlane(const cVector3d& a_position,
                           const cVector3d& a_normal);

	//! Create a triangle mesh representation.
	void createDynamicMesh(bool a_staticObject = false,
                           const cVector3d& a_offsetPos = cVector3d(0.0, 0.0, 0.0),
                           const cMatrix3d& a_offsetRot = cIdentity3d());

    //! Update global position frames.
    void updateGlobalPositions(const bool a_frameOnly);

    //! Render object in OpenGL.
    void render(const int a_renderMode);

    //! Show collision model.
    void setShowDynamicCollisionModel(const bool a_show) { m_showDynamicCollisionModel = a_show; }

    //! Read show collision model status.
    bool getShowDynamicCollisionModel() { return(m_showDynamicCollisionModel); }

    //! Parent world.
    cODEWorld* m_ODEWorld;

    //! Object used to represent the geometry and graphical properties of the object.
    cGenericObject* m_imageModel;


  private:

	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! if \b true then ODE object is static and does not move. 
    bool m_static;

    //! ODE body.
    dBodyID m_ode_body;

    //! ODE body mass matrix.
    dMass m_ode_mass;

    //! Mass of object units: [kg].
    double m_mass;

    //! ODE body geometry.
    dGeomID m_ode_geom;

    //! ODE vertices (for triangle mesh models) - Do not use doubles since not supported under ODE! 
    float* m_vertices;

    //! ODE indices (for triangle mesh models).
    int* m_vertexIndices;

    //! ODE previous tri mesh position and orientation.
    double m_prevTransform[16];

    //! ODE tri mesh ID.
    dTriMeshDataID m_ode_triMeshDataID;

    //! Enable/Disable graphical representation of collision model.
    bool m_showDynamicCollisionModel;

    //! Color use to render collision model.
    cColorf m_colorDynamicCollisionModel;

    //! Dynamic model type.
    cODEDynamicModelType m_typeDynamicCollisionModel;

    /*! 
		Variables which store the parameters of the collision model.
        Depending of the dynamic model type, these value correspond to
        lengths or radius.
	*/
    double m_paramDynColModel0;
    double m_paramDynColModel1;
    double m_paramDynColModel2;
    cVector3d m_posOffsetDynColModel;
    cMatrix3d m_rotOffsetDynColModel;

    //! Build table of triangles and vertices for ODE mesh representation.
    int buildMeshTable(cMesh* a_mesh, int& a_indexOffset, int& a_verticesCount, int& a_indexCount);

    //! Initialize ODE body.
    void initialize(cODEWorld* a_world);

    //! Compute collision with object geometry.
    virtual bool computeOtherCollisionDetection(cVector3d& a_segmentPointA,
                                                cVector3d& a_segmentPointB,
                                                cCollisionRecorder& a_recorder,
                                                cCollisionSettings& a_settings);
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
