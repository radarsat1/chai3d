#ifndef __DHDMSC_H__
#define __DHDMSC_H__


/****************************************************************************
 *  backward compatibility (deprecated)
 ****************************************************************************/
int   __API dhdGetPosition              (float &px, float &py, float &pz);
int   __API dhdGetOrientation           (float &oa, float &ob, float &og);
int   __API dhdSetForce                 (float  fx, float  fy, float  fz);
int   __API dhdGetForce                 (float &fx, float &fy, float &fz);
int   __API dhdSetTorque                (float  ta, float  tb, float  tg);
int   __API dhdGetTorque                (float &ta, float &tb, float &tg);
long  __API dhdGetTimer                 ();
int   __API dhdSetDeviceAngle           (float angle);
int   __API dhdSetGripperMass           (float mass);
int   __API dhdGetButtons               (int buttons[DHD_MAX_BUTTONS]);

#endif
