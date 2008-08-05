/* C header */

#ifndef __DHDC_H__
#define __DHDC_H__


/****************************************************************************
 *  OS DEPENDENCIES
 ****************************************************************************/

/* 
  NOTE:
  make sure to define your correct target platform 
  (or define the appropriate target in your preprocessor definitions list)

#define WIN32
#define LINUX
#define APPLE

*/

// #define WIN32

#ifdef _WIN32
#ifndef __DHDC_EXPORT
#define __API  _declspec(dllimport) __stdcall
#else
#define __API  _declspec(dllexport) __stdcall
#endif
#endif

#ifndef __API
#define __API
#endif

#ifdef __cplusplus
extern "C" {
#endif



/****************************************************************************
 *  ERROR MANAGEMENT
 ****************************************************************************/

  /* error codes */
  enum dhd_errors { 
    DHD_NO_ERROR,
    DHD_ERROR,
    DHD_ERROR_COM,
    DHD_ERROR_DHC_BUSY,
    DHD_ERROR_NO_DRIVER_FOUND,
    DHD_ERROR_NO_DEVICE_FOUND,
    DHD_ERROR_NOT_AVAILABLE,
    DHD_ERROR_TIMEOUT,
    DHD_ERROR_GEOMETRY,
    DHD_ERROR_EXPERT_MODE_DISABLED,
    DHD_ERROR_NOT_IMPLEMENTED,
    DHD_ERROR_OUT_OF_MEMORY,
    DHD_ERROR_DEVICE_NOT_READY,
    DHD_ERROR_FILE_NOT_FOUND,
    DHD_ERROR_CONFIGURATION
  };

  /* types */
  typedef int  *dhdErrorVal;
  typedef char *dhdErrorStr;

  /* dhd_errno thread-safe global */
  dhdErrorVal __API dhdErrnoLocation ();

#define dhdErrno (*dhdErrnoLocation())

  /* error processing */
  dhdErrorStr __API dhdErrorGetLastStr ();
  dhdErrorStr __API dhdErrorGetStr     (int error);


/****************************************************************************
 *  CONSTANTS
 ****************************************************************************/

/* devices */
#define DHD_DEVICE_NONE           0
#define DHD_DEVICE_SIMULATOR      11
#define DHD_DEVICE_3DOF           31
#define DHD_DEVICE_6DOF           61
#define DHD_DEVICE_6DOF_500       62
#define DHD_DEVICE_OMEGA          32
#define DHD_DEVICE_CUSTOM         91

/* status */
#define DHD_ON                    1
#define DHD_OFF                   0

/* device count */
#define DHD_MAX_DEVICE            8

/* encoder count */
#define DHD_MAX_ENC               7

/* motor count */
#define DHD_MAX_MOTOR             6

/* delta motor index */
#define DHD_DELTA_MOTOR_0         0
#define DHD_DELTA_MOTOR_1         1
#define DHD_DELTA_MOTOR_2         2

/* delta encoder index */
#define DHD_DELTA_ENC_0           0
#define DHD_DELTA_ENC_1           1
#define DHD_DELTA_ENC_2           2

/* wrist motor index */
#define DHD_WRIST_MOTOR_0         3
#define DHD_WRIST_MOTOR_1         4
#define DHD_WRIST_MOTOR_2         5

/* wrist encoder index */
#define DHD_WRIST_ENC_0           3
#define DHD_WRIST_ENC_1           4
#define DHD_WRIST_ENC_2           5

/* TimeGuard return value */
#define DHD_TIMEGUARD             1

/* status count */
#define DHD_MAX_STATUS           12

/* status codes */
#define DHD_STATUS_POWER          0
#define DHD_STATUS_CONNECTED      1
#define DHD_STATUS_STARTED        2
#define DHD_STATUS_RESET          3
#define DHD_STATUS_IDLE           4
#define DHD_STATUS_FORCE          5
#define DHD_STATUS_BRAKE          6
#define DHD_STATUS_TORQUE         7
#define DHD_STATUS_WRIST_DETECTED 8
#define DHD_STATUS_ERROR          9
#define DHD_STATUS_GRAVITY        10
#define DHD_STATUS_TIMEGUARD      11

/* buttons count */
#define DHD_MAX_BUTTONS            8


/****************************************************************************
 *  standard API
 ****************************************************************************/

  int  __API dhdGetDeviceCount              ();
  int  __API dhdSetDevice                   (char ID = -1);
  int  __API dhdGetDeviceID                 ();

  int  __API dhdGetSerialNumber             (unsigned short *sn, char ID = -1);

  int  __API dhdOpen                        ();
  int  __API dhdOpenID                      (char ID);
  int  __API dhdClose                       (char ID = -1);
  int  __API dhdStop                        (char ID = -1);

  int  __API dhdGetSystemType               (char ID = -1);
  int  __API dhdGetVersion                  (double *ver, char ID = -1);
  int  __API dhdGetStatus                   (int status[DHD_MAX_STATUS], char ID = -1);
  int  __API dhdGetDeviceAngleRad           (double *angle, char ID = -1);
  int  __API dhdGetDeviceAngleDeg           (double *angle, char ID = -1);
  int  __API dhdGetGripperMass              (double *mass, char ID = -1);

  unsigned 
  long __API dhdGetSystemCounter            ();

  int  __API dhdGetButton                   (int index, char ID = -1);

  int  __API dhdReset                       (char ID = -1);
  int  __API dhdWaitForReset                (int timeout = 0, char ID = -1);

  int  __API dhdSetGravityCompensation      (int val = DHD_ON, char ID = -1);
  int  __API dhdSetBrakes                   (int val = DHD_ON, char ID = -1);
  int  __API dhdSetDeviceAngleRad           (double angle, char ID = -1);
  int  __API dhdSetDeviceAngleDeg           (double angle, char ID = -1);
  int  __API dhdSetGripperMass              (double mass,  char ID = -1);

  int  __API dhdGetPosition                 (double *px, double *py, double *pz, char ID = -1);
  int  __API dhdGetForce                    (double *fx, double *fy, double *fz, char ID = -1);
  int  __API dhdSetForce                    (double  fx, double  fy, double  fz, char ID = -1);

  int  __API dhdGetOrientationRad           (double *oa, double *ob, double *og, char ID = -1);
  int  __API dhdGetOrientationDeg           (double *oa, double *ob, double *og, char ID = -1);
  int  __API dhdGetTorque                   (double *ta, double *tb, double *tg, char ID = -1);
  int  __API dhdSetTorque                   (double  ta, double  tb, double  tg, char ID = -1);	

  int  __API dhdGetPositionAndOrientation   (double *px, double *py, double *pz, double *oa, double *ob, double *og, char ID = -1);
  int  __API dhdGetForceAndTorque           (double *fx, double *fy, double *fz, double *ta, double *tb, double *tg, char ID = -1);
  int  __API dhdSetForceAndTorque           (double  fx, double  fy, double  fz, double  ta, double  tb, double  tg, char ID = -1);


/****************************************************************************
 *  expert API
 ****************************************************************************/

  int  __API dhdEnableExpertMode            ();
  int  __API dhdDisableExpertMode           ();

  int  __API dhdCalibrateWrist              (char ID = -1);

  int  __API dhdSetTimeGuard                (int us,  char ID = -1);
  int  __API dhdSetVelocityThreshold        (unsigned char val, char ID = -1);

  int  __API dhdUpdateEncoders              (char ID = -1);
  int  __API dhdGetDeltaEncoders            (int *enc0, int *enc1, int *enc2, char ID = -1);
  int  __API dhdGetWristEncoders            (int *enc0, int *enc1, int *enc2, char ID = -1);

  int  __API dhdSetMotor                    (int index, short val, char ID = -1);
  int  __API dhdSetDeltaMotor               (int mot0, int mot1, int mot2, char ID = -1);
  int  __API dhdSetWristMotor               (int mot0, int mot1, int mot2, char ID = -1);

  int  __API dhdDeltaEncoderToPosition      (int  enc0, int  enc1, int  enc2, double *px, double *py, double *pz, char ID = -1);
  int  __API dhdDeltaPositionToEncoder      (double px, double py, double pz, int  *enc0, int  *enc1, int  *enc2, char ID = -1);
  int  __API dhdDeltaMotorToForce           (short mot0, short mot1, short mot2, int enc0, int enc1, int enc2, double  *fx, double  *fy, double  *fz, char ID = -1);
  int  __API dhdDeltaForceToMotor           (double  fx, double  fy, double  fz, int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2, char ID = -1);
  int  __API dhdWristEncoderToOrientation   (int  enc0, int  enc1, int  enc2, double *oa, double *ob, double *og, char ID = -1);
  int  __API dhdWristOrientationToEncoder   (double oa, double ob, double og, int  *enc0, int  *enc1, int  *enc2, char ID = -1);
  int  __API dhdWristMotorToTorque          (short mot0, short mot1, short mot2, int enc0, int enc1, int enc2, double  *ta, double  *tb, double  *tg, char ID = -1);
  int  __API dhdWristTorqueToMotor          (double  ta, double  tb, double  tg, int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2, char ID = -1);



#ifdef __cplusplus
}
#endif


#endif
