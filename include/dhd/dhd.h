// C++ header

#ifndef __DHD_H__
#define __DHD_H__


// include C file
#include "dhdc.h"


//
// data types
//

#define ulong   unsigned long
#define uint    unsigned int
#define ushort  unsigned short
#define uchar   unsigned char


//
// internal macros
//

#define DHD_MIN(x,y)         (((x)<(y))?(x):(y))
#define DHD_MAX(x,y)         (((x)<(y))?(y):(x))

#define BYTE_H(x)            (((x) >> 8) & 0x00ff)
#define BYTE_L(x)            ((x) & 0x00ff)
#define BYTE_HL(xh,xl)       ((((xh) << 8) & 0xff00) + (xl))

#define QUAD_HH(x)           (((x) >> 24) & 0x000000ff)
#define QUAD_HL(x)           (((x) >> 16) & 0x000000ff)
#define QUAD_LH(x)           (((x) >>  8) & 0x000000ff)
#define QUAD_LL(x)           (((x)      ) & 0x000000ff)
#define QUAD_(hh,hl,lh,ll)   (((hh << 24) & 0xff000000) + \
  ((hl << 16) & 0x00ff0000) + \
  ((lh <<  8) & 0x0000ff00) + \
  (ll))

#define DHD_INCREASE(p,val)         (p)=((++p)>=val)?0:(p)
#define DHD_DECREASE(p,val)         (p)=((--p)<0)?(val-1):(p)


/////////////////////////////////////////////////////////////////////////////
//  CLASS DECLARATION
//////////////////////////////////////////////////////////////////////////////
class DHD
{
public:
  DHD  ();
  ~DHD ();


  // standard methods

  int    open                      ();
  int    open                      (int index);
  int    close                     ();

  int    getDeviceCount            ();
  int    getSystemType             ();
  int    getVersion                (double *ver);
  int    getStatus                 (int status[DHD_MAX_STATUS]);
  int    getID                     ();
  int    reset                     ();
  int    waitForReset              (int timeout = -1);

  int    setGravityCompensation    (bool val);
  int    setBrakes                 (int val);
  int    setDeviceAngle            (double angle);

  int    getButton                 (int index = 0);

  int    getPosition               (double *px, double *py, double *pz);
  int    getForce                  (double *fx, double *fy, double *fz);
  int    setForce                  (double  fx, double  fy, double  fz);

  int    getOrientationRad         (double *oa, double *ob, double *og);
  int    getOrientationDeg         (double *oa, double *ob, double *og);
  int    getTorque                 (double *ta, double *tb, double *tg);
  int    setTorque                 (double  ta, double  tb, double  tg);

  int    getPositionAndOrientation (double *px, double *py, double *pz, double *oa, double *ob, double *og);
  int    getForceAndTorque         (double *fx, double *fy, double *fz, double *ta, double *tb, double *tg);
  int    setForceAndTorque         (double  fx, double  fy, double  fz, double  ta, double  tb, double  tg);

  ulong  getSystemCounter          ();
  int    setCallback               (int interval, void (*callback)(void));
  int    start                     ();
  int    stop                      ();

  // advanced methods

  int    calibrateWrist            ();

  int    readConfig                ();
  int    writeConfig               ();
  int    writeHeader               ();
  int    readConfigFromFile        (char *filename);
  int    writeConfigToFile         (char *filename);

  int    setTimeGuard              (int us);
  int    setVelocityThreshold      (uchar val);

  int    updateEncoders            ();
  int    getEncoder                (int index);
  int    getDeltaEncoders          (int *enc0, int *enc1, int *enc2);
  int    getWristEncoders          (int *enc0, int *enc1, int *enc2);

  int    setMotor                  (int index, short val);
  int    setDeltaMotor             (short mot0, short mot1, short mot2);
  int    setWristMotor             (short mot0, short mot1, short mot2);

  int    deltaEncoderToPosition    (int enc0, int enc1, int enc2, double *px, double *py, double *pz);
  int    deltaPositionToEncoder    (double px, double py, double pz, int *enc0, int *enc1, int *enc2);
  int    deltaMotorToForce         (short mot0, short mot1, short mot2, int enc0, int enc1, int enc2, double *fx, double *fy, double *fz);
  int    deltaForceToMotor         (double fx, double fy, double fz, int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2);
  int    wristEncoderToOrientation (int enc0, int enc1, int enc2, double *oa, double *ob, double *og);
  int    wristOrientationToEncoder (double oa, double ob, double og, int *enc0, int *enc1, int *enc2);
  int    wristMotorToTorque        (short mot0, short mot1, short mot2, int enc0, int enc1, int enc2, double *ta, double *tb, double *tg);
  int    wristTorqueToMotor        (double ta, double tb, double tg, int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2);

  // memory management

  int    setEMemory                (ushort addr, uchar *data, uchar length);
  int    getEMemory                (ushort addr, uchar *data, uchar length);
  int    writeFirmware             (char *filename, int *progress);
  int    readFirmware              (char *filename, int *progress);

  // control
  int    initGoto                  (int enc0, int enc1, int enc2);
  int    updateGoto                (int enc0, int enc1, int enc2);
  int    computeGoto               (int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2);
  int    gotoPosition              (double x, double y, double z);
  int    gotoEncoders              (int enc0, int enc1, int enc2);
  int    getDeltaMotor             (short *mot0, short *mot1, short *mot2);

  // retrieve device configuration

  int    getMemoryModel                ();

  ushort getMemVersion                 ();
  uchar  getDhcVersion                 ();
  uchar  getDhdVersion                 ();
  uchar  getDhwVersion                 ();
  ushort getSerialNumber               ();

  short  getDeltaMotorValMax           ();
  short  getDeltaMotorZeroVal          ();
  short  getDeltaMotorRotationSign     ();
  double getDeltaMotorVoltMax          ();
  double getDeltaMotorReduction        ();
  double getDeltaEncoderIncrements     ();
  double getDeltaArmCoupleGravity      ();
  double getDeltaArmBCoupleGravity     ();
  double getDeltaDacValuePerVolt       ();
  double getDeltaMotorNmPerAmp         ();
  double getDeltaMotorVoltPerAmp       ();
  double getDeltaRadiusRac             ();
  double getDeltaAngleOffset           ();
  double getDeltaOriginDistance        ();
  double getDeltaWorkspaceXSize        ();
  double getDeltaWorkspaceYSize        ();
  double getDeltaWorkspaceZSize        ();
  double getDeltaLengthA               ();
  double getDeltaLengthB               ();
  double getDeltaTheta0                ();
  double getDeltaTheta1                ();
  double getDeltaTheta2                ();
  double getDeltaBaseAngle             ();
  double getDeltaGripperMass           ();
  double getDeltaArmBMass0             ();
  double getDeltaArmBMass1             ();
  double getDeltaArmBMass2             ();
  double getDeltaArmAngGravity         ();
  double getDeltaDACSlope0             ();
  double getDeltaDACSlope1             ();
  double getDeltaDACSlope2             ();
  double getDeltaDACOffset0            ();
  double getDeltaDACOffset1            ();
  double getDeltaDACOffset2            ();

  short  getWristDacZeroValue          ();
  short  getWristMotorValMax           ();
  short  getWristMotorRotationSign     ();
  short  getWristEncOffset0            ();
  short  getWristEncOffset1            ();
  short  getWristEncOffset2            ();
  double getWristSpringInitialLength0  ();
  double getWristSpringInitialLength1  ();
  double getWristSpringInitialLength2  ();
  double getWristSpringInitialForce0   ();
  double getWristSpringInitialForce1   ();
  double getWristSpringInitialForce2   ();
  double getWristSpringK0              ();
  double getWristSpringK1              ();
  double getWristSpringK2              ();
  double getWristSpringDd0             ();
  double getWristSpringDd1             ();
  double getWristSpringDd2             ();
  double getWristVoltReference         ();
  double getWristMotorVoltMax          ();
  double getWristMotorVoltMin          ();
  double getWristMotorReduction        ();
  double getWristMotorVoltPerAmp       ();
  double getWristMotorNmPerAmp         ();
  double getWristPulleyRadius          ();
  double getWristEncoderIncrements     ();
  double getWristDacValuePerVolt       ();
  double getWristAngle                 ();
  double getWristMass                  ();

  // define device configuration

  int    setMemoryModel                (int memoryModel);

  int    setMemVersion                 (ushort val);
  int    setDhcVersion                 (uchar  val);
  int    setDhdVersion                 (uchar  val);
  int    setDhwVersion                 (uchar  val);
  int    setSerialNumber               (ushort val);

  int    setDeltaMotorValMax           (short  val);
  int    setDeltaMotorZeroVal          (short  val);
  int    setDeltaMotorRotationSign     (short  val);
  int    setDeltaMotorVoltMax          (double val);
  int    setDeltaMotorReduction        (double val);
  int    setDeltaEncoderIncrements     (double val);
  int    setDeltaArmCoupleGravity      (double val);
  int    setDeltaArmBCoupleGravity     (double val);
  int    setDeltaDacValuePerVolt       (double val);
  int    setDeltaMotorNmPerAmp         (double val);
  int    setDeltaMotorVoltPerAmp       (double val);
  int    setDeltaRadiusRac             (double val);
  int    setDeltaAngleOffset           (double val);
  int    setDeltaOriginDistance        (double val);
  int    setDeltaWorkspaceXSize        (double val);
  int    setDeltaWorkspaceYSize        (double val);
  int    setDeltaWorkspaceZSize        (double val);
  int    setDeltaLengthA               (double val);
  int    setDeltaLengthB               (double val);
  int    setDeltaTheta0                (double val);
  int    setDeltaTheta1                (double val);
  int    setDeltaTheta2                (double val);
  int    setDeltaBaseAngle             (double val);
  int    setDeltaGripperMass           (double val);
  int    setDeltaArmBMass0             (double val);
  int    setDeltaArmBMass1             (double val);
  int    setDeltaArmBMass2             (double val);
  int    setDeltaArmAngGravity         (double val);
  int    setDeltaDACSlope0             (double val);
  int    setDeltaDACSlope1             (double val);
  int    setDeltaDACSlope2             (double val);
  int    setDeltaDACOffset0            (double val);
  int    setDeltaDACOffset1            (double val);
  int    setDeltaDACOffset2            (double val);

  int    setWristDacZeroValue          (short  val);
  int    setWristMotorValMax           (short  val);
  int    setWristMotorRotationSign     (short  val);
  int    setWristEncOffset0            (short  val);
  int    setWristEncOffset1            (short  val);
  int    setWristEncOffset2            (short  val);
  int    setWristSpringInitialLength0  (double val);
  int    setWristSpringInitialLength1  (double val);
  int    setWristSpringInitialLength2  (double val);
  int    setWristSpringInitialForce0   (double val);
  int    setWristSpringInitialForce1   (double val);
  int    setWristSpringInitialForce2   (double val);
  int    setWristSpringK0              (double val);
  int    setWristSpringK1              (double val);
  int    setWristSpringK2              (double val);
  int    setWristSpringDd0             (double val);
  int    setWristSpringDd1             (double val);
  int    setWristSpringDd2             (double val);
  int    setWristVoltReference         (double val);
  int    setWristMotorVoltMax          (double val);
  int    setWristMotorVoltMin          (double val);
  int    setWristMotorReduction        (double val);
  int    setWristMotorVoltPerAmp       (double val);
  int    setWristMotorNmPerAmp         (double val);
  int    setWristPulleyRadius          (double val);
  int    setWristEncoderIncrements     (double val);
  int    setWristDacValuePerVolt       (double val);
  int    setWristAngle                 (double val);
  int    setWristMass                  (double val);


private:
  class iDHD;
  iDHD  *idhd;
};


#endif

