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
#include "CGELSkeletonLink.h"
#include "CGELMesh.h"
//---------------------------------------------------------------------------

//===========================================================================
// DEFINITION - DEFAULT VALUES:
//===========================================================================

// Physical properties:
double cGELSkeletonLink::default_kSpringElongation = 100.0; // [N/m]
double cGELSkeletonLink::default_kSpringFlexion    = 0.1;   // [Nm/RAD]
double cGELSkeletonLink::default_kSpringTorsion   = 0.1;    // [Nm/RAD]

// Graphical properties:
cColorf cGELSkeletonLink::default_color(0.2, 0.2, 1.0);


//===========================================================================
/*!
    Constructor of cGELSkeletonLink.

    \fn     cGELSkeletonLink::cGELSkeletonLink(cGELSkeletonNode* a_node0,
                                               cGELSkeletonNode* a_node1)
    \param  a_node0  Node 0.
    \param  a_node1  Node 1.
*/
//===========================================================================
cGELSkeletonLink::cGELSkeletonLink(cGELSkeletonNode* a_node0, cGELSkeletonNode* a_node1)
{
    // Set nodes:
    m_node0 = a_node0;
    m_node1 = a_node1;
	m_enabled = true;

    // set position references
    m_wLink01 = cSub(m_node1->m_pos, m_node0->m_pos);
    m_wLink10 = cSub(m_node0->m_pos, m_node1->m_pos);

	if (m_wLink01.length() == 0)
	{
		m_enabled = false;
	}

    m_wzLink01 = m_wLink01;
    m_wzLink10 = m_wLink10;

    m_nzLink01 = cMul(cTrans(m_node0->m_rot), m_wzLink01);
    m_nzLink10 = cMul(cTrans(m_node1->m_rot), m_wzLink10);

    // compute reference frames
    cVector3d v(1.0, 0.0, 0.0);
    double ang = cAngle(v, m_wLink01);
    if ((ang < 0.01) || (ang > 3.13)) { v.set(0.0, 1.0, 0.0); }

    cVector3d A0 = cNormalize( cCross(m_wLink01, v) );
    cVector3d A1 = A0;
    /// \todo (seb 20090514) had to rename B0 to _B0 due to name collision, we should figure out where
    ///                      and probably use a CHAI3D namespace to avoid future collisions for the user
    cVector3d _B0 = cNormalize( cCross(m_wLink01, A0) );

    m_A0 = cMul(cTrans(m_node0->m_rot), A0);
    m_B0 = cMul(cTrans(m_node0->m_rot), _B0);
    m_A1 = cMul(cTrans(m_node1->m_rot), A1);

    // set default color
    m_color = default_color;

    // compute initial length
    m_length0 = cDistance(m_node1->m_pos, m_node0->m_pos);
    m_length = m_length0;

    // set elongation spring constant [N/M]
    m_kSpringElongation = default_kSpringElongation;

    // set flexion angular spring constant [NM/RAD]
    m_kSpringFlexion = default_kSpringFlexion;

    // set torsion angular spring constant [NM/RAD]
    m_kSpringTorsion = default_kSpringTorsion;
}


//===========================================================================
/*!
     Destructor of cGELSkeletonLink.

      \fn       cGELSkeletonLink::~cGELSkeletonLink()
*/
//===========================================================================
cGELSkeletonLink::~cGELSkeletonLink()
{

}

