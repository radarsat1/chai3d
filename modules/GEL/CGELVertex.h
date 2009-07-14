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
#ifndef CGELVertexH
#define CGELVertexH
//---------------------------------------------------------------------------
#include "CGELMassParticle.h"
#include "CGELSkeletonLink.h"
#include "CGELSkeletonNode.h"
#include "chai3d.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CGELVertex.h

    \brief 
    <b> GEL Module </b> \n 
    Deformable Mesh Vertex.
*/
//===========================================================================

//===========================================================================
/*!
    \class      cGELVertex
    \ingroup    GEL

    \brief      
    cGELVertex implement a cVertex class with a mass particle for 
    simulating deformable objects
*/
//===========================================================================
class cGELVertex
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cGELVertex.
    cGELVertex(cVertex* a_vertex);

    //! Destructor of cGELVertex.
    virtual ~cGELVertex();


	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------

    //! Mesh object vertex.
    cVertex* m_vertex;

    //! Mass particle for current vertex.
    cGELMassParticle* m_massParticle;

    //! Skeleton link to which this vertex may be linked to.
    cGELSkeletonLink* m_link;

    //! Skeleton node to which this vertex may be linked to.
    cGELSkeletonNode* m_node;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
