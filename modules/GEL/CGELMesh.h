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
#ifndef CGELMeshH
#define CGELMeshH
//---------------------------------------------------------------------------
#include "CGELSkeletonNode.h"
#include "CGELSkeletonLink.h"
#include "CGELLinearSpring.h"
#include "CGELVertex.h"
#include "chai3d.h"
#include <typeinfo>
#include <vector>
#include <list>
//---------------------------------------------------------------------------
using std::vector;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CGELMesh.h

    \brief 
    <b> GEL Module </b> \n 
    Deformable Mesh.
*/
//===========================================================================

//===========================================================================
/*!
    \class      cGELMesh
    \ingroup    GEL

    \brief      
    cGELMesh inherits from cMesh and integrate a skeleton model for 
    deformation simulation.
*/
//===========================================================================
class cGELMesh : public cMesh
{

  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cMesh.
    cGELMesh(cWorld* a_world):cMesh(a_world){ initialise(); };

    //! Destructor of cMesh.
    virtual ~cGELMesh() {};


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Build dynamic vertices for deformable mesh.
    void buildVertices();

    //! Connect each vertex to skeleton.
    void connectVerticesToSkeleton(bool a_connectToNodesOnly);

    //! Update position of vertices connected to skeleton.
    void updateVertexPosition();

    //! Clear forces.
    void clearForces();

    //! Clear external forces.
    void clearExternalForces();

    //! Compute forces.
    void computeForces();

    //! Compute next pose.
    void computeNextPose(double iTimeInterval);

    //! Apply new computed pose.
    void applyNextPose();

    //! Render deformable mesh.
    virtual void render(const int a_renderMode=CHAI_RENDER_MODE_RENDER_ALL);


	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! List of nodes composing the skeleton.
    list<cGELSkeletonNode*> m_nodes;

    //! List of links connecting the different nodes.
    list<cGELSkeletonLink*> m_links;

    //! List of linear springs connecting vertices together.
    list<cGELLinearSpring*> m_linearSprings;

    //! List of deformable vertices.
    vector<cGELVertex> m_gelVertices;

    //! If \b true then display skeleton.
    bool m_showSkeletonModel;

    //! If \b true then display mass particle model.
    bool m_showMassParticleModel;

    //! Use skeleton model.
    bool m_useSkeletonModel;

    //! Use vertex mass particle model.
    bool m_useMassParticleModel;


  private:

	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Initialize deformable mesh.
    void initialise();
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
