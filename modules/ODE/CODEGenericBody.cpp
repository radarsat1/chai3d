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
#include "CODEGenericBody.h"
//---------------------------------------------------------------------------
#include "CODEWorld.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Initialise ODE body

    \fn       void cODEGenericBody::initialize(cODEWorld* a_world)
    \param    a_world  World to which this new object belongs to.
*/
//===========================================================================
void cODEGenericBody::initialize(cODEWorld* a_world)
{
	// some default mass
	m_mass = 1.0;

    // store parent world
    m_ODEWorld = a_world;

	// no body geometry defined yet
	m_imageModel = new cGenericObject();

    // add body to world
    m_ODEWorld->m_bodies.push_back(this);

    // init ODE data
    m_ode_triMeshDataID = NULL;
    m_ode_body = NULL;

    m_prevTransform[0] = 1.0;
    m_prevTransform[1] = 0.0;
    m_prevTransform[2] = 0.0;
    m_prevTransform[3] = 0.0;
    m_prevTransform[4] = 0.0;
    m_prevTransform[5] = 1.0;
    m_prevTransform[6] = 0.0;
    m_prevTransform[7] = 0.0;
    m_prevTransform[8] = 0.0;
    m_prevTransform[9] = 0.0;
    m_prevTransform[10] = 1.0;
    m_prevTransform[11] = 0.0;
    m_prevTransform[12] = 0.0;
    m_prevTransform[13] = 0.0;
    m_prevTransform[14] = 0.0;
    m_prevTransform[15] = 1.0;

    // set a default color that shall be used to render the dynamic collision model
    m_colorDynamicCollisionModel.set(1.0, 1.0, 0.0);

    // hide dynamic collision model
    m_showDynamicCollisionModel = false;

    // set default values
    m_posOffsetDynColModel.zero();
    m_rotOffsetDynColModel.identity();
    m_paramDynColModel0 = 0.0;
    m_paramDynColModel1 = 0.0;
    m_paramDynColModel2 = 0.0;
}


//===========================================================================
/*!
    Set the mass of current object

    \fn       void cODEGenericBody::setMass(double a_mass)
    \param    a_mass  New object mass.
*/
//===========================================================================
void cODEGenericBody::setMass(double a_mass)
{
    // check if ODE body defined
    if (m_ode_body == NULL) { return; }

    // store value
    m_mass = a_mass;

    // adjust mass
    dMassAdjust(&m_ode_mass, a_mass);
	dBodySetMass(m_ode_body,&m_ode_mass);
}


//===========================================================================
/*!
    Set a desired position.

    \fn       void cODEGenericBody::setPosition(cVector3d &a_position);
    \param    a_position  New desired position.
*/
//===========================================================================
void cODEGenericBody::setPosition(cVector3d &a_position)
{
    // check if body defined
    if (m_ode_body != NULL)
    {
        // store value
        m_localPos = a_position;

        // adjust position
        dBodySetPosition(m_ode_body, a_position.x, a_position.y, a_position.z);
    }
    else if (m_ode_geom != NULL)
    {
        // store value
        m_localPos = a_position;

        // adjust position
        dGeomSetPosition(m_ode_geom, a_position.x, a_position.y, a_position.z);
    }
}


//===========================================================================
/*!
    Set the orientation of object.

    \fn       void cODEGenericBody::setRotation(cMatrix3d &a_rotation);
    \param    a_rotation  New desired orientation.
*/
//===========================================================================
void cODEGenericBody::setRotation(cMatrix3d &a_rotation)
{
    // apply new rotation to ODE body
	dMatrix3 R;

	R[0]  = a_rotation.m[0][0];
	R[1]  = a_rotation.m[0][1];
	R[2]  = a_rotation.m[0][2];
	R[4]  = a_rotation.m[1][0];
	R[5]  = a_rotation.m[1][1];
	R[6]  = a_rotation.m[1][2];
	R[8]  = a_rotation.m[2][0];
	R[9]  = a_rotation.m[2][1];
	R[10] = a_rotation.m[2][2];

    // check if body defined
    if (m_ode_body != NULL)
    {
        // store new rotation matrix
        m_localRot = a_rotation;
        dBodySetRotation(m_ode_body, R);
    }
    else if (m_ode_geom != NULL)
    {
        // store new rotation matrix
        m_localRot = a_rotation;
        dGeomSetRotation(m_ode_geom, R);
    }
}


//===========================================================================
/*!
    Apply an external force at a given position. Position and force
    are expressed in global coordinates.

    \fn       void cODEGenericBody::addGlobalForceAtGlobalPos(cVector3d& a_force,
                                                            cVector3d& a_pos)
    \param    a_force  Force vector to apply on body
    \param    a_pos    Position (in global coordinates) when force is applied.
*/
//===========================================================================
void cODEGenericBody::addGlobalForceAtGlobalPos(cVector3d& a_force, cVector3d& a_pos)
{
    if (m_ode_body != NULL)
    {
        dBodyAddForceAtPos(m_ode_body,
                        a_force.x, a_force.y, a_force.z,
                        a_pos.x, a_pos.y, a_pos.z);
    }
}


//===========================================================================
/*!
    Update position and orientation from ODE models to chai models.

    \fn       void cODEGenericBody::updateBodyPosition(void)
*/
//===========================================================================
void cODEGenericBody::updateBodyPosition(void)
{
    const double *odePosition;
    const double *odeRotation;

    // Retrieve position and orientation from ODE body.
    if (m_ode_body != NULL)
    {
        odePosition =  dBodyGetPosition(m_ode_body);
        odeRotation =  dBodyGetRotation(m_ode_body);
    }
    else
    {
        return;
    }

    // set new position
    m_localPos.set(odePosition[0],odePosition[1],odePosition[2]);

    // set new orientation
	m_localRot.set(odeRotation[0],odeRotation[1],odeRotation[2],
                odeRotation[4],odeRotation[5],odeRotation[6],
                odeRotation[8],odeRotation[9],odeRotation[10]);

    // store previous position if object is a mesh
    if (m_ode_triMeshDataID != NULL)
    {
        m_prevTransform[0] = odeRotation[0];
        m_prevTransform[1] = odeRotation[4];
        m_prevTransform[2] = odeRotation[8];
        m_prevTransform[3] = 0.0;
        m_prevTransform[4] = odeRotation[1];
        m_prevTransform[5] = odeRotation[5];
        m_prevTransform[6] = odeRotation[9];
        m_prevTransform[7] = 0.0;
        m_prevTransform[8] = odeRotation[2];
        m_prevTransform[9] = odeRotation[6];
        m_prevTransform[10] = odeRotation[10];
        m_prevTransform[11] = 0.0;
        m_prevTransform[12] = odePosition[0];
        m_prevTransform[13] = odePosition[1];
        m_prevTransform[14] = odePosition[2];
        m_prevTransform[15] = 1.0;

        dGeomTriMeshSetLastTransform(m_ode_geom, m_prevTransform);
    }

	// Normalize frame
	// This can be useful is ODE is running in SINGLE precision mode
	// where precision is a problem
	/*
	cVector3d c0(odeRotation[0], odeRotation[4], odeRotation[8]);
	cVector3d c1(odeRotation[1], odeRotation[5], odeRotation[9]);
	cVector3d c2(odeRotation[2], odeRotation[6], odeRotation[10]);
	c0.crossr(c1, c2);
	c2.crossr(c0, c1);
	c0.normalize();
	c1.normalize();
	c2.normalize();

	// set new orientation
	m_localRot.setCol(c0, c1, c2);
	*/
}


//===========================================================================
/*!
    Compute collision detection between a ray and body image.

    \fn       bool cODEGenericBody::computeOtherCollisionDetection(
								    cVector3d& a_segmentPointA,
								    cVector3d& a_segmentPointB,
								    cCollisionRecorder& a_recorder,
								    cCollisionSettings& a_settings)
    \param    a_segmentPointA  Start point of segment.
    \param    a_segmentPointB  End point of segment.
    \param    a_recorder  Stores all collision events.
    \param    a_settings  Contains collision settings information.
    \return   Return \b true if a collision has occurred.
*/
//===========================================================================
bool cODEGenericBody::computeOtherCollisionDetection(cVector3d& a_segmentPointA,
													 cVector3d& a_segmentPointB,
													 cCollisionRecorder& a_recorder,
												     cCollisionSettings& a_settings)
{
		bool hit = false;
		if (m_imageModel!=NULL)
		{
			hit = m_imageModel->computeCollisionDetection(a_segmentPointA,
														 a_segmentPointB,
														 a_recorder,
														 a_settings);
		}
		return(hit);
}


//===========================================================================
/*!
    Define a generic object such as a mesh or a shape which is used
    to model the geometry of this current ODE object.

    \fn       void cODEGenericBody::setImageModel(cGenericObject* a_imageModel)
    \param    a_imageModel  CHAI 3D graphical image model.
*/
//===========================================================================
void cODEGenericBody::setImageModel(cGenericObject* a_imageModel)
{
	// check object
	if (a_imageModel == NULL) { return; }

    // store pointer to body
    m_imageModel = a_imageModel;

    // set external parent of body image to current ODE object.
    // rule applies to children which, in the case a mesh, belong
    // to the same object.
    m_imageModel->setExternalParent(this, true);

	// set parent
	m_imageModel->setParent(this);
}


//===========================================================================
/*!
    Uses the bounding box of the geometric representation of the object
    to generate a dynamic box.

    \fn     void cODEGenericBody::createDynamicBoundingBox(bool a_staticObject)
    \param  a_staticObject  If \b true, then object is static had no
            dynamic component is created.
*/
//===========================================================================
void cODEGenericBody::createDynamicBoundingBox(bool a_staticObject)
{
	// check if body image exists
	if (m_imageModel == NULL) { return; }

    // create ode dynamic body if object is non static
    if (!a_staticObject)
    {
        m_ode_body = dBodyCreate(m_ODEWorld->m_ode_world);

	    // store pointer to current object
	    dBodySetData (m_ode_body, this);
    }
    m_static = a_staticObject;

    // computing bounding box of geometry representation
    m_imageModel->computeBoundaryBox(true);

    // get size and center of box
    cVector3d center = m_imageModel->getBoundaryCenter();

    // compute dimensions
    cVector3d size = m_imageModel->getBoundaryMax() -
                     m_imageModel->getBoundaryMin();

    // build box
    m_ode_geom = dCreateBox(m_ODEWorld->m_ode_space, size.x, size.y , size.z);

    // offset box
    dGeomSetPosition (m_ode_geom, center.x, center.y, center.z);

    if (!m_static)
    {
        // set inertia properties
        dMassSetBox(&m_ode_mass, 1.0, size.x, size.y, size.z);
	    dMassAdjust(&m_ode_mass, m_mass);
	    dBodySetMass(m_ode_body,&m_ode_mass);

        // attach body and geometry together
        dGeomSetBody(m_ode_geom, m_ode_body);
    }

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_BOX;

    // store dynamic model parameters
    m_paramDynColModel0 = size.x;
    m_paramDynColModel1 = size.y;
    m_paramDynColModel2 = size.z;
    //m_posOffsetDynColModel;
    //m_rotOffsetDynColModel;
}


//===========================================================================
/*!
    Create an ODE dynamic sphere model. This model is independent from the
    body image which also needs to be defined by the user.
    It is possible to visualize the actual ODE physical model by
    calling setShowDynamicCollisionModel() method.

    \fn     void cODEGenericBody::createDynamicSphere(const double a_radius,
                                            bool a_staticObject,
										    const cVector3d& a_offsetPos,
										    const cMatrix3d& a_offsetRot)
    \param	a_radius   Radius of sphere.
    \param  a_staticObject  If \b true, then object is static had no
            dynamic component is created.
    \param	a_offsetPos  Offset position in respect current object position.
    \param	a_offsetRot  Offset position in respect current object rotation.
*/
//===========================================================================
void cODEGenericBody::createDynamicSphere(const double a_radius,
                                          bool a_staticObject,
										  const cVector3d& a_offsetPos,
										  const cMatrix3d& a_offsetRot)
{
    // create ode dynamic body if object is non static
    if (!a_staticObject)
    {
        m_ode_body = dBodyCreate(m_ODEWorld->m_ode_world);

	    // store pointer to current object
	    dBodySetData (m_ode_body, this);
    }
    m_static = a_staticObject;

    // build sphere
    m_ode_geom = dCreateSphere(m_ODEWorld->m_ode_space, a_radius);

	// adjust position offset
	dGeomSetPosition (m_ode_geom, a_offsetPos.x, a_offsetPos.y, a_offsetPos.z);

	// adjust orientation offset
	dMatrix3 R;
	R[0]  = a_offsetRot.m[0][0];
	R[1]  = a_offsetRot.m[0][1];
	R[2]  = a_offsetRot.m[0][2];
	R[4]  = a_offsetRot.m[1][0];
	R[5]  = a_offsetRot.m[1][1];
	R[6]  = a_offsetRot.m[1][2];
	R[8]  = a_offsetRot.m[2][0];
	R[9]  = a_offsetRot.m[2][1];
	R[10] = a_offsetRot.m[2][2];
	dGeomSetRotation (m_ode_geom, R);

    // set inertia properties
    if (!m_static)
    {
        dMassSetSphere(&m_ode_mass, 1.0, a_radius);
	    dMassAdjust(&m_ode_mass, m_mass);
	    dBodySetMass(m_ode_body,&m_ode_mass);

        // attach body and geometry together
        dGeomSetBody(m_ode_geom, m_ode_body);
    }

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_SPHERE;

    // store dynamic model parameters
    m_paramDynColModel0 = a_radius;
    m_paramDynColModel1 = 0.0;
    m_paramDynColModel2 = 0.0;
    m_posOffsetDynColModel = a_offsetPos;
    m_rotOffsetDynColModel = a_offsetRot;
}


//===========================================================================
/*!
    Create an ODE dynamic box model. This model is independent from the
    body image which also needs to be defined by the user.
    It is possible to visualize the actual ODE physical model by
    calling setShowDynamicCollisionModel() method.

    \fn     void cODEGenericBody::createDynamicBox(const double a_lengthX,
									   const double a_lengthY,
									   const double a_lengthZ,
                                       bool a_staticObject,
									   const cVector3d& a_offsetPos,
									   const cMatrix3d& a_offsetRot)
    \param	a_lengthX  Size of box along x axis.
    \param	a_lengthY  Size of box along y axis.
    \param	a_lengthZ  Size of box along z axis.
    \param  a_staticObject  If \b true, then object is static had no
            dynamic component is created.
    \param	a_offsetPos  Offset position in respect current object position.
    \param	a_offsetRot  Offset position in respect current object rotation.
*/
//===========================================================================
void cODEGenericBody::createDynamicBox(const double a_lengthX,
									   const double a_lengthY,
									   const double a_lengthZ,
                                       bool a_staticObject,
									   const cVector3d& a_offsetPos,
									   const cMatrix3d& a_offsetRot)
{
    // create ode dynamic body if object is non static
    if (!a_staticObject)
    {
        m_ode_body = dBodyCreate(m_ODEWorld->m_ode_world);

	    // store pointer to current object
	    dBodySetData (m_ode_body, this);
    }
    m_static = a_staticObject;

    // build box
    m_ode_geom = dCreateBox(m_ODEWorld->m_ode_space, a_lengthX, a_lengthY, a_lengthZ);

	// adjust position offset
	dGeomSetPosition (m_ode_geom, a_offsetPos.x, a_offsetPos.y, a_offsetPos.z);

	// adjust orientation offset
	dMatrix3 R;
	R[0]  = a_offsetRot.m[0][0];
	R[1]  = a_offsetRot.m[0][1];
	R[2]  = a_offsetRot.m[0][2];
	R[4]  = a_offsetRot.m[1][0];
	R[5]  = a_offsetRot.m[1][1];
	R[6]  = a_offsetRot.m[1][2];
	R[8]  = a_offsetRot.m[2][0];
	R[9]  = a_offsetRot.m[2][1];
	R[10] = a_offsetRot.m[2][2];
	dGeomSetRotation (m_ode_geom, R);

    // set inertia properties
    if (!m_static)
    {
        dMassSetBox(&m_ode_mass, 1.0, a_lengthX, a_lengthY, a_lengthZ);
	    dMassAdjust(&m_ode_mass, m_mass);
	    dBodySetMass(m_ode_body,&m_ode_mass);

        // attach body and geometry together
        dGeomSetBody(m_ode_geom, m_ode_body);
    }

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_BOX;

    // store dynamic model parameters
    m_paramDynColModel0 = a_lengthX;
    m_paramDynColModel1 = a_lengthY;
    m_paramDynColModel2 = a_lengthZ;
    m_posOffsetDynColModel = a_offsetPos;
    m_rotOffsetDynColModel = a_offsetRot;
}


//===========================================================================
/*!
    Create an ODE dynamic cylinder model (capsule).
    This model is independent from the body image which also needs to be
    defined by the user.
    It is possible to visualize the actual ODE physical model by
    calling setShowDynamicCollisionModel() method.

    \fn     void cODEGenericBody::createDynamicCapsule(const double a_radius,
                                           const double a_length,
                                           bool a_staticObject,
										   const cVector3d& a_offsetPos,
										   const cMatrix3d& a_offsetRot)
    \param	a_radius   Radius of capsule.
    \param	a_length   Length of capsule.
    \param  a_staticObject  If \b true, then object is static had no dynamic
            component is created.
    \param	a_offsetPos  Offset position in respect current object position.
    \param	a_offsetRot  Offset position in respect current object rotation.
*/
//===========================================================================
void cODEGenericBody::createDynamicCapsule(const double a_radius,
                                           const double a_length,
                                           bool a_staticObject,
										   const cVector3d& a_offsetPos,
										   const cMatrix3d& a_offsetRot)
{
    // create ode dynamic body if object is non static
    if (!a_staticObject)
    {
        m_ode_body = dBodyCreate(m_ODEWorld->m_ode_world);

	    // store pointer to current object
	    dBodySetData (m_ode_body, this);
    }
    m_static = a_staticObject;

    // build box
    m_ode_geom = dCreateCCylinder(m_ODEWorld->m_ode_space, a_radius, a_length);

	// adjust position offset
	dGeomSetPosition (m_ode_geom, a_offsetPos.x, a_offsetPos.y, a_offsetPos.z);

	// adjust orientation offset
	dMatrix3 R;
	R[0]  = a_offsetRot.m[0][0];
	R[1]  = a_offsetRot.m[0][1];
	R[2]  = a_offsetRot.m[0][2];
	R[4]  = a_offsetRot.m[1][0];
	R[5]  = a_offsetRot.m[1][1];
	R[6]  = a_offsetRot.m[1][2];
	R[8]  = a_offsetRot.m[2][0];
	R[9]  = a_offsetRot.m[2][1];
	R[10] = a_offsetRot.m[2][2];
	dGeomSetRotation (m_ode_geom, R);

    // set inertia properties
    if (!m_static)
    {
        dMassSetCylinder (&m_ode_mass, 1.0, 3, a_radius, a_length);  // 3 = x-axis direction
	    dMassAdjust(&m_ode_mass, m_mass);
	    dBodySetMass(m_ode_body,&m_ode_mass);

        // attach body and geometry together
        dGeomSetBody(m_ode_geom, m_ode_body);
    }

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_CYLINDER;

    // store dynamic model parameters
    m_paramDynColModel0 = a_radius;
    m_paramDynColModel1 = a_length;
    m_paramDynColModel2 = 0.0;
    m_posOffsetDynColModel = a_offsetPos;
    m_rotOffsetDynColModel = a_offsetRot;
}


//===========================================================================
/*!
    Create an ODE static model of a plane.
    This model is independent from the body image which also needs to be
    defined by the user.
    It is possible to visualize the actual ODE physical model by
    calling setShowDynamicCollisionModel() method.

    \fn       void cODEGenericBody::createStaticPlane(const cVector3d& a_position,
								    const cVector3d& a_normal)
    \param	a_position  Position of a point located on the plane.
    \param	a_normal   Surface normal of the plane.
*/
//===========================================================================
void cODEGenericBody::createStaticPlane(const cVector3d& a_position,
										const cVector3d& a_normal)
{
    // object is static by default
    m_static = true;

    // temp variables
    cVector3d normal = a_normal;
    cVector3d offset(0,0,0);

    // check normal
    if (normal.length() == 0) { return; }

    // compute parameters
    normal.normalize();
    double a = normal.x;
    double b = normal.y;
    double c = normal.z;

    offset = cProject(a_position, normal);
    double d = offset.length();
    if (d > 0)
    {
        if (cAngle(offset, normal) > cDegToRad(90))
        {
            d = -d;
        }
    }

    // build fixed plane
    m_ode_geom = dCreatePlane(m_ODEWorld->m_ode_space, a, b, c, d);

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_PLANE;

    // store dynamic model parameters
    m_paramDynColModel0 = normal.x;
    m_paramDynColModel1 = normal.y;
    m_paramDynColModel2 = normal.z;
    m_posOffsetDynColModel = a_position;
    m_rotOffsetDynColModel.identity();
}


//===========================================================================
/*!
    Create an ODE dynamic model of a mesh.
    This model requires the body image to be a mesh and uses it triangles
    to build its physical model.

    \fn     void cODEGenericBody::createDynamicMesh(bool a_staticObject,
                                        const cVector3d& a_offsetPos,
										const cMatrix3d& a_offsetRot)
    \param  a_staticObject  If \b true, then object is static had no
            dynamic component is created.
    \param	a_offsetPos  Offset position in respect current object position.
    \param	a_offsetRot  Offset position in respect current object rotation.
*/
//===========================================================================
void cODEGenericBody::createDynamicMesh(bool a_staticObject,
                                        const cVector3d& a_offsetPos,
										const cMatrix3d& a_offsetRot)
{
    // create ode dynamic body if object is non static
    if (!a_staticObject)
    {
        m_ode_body = dBodyCreate(m_ODEWorld->m_ode_world);

	    // store pointer to current object
	    dBodySetData (m_ode_body, this);
    }
    m_static = a_staticObject;

    // make sure body image has been defined
    if (m_imageModel == NULL) { return; }

    // check if body image is a mesh
    cMesh* mesh = dynamic_cast<cMesh*>(m_imageModel);
    if (mesh == NULL) { return; }

    // store dynamic model type
    m_typeDynamicCollisionModel = ODE_MODEL_TRIMESH;

    // store dynamic model parameters
    m_paramDynColModel0 = 0.0;
    m_paramDynColModel1 = 0.0;
    m_paramDynColModel2 = 0.0;
    m_posOffsetDynColModel = a_offsetPos;
    m_rotOffsetDynColModel = a_offsetRot;

    // create a table which lists all vertices and triangles
    // these vertices must be of type float and not double!
    // even if we are using the double precision compiled version of ODE !
    int numTriangles = mesh->getNumTriangles(true);

    int vertexCount = mesh->getNumVertices(true);
    m_vertices = new float[3 * vertexCount];

    int indexCount = 3 * numTriangles;
    m_vertexIndices = new int[indexCount];

    // build table of ODE vertices and vertex indices recusevily
    int nIndexOffset = 0;
    int nVerticesCount = 0;
    int nIndexCount = 0;
    int nTriangles = buildMeshTable(mesh, nIndexOffset, nVerticesCount, nIndexCount);

    // build box
    m_ode_triMeshDataID = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(m_ode_triMeshDataID,
                                m_vertices,          // vertex positions
                                3 * sizeof(float),   // size of vertex
                                vertexCount,         // number of vertices
                                m_vertexIndices,     // triangle indices
                                3 * nTriangles,      // number of triangles
                                3 * sizeof(int));

    m_ode_geom = dCreateTriMesh(m_ODEWorld->m_ode_space, m_ode_triMeshDataID, 0, 0, 0);

    // remember the mesh's dTriMeshDataID on its userdata for convenience.
    dGeomSetData(m_ode_geom, m_ode_triMeshDataID);

    // computing bounding box of geometry representation
    m_imageModel->computeBoundaryBox(true);

    // compute dimensions
    cVector3d size = m_imageModel->getBoundaryMax() -
                     m_imageModel->getBoundaryMin();

    // set inertia properties
    if (!m_static)
    {
        dMassSetBox(&m_ode_mass, 1.0, size.x, size.y, size.z);
	    dMassAdjust(&m_ode_mass, m_mass);
	    dBodySetMass(m_ode_body,&m_ode_mass);

        dGeomSetBody(m_ode_geom, m_ode_body);
    }

	// adjust position offset
	dGeomSetPosition (m_ode_geom, a_offsetPos.x, a_offsetPos.y, a_offsetPos.z);

	// adjust orientation offset
	dMatrix3 R;
	R[0]  = a_offsetRot.m[0][0];
	R[1]  = a_offsetRot.m[0][1];
	R[2]  = a_offsetRot.m[0][2];
	R[4]  = a_offsetRot.m[1][0];
	R[5]  = a_offsetRot.m[1][1];
	R[6]  = a_offsetRot.m[1][2];
	R[8]  = a_offsetRot.m[2][0];
	R[9]  = a_offsetRot.m[2][1];
	R[10] = a_offsetRot.m[2][2];
	dGeomSetRotation (m_ode_geom, R);
}


//===========================================================================
/*!
    Creates an ODE list of vertices and vertex indices from a mesh
    and its children.

    \fn       int cODEGenericBody::buildMeshTable(cMesh* a_mesh,
                                int& a_indexOffset,
                                int& a_verticesCount,
                                int& a_indexCount)
    \param	a_mesh  current mesh to be added to ODE vertex and triangle list.
    \param	a_indexOffset  index offset as meshes are added in one list.
    \param	a_verticesCount  vertex counter.
    \param	a_indexCount  Index counter.
    \param    return the number of triangle added to ODE list
*/
//===========================================================================
int cODEGenericBody::buildMeshTable(cMesh* a_mesh,
                                    int& a_indexOffset,
                                    int& a_verticesCount,
                                    int& a_indexCount)
{
    // temp variables
    int nTriangles = 0;

    // compute number of vertices and triangles for a_mesh
    int numTriangles = a_mesh->getNumTriangles();
    int numVertices = a_mesh->getNumVertices();

    // store vertices
    for (int i=0; i<numVertices; i++)
    {
        cVertex* nextVertex = a_mesh->getVertex(i);
        cVector3d pos = nextVertex->getPos();
        m_vertices[a_verticesCount] = pos.x;
        a_verticesCount++;
        m_vertices[a_verticesCount] = pos.y;
        a_verticesCount++;
        m_vertices[a_verticesCount] = pos.z;
        a_verticesCount++;
    }

    // store triangles
    for (int i=0; i<numTriangles; i++)
    {
        cTriangle* nextTriangle = a_mesh->getTriangle(i);
        double area = nextTriangle->computeArea();
        if (area > 0)
        {
            int vertex0 = nextTriangle->getIndexVertex0() + a_indexOffset;
            int vertex1 = nextTriangle->getIndexVertex1() + a_indexOffset;
            int vertex2 = nextTriangle->getIndexVertex2() + a_indexOffset;
            m_vertexIndices[a_indexCount] = vertex0; a_indexCount++;
            m_vertexIndices[a_indexCount] = vertex1; a_indexCount++;
            m_vertexIndices[a_indexCount] = vertex2; a_indexCount++;
            nTriangles++;
        }
    }

    // update offset
    a_indexOffset = a_indexOffset + numVertices;

    // process children
    int numChildren = a_mesh->getNumChildren();
    for (int i=0; i<numChildren; i++)
    {
        cGenericObject* nextObject = a_mesh->getChild(i);
        cMesh* nextMesh = dynamic_cast<cMesh*>(nextObject);
        if (nextMesh != NULL)
        {
            nTriangles = nTriangles + buildMeshTable(nextMesh, a_indexOffset, a_verticesCount, a_indexCount);
        }
    }

    return (nTriangles);
}


//===========================================================================
/*!
    Enable object from being updated within the dynamics simulation.

    \fn       void cODEGenericBody::enableDynamics()
*/
//===========================================================================
void cODEGenericBody::enableDynamics()
{
    if (m_ode_body == NULL) { return; }
    dBodyEnable(m_ode_body);
    dGeomEnable(m_ode_geom);
}


//===========================================================================
/*!
    Disable object from being updated within the dynamics simulation.

    \fn       void cODEGenericBody::enableDynamics()
*/
//===========================================================================
void cODEGenericBody::disableDynamics()
{
    if (m_ode_body == NULL) { return; }
    dBodyDisable(m_ode_body);
    dGeomDisable(m_ode_geom);
}


//===========================================================================
/*!
    Render this deformable mesh in OpenGL.

    \fn       void cODEGenericBody::render(const int a_renderMode)
    \param    a_renderMode  Rendering mode (see cGenericObject).
*/
//===========================================================================
void cODEGenericBody::render(const int a_renderMode)
{
    if (m_imageModel != NULL)
    {
        m_imageModel->renderSceneGraph(a_renderMode);
    }
    if (m_showDynamicCollisionModel)
    {
        glDisable(GL_LIGHTING);
        glLineWidth(1.0);
        glColor4fv(m_colorDynamicCollisionModel.pColor());

        cMatrixGL m_frameGL;
        m_frameGL.set(m_posOffsetDynColModel, m_rotOffsetDynColModel);
        m_frameGL.glMatrixPushMultiply();

        if (m_typeDynamicCollisionModel == ODE_MODEL_BOX)
        {
            double hx = m_paramDynColModel0 / 2.0;
            double hy = m_paramDynColModel1 / 2.0;
            double hz = m_paramDynColModel2 / 2.0;
            cDrawWireBox(-hx, hx, -hy, hy, -hz, hz);
        }
        else if (m_typeDynamicCollisionModel == ODE_MODEL_SPHERE)
        {
            // create sphere
            GLUquadricObj *sphere;
            sphere = gluNewQuadric ();

            // set rendering style
            gluQuadricDrawStyle (sphere, GLU_LINE);

            // set normal-rendering mode
            gluQuadricNormals (sphere, GLU_SMOOTH);

            // render a sphere
            gluSphere(sphere, m_paramDynColModel0, 16, 16);

            // delete our quadric object
            gluDeleteQuadric(sphere);
        }
        else if (m_typeDynamicCollisionModel == ODE_MODEL_CYLINDER)
        {
			// transle to center cylinder
			glTranslated(0.0, 0.0, -m_paramDynColModel1 / 2.0);

            // create cylinder
            GLUquadricObj *cylinder;
            cylinder = gluNewQuadric ();

            // set rendering style
            gluQuadricDrawStyle (cylinder, GLU_LINE);

            // set normal-rendering mode
            gluQuadricNormals (cylinder, GLU_SMOOTH);

            // render a sphere
            gluCylinder(cylinder, m_paramDynColModel0, m_paramDynColModel0, m_paramDynColModel1, 16, 16);

            // delete our quadric object
            gluDeleteQuadric(cylinder);
        }

        glEnable(GL_LIGHTING);
        m_frameGL.glMatrixPop();
    }
}


//===========================================================================
/*!
    Compute globalPos and globalRot given the localPos and localRot
    of this object and its parents.  Optionally propagates to children.

    If \e a_frameOnly is set to \b false, additional global positions such as
    vertex positions are computed (which can be time-consuming).

    \fn       void cODEGenericBody::updateGlobalPositions(const bool a_frameOnly)
    \param    a_frameOnly  If \b true then only the global frame is computed
*/
//===========================================================================
void cODEGenericBody::updateGlobalPositions(const bool a_frameOnly)
{
	if (m_imageModel != NULL)
    {
        m_imageModel->computeGlobalPositions(a_frameOnly,
                                            m_globalPos,
                                            m_globalRot);
    }
};


