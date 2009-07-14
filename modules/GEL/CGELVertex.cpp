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
#include "CGELVertex.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cGELVertex.

    \fn       cGELVertex::cGELVertex(cVertex* a_vertex)
*/
//==========================================================================
cGELVertex::cGELVertex(cVertex* a_vertex)
{
    m_vertex = a_vertex;
    m_massParticle = new cGELMassParticle();
    m_link = NULL;
    m_node = NULL;
    m_massParticle->m_pos = a_vertex->m_localPos;
}


//===========================================================================
/*!
    Destructor of cGELVertex.

    \fn       cGELVertex::~cGELVertex()
*/
//==========================================================================
cGELVertex::~cGELVertex()
{

}
