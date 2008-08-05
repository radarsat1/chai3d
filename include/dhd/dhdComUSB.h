#ifndef __DHDCOMUSB_H__
#define __DHDCOMUSB_H__

#include "dhdCom.h"

#ifdef WIN32
#define USB_HANDLE    HANDLE
#endif
#ifdef LINUX
#define USB_HANDLE    usb_dev_handle*
#endif
#ifdef APPLE
#define USB_HANDLE    usb_dev_handle*
#endif


// communication control
#define   DHD_COMUSB_PIPE_IN       2
#define   DHD_COMUSB_PIPE_OUT      0

class dhdComUSB : public dhdCom {
private:
  static int            deviceCount;
  static int            deviceOpened;
  static USB_HANDLE     deviceTableUSB[DHD_MAX_DEVICE];

  int                   ID;
  ulong                 nBytes;
  uchar                 buffer[1024];
  USB_HANDLE            handle;

  int writeToPipe  (int pipe, uchar *buffer, int size);
  int readFromPipe (int pipe, uchar *buffer, int size);

public:

  dhdComUSB                         ();
  ~dhdComUSB                        ();

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
  virtual int  getAllEncoders       (int enc[DHD_MAX_ENC]);

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

