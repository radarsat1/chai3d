#ifndef __DHDCOMPCI_H__
#define __DHDCOMPCI_H__

#include "dhdCom.h"


// communication control
#define   DHD_COMPCI_TIMEOUT          100         // [ms]
#define   DHD_COMPCI_MSG_TIMEOUT      300         // [ms]
#define   DHD_COMPCI_BUSY             0x00AA


class dhdComPCI : public dhdCom {
private:
  static int    deviceCount;
  static int    deviceOpened;
  static ulong  deviceTablePCI[DHD_MAX_DEVICE];

private:
  int   ID;
  ulong address;
  int   lastDataAckInState;

  void  writePortUchar (ulong address, uchar byte);
  uchar readPortUchar  (ulong address);

  int   comInit               ();
  int   comSetMessage         (uchar iMessage);
  int   comSetMessage         (uchar iMessage, uchar iData);
  int   comWaitAckMessageIn   ();
  int   comWaitClearMessageIn ();
  int   comWaitAckDataIn      ();
  uchar comGetData            ();
  int   comGetData            (uchar *iData0, uchar *iData1);
  int   setByteData           (uchar iByte);
  uchar getByteData           ();
  uchar getByteData0          ();
  uchar getByteData1          ();
  int   swapAckDataOut        ();
  int   setAckMessageOut      ();
  int   clearAckMessageOut    ();
  int   setAckDataOut         ();
  int   clearAckDataOut       ();
  int   setByteData0          (uchar iByte);
  int   setByteData1          (uchar iByte);
  int   comSetData            (uchar iData);
  int   comSetData            (uchar iData0, uchar iData1);
  int   setPortDirectionOut   ();
  int   setPortDirectionIn    ();
  int   getAckDataIn          ();
  int   getAckMessageIn       ();

public:

  dhdComPCI                         ();
  ~dhdComPCI                        ();

  int          getDeviceCount       ();
  int          probe                ();

  virtual int  open                 ();
  virtual int  open                 (int index);
  virtual int  getID                () { return ID; };
  virtual bool ready                ();
  virtual int  close                ();
  virtual int  reset                ();
  virtual int  calibrateWrist       ();

  virtual int  getStatus            (uchar *status);
  virtual int  getVersion           (double *ver);
  virtual int  getSystemType        ();
  virtual int  getButtons           (uchar *buttons);
  virtual int  getDeltaEncoders     (int *enc0, int *enc1, int *enc2);
  virtual int  getWristEncoders     (int *enc0, int *enc1, int *enc2);

  virtual int  setMotor             (int index, short val);
  virtual int  setDeltaMotor        (short mot0, short mot1, short mot2);
  virtual int  setWristMotor        (short mot0, short mot1, short mot2);

  virtual int  setBrakesOn          ();
  virtual int  setBrakesOff         ();

  virtual int  setVelocityThreshold (uchar val);

  virtual int  setEMemory           (ushort addr, uchar *data, uchar length);
  virtual int  getEMemory           (ushort addr, uchar *data, uchar length);
  virtual int  setFirmware          (ushort addr, uchar *data, uchar length);
  virtual int  getFirmware          (ushort addr, uchar *data, uchar length);
};


#endif

