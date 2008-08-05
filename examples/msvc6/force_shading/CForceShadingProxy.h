//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2004 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author:    <http://www.chai3d.org>
    \author:    Chris Sewell
    \version    1.0
    \date       05/2006
*/
//===========================================================================

#include "CProxyPointForceAlgo.h"


//===========================================================================
/*!
      \class      cForceShadingProxy
      \brief      cForceShadingProxy extends cProxyPointForceAlgo for 
                  implementing a somewhat simplified version of force
                  shading, as described in Diego Ruspini's paper "The
                  Haptic Display of Complex Graphical Environments"
*/
//===========================================================================
class cForceShadingProxy : public cProxyPointForceAlgo
{
  public:
    // CONSTRUCTOR AND DESTRUCTOR:
    //! Constructor of cForceShadingProxy
    cForceShadingProxy() : cProxyPointForceAlgo() { m_useShading = 1; };

    // METHODS:
    //! Compute next proxy position using Ruspini's two-pass algorithm
    virtual void computeNextBestProxyPosition(cVector3d a_goal);
    //! Set whether to use force shading
    void setUseShading(int a_useShading) { m_useShading = a_useShading; }
    //! Get whether force shading is being used
    int getUseShading() { return m_useShading; }

  protected:
    // PROPERTIES:
    //! Use force shading?
    int m_useShading;
};