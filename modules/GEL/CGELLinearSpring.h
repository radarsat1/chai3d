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
#ifndef CGELLinearSpringH
#define CGELLinearSpringH
//---------------------------------------------------------------------------
#include "chai3d.h"
#include "CGELMassParticle.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CGELLinearSpring.h

    \brief 
    <b> GEL Module </b> \n 
    Simple Linear Spring Model.
*/
//===========================================================================

//===========================================================================
/*!
    \class      cGELLinearSpring
    \ingroup    GEL

    \brief      
    cGELLinearSpring models a linear spring between two mass particles.
*/
//===========================================================================
class cGELLinearSpring
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cGELLinearSpring.
    cGELLinearSpring(cGELMassParticle* a_node0, cGELMassParticle* a_node1);

    //! Destructor of cGELLinearSpring.
    ~cGELLinearSpring();


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    /*!
         Render link in OpenGL.
    */
    //-----------------------------------------------------------------------
    inline void render()
    {
        // render link
        glColor4fv( (const float *)&m_color);
        glBegin(GL_LINES);
          glVertex3dv( (const double *)&m_node0->m_pos);
          glVertex3dv( (const double *)&m_node1->m_pos);
        glEnd();
    }


    //-----------------------------------------------------------------------
    /*!
         Compute forces applied on mass nodes
    */
    //-----------------------------------------------------------------------
    inline void computeForces()
    {
        // update basic parameters of current link
        cVector3d m_link01 = cSub(m_node1->m_pos, m_node0->m_pos);
        double m_length = m_link01.length();

        //-------------------------------------------------------------
        // ELONGATION:
        //-------------------------------------------------------------
        // if distance too small, no forces are applied
        if (m_length < 0.000001) { return; }

        // elongation compute force
        double f = m_kSpringElongation * (m_length - m_length0);

        // apply force
        if (m_length > 0.000001)
        {
            cVector3d force = cMul(f/m_length, m_link01);
            m_node0->addForce(force);
            cVector3d tmpfrc = cMul(-1, force);
            m_node1->addForce(tmpfrc);
        }
    }


	//-----------------------------------------------------------------------
    // MEMBERS - GRAPHICAL PROPERTIES:
    //-----------------------------------------------------------------------

    //! Color property used for displaying link.
    cColorf m_color;


	//-----------------------------------------------------------------------
    // MEMBERS - MASS NODES:
    //-----------------------------------------------------------------------
    
    //! Node 0 of current link.
    cGELMassParticle *m_node0;

    //! Node 1 of current link.
    cGELMassParticle *m_node1;


	//-----------------------------------------------------------------------
    // MEMBERS - PHYSICAL PROPERTIES:
    //-----------------------------------------------------------------------

    //! Linear spring constant.
    double m_kSpringElongation;

	//! Is link enabled?
	bool m_enabled;

    //! Linear damping.
    double m_kDamperElongation;

    //! Initial length of link.
    double m_length0;


  public:

	//-----------------------------------------------------------------------
    // MEMBERS - DEFAULT SETTINGS:
    //-----------------------------------------------------------------------

    //! Default property - linear stiffness.
    static double default_kSpringElongation;   // [N/m]

    //! Default property - color property.
    static cColorf default_color;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

