#ifndef __DHDWRIST_H__
#define __DHDWRIST_H__

#include "dhdCom.h"

enum WristParameters {
  DHD_EEPROM_WRIST_DACZEROVALUE = DHD_EEPROM_WRIST_OFFSET,
  DHD_EEPROM_WRIST_MOTORVALMAX,
  DHD_EEPROM_WRIST_MOTORROTATIONSIGN,
  DHD_EEPROM_WRIST_ENCOFFSET0,
  DHD_EEPROM_WRIST_ENCOFFSET1,
  DHD_EEPROM_WRIST_ENCOFFSET2,
  DHD_EEPROM_WRIST_SPRINGINITIALLENGTH0,
  DHD_EEPROM_WRIST_SPRINGINITIALLENGTH1,
  DHD_EEPROM_WRIST_SPRINGINITIALLENGTH2,
  DHD_EEPROM_WRIST_SPRINGINITIALFORCE0,
  DHD_EEPROM_WRIST_SPRINGINITIALFORCE1,
  DHD_EEPROM_WRIST_SPRINGINITIALFORCE2,
  DHD_EEPROM_WRIST_SPRINGK0,
  DHD_EEPROM_WRIST_SPRINGK1,
  DHD_EEPROM_WRIST_SPRINGK2,
  DHD_EEPROM_WRIST_SPRINGDD0,
  DHD_EEPROM_WRIST_SPRINGDD1,
  DHD_EEPROM_WRIST_SPRINGDD2,
  DHD_EEPROM_WRIST_VOLTREFERENCE,
  DHD_EEPROM_WRIST_MOTORVOLTMAX,
  DHD_EEPROM_WRIST_MOTORVOLTMIN,
  DHD_EEPROM_WRIST_MOTORREDUCTION,
  DHD_EEPROM_WRIST_MOTORVOLTPERAMP,
  DHD_EEPROM_WRIST_MOTORNMPERAMP,
  DHD_EEPROM_WRIST_PULLEYRADIUS,
  DHD_EEPROM_WRIST_ENCODERINCREMENTS,
  DHD_EEPROM_WRIST_DACVALUEPERVOLT,
  DHD_EEPROM_WRIST_ANGLE,
  DHD_EEPROM_WRIST_WRISTMASS
};


class dhdWrist 
{
private:
  short  dacZeroValue;
  short  motorValMax;
  short  motorRotationSign;
  short  encOffset0;
  short  encOffset1;
  short  encOffset2;
  double springInitialLength0;
  double springInitialLength1;
  double springInitialLength2;
  double springInitialForce0;
  double springInitialForce1;
  double springInitialForce2;
  double springK0;
  double springK1;
  double springK2;
  double springDd0;
  double springDd1;
  double springDd2;
  double voltReference;
  double motorVoltMax;
  double motorVoltMin;
  double motorReduction;
  double motorVoltPerAmp;
  double motorNmPerAmp;
  double pulleyRadius;
  double encoderIncrements;
  double dacValuePerVolt;
  double angle;
  double wristMass;

  double epsilon;
  double a1x;
  double a1y;
  double a1z;
  double a2x;
  double a2y;
  double a2z;
  double a3x;
  double a3y;
  double a3z;
  double b1x;
  double b1y;
  double b1z;
  double b2x;
  double b2y;
  double b2z;
  double b3x;
  double b3y;
  double b3z;
  double c1x;
  double c1y;
  double c1z;
  double c2x;
  double c2y;
  double c2z;
  double c3x;
  double c3y;
  double c3z;
  double d1x;
  double d1y;
  double d1z;
  double d2x;
  double d2y;
  double d2z;
  double d3x;
  double d3y;
  double d3z;

  double dicab1;
  double dicab2;
  double dicab3;
  double rA1carre;
  double rA2carre;
  double rB1carre;
  double rB2carre;
  double cosAngle;
  double sinAngle;
  double reduction;

  int    Enc0, Enc1, Enc2;
  int    Fenc0, Fenc1, Fenc2;
  int    Mot0, Mot1, Mot2;

  int update ();
  int model (int enc0, int enc1, int enc2);

public:
  double Oa, Ob, Og;
  double Ta, Tb, Tg;

  dhdWrist  ();
  ~dhdWrist ();

  int    readWristConfig               (dhdCom *com);
  int    writeWristConfig              (dhdCom *com);

  int    setWristDacZeroValue          (short  val) { dacZeroValue         = val; return update (); };
  int    setWristMotorValMax           (short  val) { motorValMax          = val; return update (); };
  int    setWristMotorRotationSign     (short  val) { motorRotationSign    = val; return update (); };
  int    setWristEncOffset0            (short  val) { encOffset0           = val; return update (); };
  int    setWristEncOffset1            (short  val) { encOffset1           = val; return update (); };
  int    setWristEncOffset2            (short  val) { encOffset2           = val; return update (); };
  int    setWristSpringInitialLength0  (double val) { springInitialLength0 = val; return update (); };
  int    setWristSpringInitialLength1  (double val) { springInitialLength1 = val; return update (); };
  int    setWristSpringInitialLength2  (double val) { springInitialLength2 = val; return update (); };
  int    setWristSpringInitialForce0   (double val) { springInitialForce0  = val; return update (); };
  int    setWristSpringInitialForce1   (double val) { springInitialForce1  = val; return update (); };
  int    setWristSpringInitialForce2   (double val) { springInitialForce2  = val; return update (); };
  int    setWristSpringK0              (double val) { springK0             = val; return update (); };
  int    setWristSpringK1              (double val) { springK1             = val; return update (); };
  int    setWristSpringK2              (double val) { springK2             = val; return update (); };
  int    setWristSpringDd0             (double val) { springDd0            = val; return update (); };
  int    setWristSpringDd1             (double val) { springDd1            = val; return update (); };
  int    setWristSpringDd2             (double val) { springDd2            = val; return update (); };
  int    setWristVoltReference         (double val) { voltReference        = val; return update (); };
  int    setWristMotorVoltMax          (double val) { motorVoltMax         = val; return update (); };
  int    setWristMotorVoltMin          (double val) { motorVoltMin         = val; return update (); };
  int    setWristMotorReduction        (double val) { motorReduction       = val; return update (); };
  int    setWristMotorVoltPerAmp       (double val) { motorVoltPerAmp      = val; return update (); };
  int    setWristMotorNmPerAmp         (double val) { motorNmPerAmp        = val; return update (); };
  int    setWristPulleyRadius          (double val) { pulleyRadius         = val; return update (); };
  int    setWristEncoderIncrements     (double val) { encoderIncrements    = val; return update (); };
  int    setWristDacValuePerVolt       (double val) { dacValuePerVolt      = val; return update (); };
  int    setWristAngle                 (double val) { angle                = val; return update (); };
  int    setWristMass                  (double val) { wristMass            = val; return update (); };

  short  getWristDacZeroValue          ()           { return dacZeroValue; };
  short  getWristMotorValMax           ()           { return motorValMax; };
  short  getWristMotorRotationSign     ()           { return motorRotationSign; };
  short  getWristEncOffset0            ()           { return encOffset0; };
  short  getWristEncOffset1            ()           { return encOffset1; };
  short  getWristEncOffset2            ()           { return encOffset2; };
  double getWristSpringInitialLength0  ()           { return springInitialLength0; };
  double getWristSpringInitialLength1  ()           { return springInitialLength1; };
  double getWristSpringInitialLength2  ()           { return springInitialLength2; };
  double getWristSpringInitialForce0   ()           { return springInitialForce0; };
  double getWristSpringInitialForce1   ()           { return springInitialForce1; };
  double getWristSpringInitialForce2   ()           { return springInitialForce2; };
  double getWristSpringK0              ()           { return springK0; };
  double getWristSpringK1              ()           { return springK1; };
  double getWristSpringK2              ()           { return springK2; };
  double getWristSpringDd0             ()           { return springDd0; };
  double getWristSpringDd1             ()           { return springDd1; };
  double getWristSpringDd2             ()           { return springDd2; };
  double getWristVoltReference         ()           { return voltReference; };
  double getWristMotorVoltMax          ()           { return motorVoltMax; };
  double getWristMotorVoltMin          ()           { return motorVoltMin; };
  double getWristMotorReduction        ()           { return motorReduction; };
  double getWristMotorVoltPerAmp       ()           { return motorVoltPerAmp; };
  double getWristMotorNmPerAmp         ()           { return motorNmPerAmp; };
  double getWristPulleyRadius          ()           { return pulleyRadius; };
  double getWristEncoderIncrements     ()           { return encoderIncrements; };
  double getWristDacValuePerVolt       ()           { return dacValuePerVolt; };
  double getWristAngle                 ()           { return angle; };
  double getWristMass                  ()           { return wristMass; };

  int   encoderToOrientation      (int  enc0, int  enc1, int  enc2, double *oa, double *ob, double *og);
  int   orientationToEncoder      (double oa, double ob, double og, int  *enc0, int  *enc1, int  *enc2);

  int   motorToTorque             (short mot0, short mot1, short mot2, int enc0, int enc1, int enc2, double  *ta, double  *tb, double *tg);
  int   torqueToMotor             (double  ta, double  tb, double  tg, int enc0, int enc1, int enc2, short *mot0, short *mot1, short  *mot2);
};

#endif

