#ifndef __DHDDELTA_H__
#define __DHDDELTA_H__


#include "dhdCom.h"

// control velocity estimate
#define DHD_DELTA_CONTROL_MEMORIZATION      20
#define DHD_DELTA_CONTROL_DT             0.008


enum DeltaParameters {
  DHD_EEPROM_DELTA_MOTORVALMAX = DHD_EEPROM_DELTA_OFFSET,
  DHD_EEPROM_DELTA_MOTORZEROVAL,
  DHD_EEPROM_DELTA_MOTORROTATIONSIGN,
  DHD_EEPROM_DELTA_MOTORVOLTMAX,
  DHD_EEPROM_DELTA_MOTORREDUCTION,
  DHD_EEPROM_DELTA_ENCODERINCREMENTS,
  DHD_EEPROM_DELTA_ARMCOUPLEGRAVITY,
  DHD_EEPROM_DELTA_DACVALUEPERVOLT,
  DHD_EEPROM_DELTA_MOTORNMPERAMP,
  DHD_EEPROM_DELTA_MOTORVOLTPERAMP,
  DHD_EEPROM_DELTA_RADIUSRAC,
  DHD_EEPROM_DELTA_ANGLEOFFSET,
  DHD_EEPROM_DELTA_ORIGINDISTANCE,
  DHD_EEPROM_DELTA_WORKSPACEXSIZE,
  DHD_EEPROM_DELTA_WORKSPACEYSIZE,
  DHD_EEPROM_DELTA_WORKSPACEZSIZE,
  DHD_EEPROM_DELTA_LENGTHA,
  DHD_EEPROM_DELTA_LENGTHB,
  DHD_EEPROM_DELTA_THETA0,
  DHD_EEPROM_DELTA_THETA1,
  DHD_EEPROM_DELTA_THETA2,
  DHD_EEPROM_DELTA_BASEANGLE,
  DHD_EEPROM_DELTA_GRIPPERMASS,
  DHD_EEPROM_DELTA_ARMBMASS0,
  DHD_EEPROM_DELTA_ARMBMASS1,
  DHD_EEPROM_DELTA_ARMBMASS2,
  DHD_EEPROM_DELTA_ARMANGGRAVITY,
  DHD_EEPROM_DELTA_DACSLOPE0,
  DHD_EEPROM_DELTA_DACSLOPE1,
  DHD_EEPROM_DELTA_DACSLOPE2,
  DHD_EEPROM_DELTA_DACOFFSET0,
  DHD_EEPROM_DELTA_DACOFFSET1,
  DHD_EEPROM_DELTA_DACOFFSET2
};



class dhdDelta 
{
private:
  short  motorValMax;
  short  motorZeroVal;
  short  motorRotationSign;

  double motorVoltMax;
  double motorReduction;
  double encoderIncrements;
  double armCoupleGravity;
  double dacValuePerVolt;
  double motorNmPerAmp;
  double motorVoltPerAmp;
  double radiusRac;
  double angleOffset;
  double originDistance;
  double workspaceXSize;
  double workspaceYSize;
  double workspaceZSize;
  double lengthA;
  double lengthB;
  double theta0;
  double theta1;
  double theta2;
  double baseAngle;
  double gripperMass;
  double armBMass0;
  double armBMass1;
  double armBMass2;
  double armAngGravity;
  double DACSlope0;
  double DACSlope1;
  double DACSlope2;
  double DACOffset0;
  double DACOffset1;
  double DACOffset2;

  double reduction;
  double forceReduction;
  double armBCoupleGravity0;
  double armBCoupleGravity1;
  double armBCoupleGravity2;
  double cosTheta0;
  double cosTheta1;
  double cosTheta2;
  double sinTheta0;
  double sinTheta1;
  double sinTheta2;
  double tanTheta0;
  double tanTheta1;  
  double tanTheta2;
  double cosAngle;
  double sinAngle;

  bool   gravity;
  int    Enc0, Enc1, Enc2;
  int    Fenc0, Fenc1, Fenc2;
  int    Mot0, Mot1, Mot2;
  double Rx, Ry, Rz;

  double W0;
  double Ksi0;
  double Kint;
  double MaxInt;
  double LimInt;
  double M;
  double Ka;
  double EncInc;
  double Kre1;
  double Kre2;
  double Int0;
  double Int1;
  double Int2;
  double LastTime[DHD_DELTA_CONTROL_MEMORIZATION];
  int    Tenc0;
  int    Tenc1;
  int    Tenc2;
  short  Gmot0;
  short  Gmot1;
  short  Gmot2;
  int    LastEnc0[DHD_DELTA_CONTROL_MEMORIZATION];
  int    LastEnc1[DHD_DELTA_CONTROL_MEMORIZATION];
  int    LastEnc2[DHD_DELTA_CONTROL_MEMORIZATION];
  int    ControlCounter;
  int    ControlDone;
  int    Epsilon;

  int update ();
  int model  (int enc0, int enc1, int enc2);

public:
  double Px, Py, Pz;
  double Fx, Fy, Fz;

  dhdDelta  ();
  ~dhdDelta ();

  int    setConfig                   (int systemType);

  int    readDeltaConfig             (dhdCom *com);
  int    writeDeltaConfig            (dhdCom *com);

  int    setDeltaMotorValMax         (short  val) { motorValMax        = val; return update (); };
  int    setDeltaMotorZeroVal        (short  val) { motorZeroVal       = val; return update (); };
  int    setDeltaMotorRotationSign   (short  val) { motorRotationSign  = val; return update (); };
  int    setDeltaMotorVoltMax        (double val) { motorVoltMax       = val; return update (); };
  int    setDeltaMotorReduction      (double val) { motorReduction     = val; return update (); };
  int    setDeltaEncoderIncrements   (double val) { encoderIncrements  = val; return update (); };
  int    setDeltaArmCoupleGravity    (double val) { armCoupleGravity   = val; return update (); };
  int    setDeltaDacValuePerVolt     (double val) { dacValuePerVolt    = val; return update (); };
  int    setDeltaMotorNmPerAmp       (double val) { motorNmPerAmp      = val; return update (); };
  int    setDeltaMotorVoltPerAmp     (double val) { motorVoltPerAmp    = val; return update (); };
  int    setDeltaRadiusRac           (double val) { radiusRac          = val; return update (); };
  int    setDeltaAngleOffset         (double val) { angleOffset        = val; return update (); };
  int    setDeltaOriginDistance      (double val) { originDistance     = val; return update (); };
  int    setDeltaWorkspaceXSize      (double val) { workspaceXSize     = val; return update (); };
  int    setDeltaWorkspaceYSize      (double val) { workspaceYSize     = val; return update (); };
  int    setDeltaWorkspaceZSize      (double val) { workspaceZSize     = val; return update (); };
  int    setDeltaLengthA             (double val) { lengthA            = val; return update (); };
  int    setDeltaLengthB             (double val) { lengthB            = val; return update (); };
  int    setDeltaTheta0              (double val) { theta0             = val; return update (); };
  int    setDeltaTheta1              (double val) { theta1             = val; return update (); };
  int    setDeltaTheta2              (double val) { theta2             = val; return update (); };
  int    setDeltaBaseAngle           (double val) { baseAngle          = val; return update (); };
  int    setDeltaGripperMass         (double val) { gripperMass        = val; return update (); };
  int    setDeltaArmBMass0           (double val) { armBMass0          = val; return update (); };
  int    setDeltaArmBMass1           (double val) { armBMass1          = val; return update (); };
  int    setDeltaArmBMass2           (double val) { armBMass2          = val; return update (); };
  int    setDeltaArmAngGravity       (double val) { armAngGravity      = val; return update (); };
  int    setDeltaDACSlope0           (double val) { DACSlope0          = val; return update (); };
  int    setDeltaDACSlope1           (double val) { DACSlope1          = val; return update (); };
  int    setDeltaDACSlope2           (double val) { DACSlope2          = val; return update (); };
  int    setDeltaDACOffset0          (double val) { DACOffset0         = val; return update (); };
  int    setDeltaDACOffset1          (double val) { DACOffset1         = val; return update (); };
  int    setDeltaDACOffset2          (double val) { DACOffset2         = val; return update (); };

  int    setGravityCompensation      (bool val)   { gravity            = val; return update (); };

  short  getDeltaMotorValMax         ()           { return motorValMax; };
  short  getDeltaMotorZeroVal        ()           { return motorZeroVal; };
  short  getDeltaMotorRotationSign   ()           { return motorRotationSign; };
  double getDeltaMotorVoltMax        ()           { return motorVoltMax; };
  double getDeltaMotorReduction      ()           { return motorReduction; };
  double getDeltaEncoderIncrements   ()           { return encoderIncrements; };
  double getDeltaArmCoupleGravity    ()           { return armCoupleGravity; };
  double getDeltaDacValuePerVolt     ()           { return dacValuePerVolt; };
  double getDeltaMotorNmPerAmp       ()           { return motorNmPerAmp; };
  double getDeltaMotorVoltPerAmp     ()           { return motorVoltPerAmp; };
  double getDeltaRadiusRac           ()           { return radiusRac; };
  double getDeltaAngleOffset         ()           { return angleOffset; };
  double getDeltaOriginDistance      ()           { return originDistance; };
  double getDeltaWorkspaceXSize      ()           { return workspaceXSize; };
  double getDeltaWorkspaceYSize      ()           { return workspaceYSize; };
  double getDeltaWorkspaceZSize      ()           { return workspaceZSize; };
  double getDeltaLengthA             ()           { return lengthA; };
  double getDeltaLengthB             ()           { return lengthB; };
  double getDeltaTheta0              ()           { return theta0; };
  double getDeltaTheta1              ()           { return theta1; };
  double getDeltaTheta2              ()           { return theta2; };
  double getDeltaBaseAngle           ()           { return baseAngle; };
  double getDeltaGripperMass         ()           { return gripperMass; };
  double getDeltaArmBMass0           ()           { return armBMass0; };
  double getDeltaArmBMass1           ()           { return armBMass1; };
  double getDeltaArmBMass2           ()           { return armBMass2; };
  double getDeltaArmAngGravity       ()           { return armAngGravity; };
  double getDeltaDACSlope0           ()           { return DACSlope0; };
  double getDeltaDACSlope1           ()           { return DACSlope1; };
  double getDeltaDACSlope2           ()           { return DACSlope2; };
  double getDeltaDACOffset0          ()           { return DACOffset0; };
  double getDeltaDACOffset1          ()           { return DACOffset1; };
  double getDeltaDACOffset2          ()           { return DACOffset2; };

  bool   getGravityCompensation      ()           { return gravity;     };

  int    encoderToPosition           (int  enc0, int  enc1, int  enc2, double *px, double *py, double *pz);
  int    positionToEncoder           (double px, double py, double pz, int  *enc0, int  *enc1, int *enc2);
  int    relativeToEncoder           (double rx, double ry, double rz, int  *enc0, int  *enc1, int *enc2);

  int    motorToForce                (short mot0, short  mot1, short mot2, int enc0, int enc1, int enc2, double  *fx, double  *fy, double  *fz);
  int    forceToMotor                (double  fx, double   fy, double  fz, int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2);

  int    initGoto                    (int enc0, int enc1, int enc2);
  int    updateGoto                  (int enc0, int enc1, int enc2);
  int    computeGoto                 (int enc0, int enc1, int enc2, short *mot0, short *mot1, short *mot2);
};

#endif


