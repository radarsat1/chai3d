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
#ifndef CGELSkeletonNodeH
#define CGELSkeletonNodeH
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \file       CGELSkeletonNode.h

    \brief 
    <b> GEL Module </b> \n 
    Skeleton Node.
*/
//===========================================================================

//===========================================================================
/*!
    \class      cGELSkeletonNode
    \ingroup    GEL

    \brief      
    cGELSkeletonNode defines a dynamic node within the skeleton.
*/
//===========================================================================
class cGELSkeletonNode
{
  public:

    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

    //! Constructor of cGELSkeletonNode.
    cGELSkeletonNode();

    //! Destructor of cGELSkeletonNode.
    ~cGELSkeletonNode();


	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------

    //! Set the mass of the node.
    void setMass(double a_mass);

    //! Add force to node.
    inline void addForce(cVector3d &a_force)
    {
        m_force.add(a_force);
    }

    //! Add torque to node.
    inline void addTorque(cVector3d &a_torque)
    {
        m_torque.add(a_torque);
    }

    //! Set an external force to node.
    inline void setExternalForce(cVector3d &a_force)
    {
        m_externalForce = a_force;
    }

    //! Set an external torque to node.
    inline void setExternalTorque(cVector3d &a_torque)
    {
        m_externalTorque = a_torque;
    }

    //! Compute next position.
    inline void computeNextPose(double a_timeInterval)
    {
        if (!m_fixed)
        {
            // Euler double integration for position
            cVector3d damping;
            m_vel.mulr(-m_kDampingPos * m_mass, damping);
            m_force.add(damping);
            m_acc = cDiv(m_mass, cAdd(m_force, m_externalForce));
            m_vel = cAdd(m_vel, cMul(a_timeInterval, m_acc));
            m_nextPos = cAdd(m_pos, cMul(a_timeInterval, m_vel));
        }
        else
        {
            m_nextPos = m_pos;
        }

        // Euler double integration for rotation
        cVector3d dampingAng;
        m_angVel.mulr(-m_kDampingRot * m_mass, dampingAng);
        m_torque.add(dampingAng);
        m_angAcc = cMul((1/m_inertia), m_torque);
        m_angVel = cAdd(m_angVel, cMul(a_timeInterval, m_angAcc));

        double normAngVel = m_angVel.length();
        if (normAngVel < 0.000001)
        {
            m_nextRot = m_rot;
        }
        else
        {
            m_nextRot = cRotate(m_rot, m_angVel, a_timeInterval * normAngVel);
        }
    }

    //! Update pose with new computed values.
    inline void applyNextPose()
    {
        m_pos = m_nextPos;
        m_rot = m_nextRot;
    }

    //! Clear forces and torques.
    inline void clearForces()
    {
        if (m_useGravity)
        {
            m_force = m_gravity;
            m_force.mul(m_mass);
        }
        else
        {
            m_force.zero();
        }
        m_torque.zero();
    }

    //! Clear external forces and torques.
    inline void clearExternalForces()
    {
        m_externalForce.zero();
        m_externalTorque.zero();
    }

    //! Render node in OpenGL.
    inline void render()
    {
        // set pose
        cMatrixGL mat;
        mat.set(m_pos, m_rot);
        mat.glMatrixPushMultiply();

        // draw node
        m_color.render();
        cDrawSphere(m_radius, 12, 12);

        // draw frame
        if (default_showFrame == true)
        {
            double frameScale = 3.0 * m_radius;
            cDrawFrame(frameScale);
        }

        // pos open gl matrix
        mat.glMatrixPop();

        // render external forces
        glColor4fv( (const float *)&m_color);
        cVector3d v = cAdd(m_pos, cMul(1.0/50.0, m_externalForce));
        glBegin(GL_LINES);
          glVertex3dv( (const double *)&m_pos);
          glVertex3dv( (const double *)&v);
        glEnd();
    }


	//-----------------------------------------------------------------------
    // MEMBERS - GRAPHICAL PROPERTIES:
    //-----------------------------------------------------------------------

    //! Color used to display nodes.
    cColorf m_color;


	//-----------------------------------------------------------------------
    // MEMBERS - PHYSICAL PROPERTIES:
    //-----------------------------------------------------------------------

    //! Radius of mass node.
    double m_radius;

    //! Mass property.
    double m_mass;

    //! Current force applied on node.
    cVector3d m_force;

    //! Current torque applies on node.
    cVector3d m_torque;

    //! Instant acceleration at node.
    cVector3d m_acc;

    //! Instant angular acceleration at node.
    cVector3d m_angAcc;

    //! Instant velocity at node.
    cVector3d m_vel;

    //! Instant angular velocity at node.
    cVector3d m_angVel;

    //! Linear damping.
    double m_kDampingPos;

    //! Angular damping.
    double m_kDampingRot;

    //! Inertia (to be completed).
    double m_inertia;

    //! If \b true, then mass is fixed in space and can not move.
    bool m_fixed;

    //! f \b true, then gravity is enabled.
    bool m_useGravity;

    //! Gravity field.
    cVector3d m_gravity;


	//-----------------------------------------------------------------------
    // MEMBERS - POSITION & ORIENTATION:
    //-----------------------------------------------------------------------

    //! Position computed.
    cVector3d m_pos;

    //! Rotation computed.
    cMatrix3d m_rot;

    //! Next position computed.
    cVector3d m_nextPos;

    //! Next rotation computed.
    cMatrix3d m_nextRot;


  private:

	//-----------------------------------------------------------------------
    // MEMBERS - EXTERNAL FORCES:
    //-----------------------------------------------------------------------
      
    //! External force.
    cVector3d m_externalForce;

    //! External torque.
    cVector3d m_externalTorque;


  public:

	//-----------------------------------------------------------------------
    // MEMBERS - DEFAULT SETTINGS:
    //-----------------------------------------------------------------------

    //! Default value for node radius.
    static double default_radius;

    //! Default value for linear damping.
    static double default_kDampingPos;

    //! Default value for revolute damping.
    static double default_kDampingRot;

    //! Default value for node mass.
    static double default_mass;

    //! Defualt value - Is gravity field enabled?
    static bool default_useGravity;

    //! Default value - Gravity field magnitude and direction.
    static cVector3d default_gravity;

    //! Default value - Is the node reference frame displayed?
    static bool default_showFrame;

    //! Default color used to render the node.
    static cColorf default_color;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


