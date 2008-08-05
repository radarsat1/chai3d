#ifndef __DHDCOM_H__
#define __DHDCOM_H__


#include "dhd.h"
#include "dhdOSDep.h"
#include "dhdTimeGuard.h"

enum MemoryModel { NO_MEMORY, UNKNOWN_MEMORY, HC11, C8051 };

// command messages  
#define DHD_CMD_RESET               10
#define DHD_CMD_CALIBRATE_WRIST     11
#define DHD_CMD_SET_EMEMORY         15
#define DHD_CMD_GET_EMEMORY         16
#define DHD_CMD_SET_EMEMORY_PCI16   17
#define DHD_CMD_GET_EMEMORY_PCI16   18
#define DHD_CMD_SET_BRAKES_ON       20
#define DHD_CMD_SET_BRAKES_OFF      21
#define DHD_CMD_SET_DELTA_MOTORS    30
#define DHD_CMD_SET_WRIST_MOTORS    31
#define DHD_CMD_SET_MOTOR           32
#define DHD_CMD_GET_DELTA_ENC       40
#define DHD_CMD_GET_WRIST_ENC       41
#define DHD_CMD_GET_ALL_ENC         42
#define DHD_CMD_GET_STATUS          50
#define DHD_CMD_SET_VELOCITY        60
#define DHD_CMD_GET_BUTTONS         70
#define DHD_CMD_GET_VERSION         80
#define DHD_CMD_GET_DHC_SYSTEM_TYPE 81
#define DHD_CMD_WRITE_FIRMWARE      160
#define DHD_CMD_READ_FIRMWARE       161    

// general data EEPROM memory mapping
#define   DHD_EEPROM_PARAM_OFFSET                100                             // initial index of config parameters
#define   DHD_EEPROM_DELTA_OFFSET                120                             // initial index of delta parameters
#define   DHD_EEPROM_WRIST_OFFSET                200                             // initial index of wrist parameters
#define   DHD_EEPROM_INTERNAL_ENCOFFSET0        0x00
#define   DHD_EEPROM_INTERNAL_ENCOFFSET1        0x01
#define   DHD_EEPROM_INTERNAL_ENCOFFSET2        0x02
#define   DHD_EEPROM_INTERNAL_ENCOFFSET3        0x03
#define   DHD_EEPROM_INTERNAL_ENCOFFSET4        0x04
#define   DHD_EEPROM_INTERNAL_ENCOFFSET5        0x05
#define   DHD_EEPROM_INTERNAL_CALIBSTATUS       0x10
#define   DHD_EEPROM_VERSION_H                  DHD_EEPROM_PARAM_OFFSET+0x00     // EEPROM Memory Model version
#define   DHD_EEPROM_VERSION_L                  DHD_EEPROM_PARAM_OFFSET+0x01     // 
#define   DHD_EEPROM_DHC_VERSION                DHD_EEPROM_PARAM_OFFSET+0x02     // controler version
#define   DHD_EEPROM_DHD_VERSION                DHD_EEPROM_PARAM_OFFSET+0x03     // delta version
#define   DHD_EEPROM_DHW_VERSION                DHD_EEPROM_PARAM_OFFSET+0x04     // wrist version
#define   DHD_EEPROM_SERIAL_NUMBER_H            DHD_EEPROM_PARAM_OFFSET+0x05     // DHC serial number
#define   DHD_EEPROM_SERIAL_NUMBER_L            DHD_EEPROM_PARAM_OFFSET+0x06

// HC11 memory map (for backcompatibility purposes only)
#define   DHD_EEPROM_HC11_OFFSET_0L             0x10
#define   DHD_EEPROM_HC11_OFFSET_1L             0x12
#define   DHD_EEPROM_HC11_OFFSET_2L             0x14
#define   DHD_EEPROM_HC11_KSPRING_0L            0x16
#define   DHD_EEPROM_HC11_KSPRING_1L            0x18
#define   DHD_EEPROM_HC11_KSPRING_2L            0x1A

// 6DOF calibration nuances threshold (between 500 and 1000 mark encoders)
#define   DHD_EEPROM_6DOF_500_THRESHOLD         1550

// memory models definition
#define   DHD_EEPROM_MEMORY_C8051_H             0x01
#define   DHD_EEPROM_MEMORY_C8051_L             0x10
#define   DHD_EEPROM_MEMORY_HC11_H              0xFF
#define   DHD_EEPROM_MEMORY_HC11_L              0xFF

// device definition
#define   DHD_COMUSB_VENDORID      0x1451
#define   DHD_COMUSB_PRODUCTID     0x0301


class dhdCom : public dhdTimeGuard {
protected:
  static int openCount;
  static int deviceTable[DHD_MAX_DEVICE];

protected:
  bool          rdy;
  int           index;
  int           systemType;
  uchar         buttons;
  uchar         status;
  int           enc [DHD_MAX_ENC];
  short         mot[DHD_MAX_MOTOR];
  Lock          mutex;
  MemoryModel   memoryModel;

public:

  dhdCom                            ();
  virtual ~dhdCom                   ();

  virtual int  open                 () = 0;
  virtual int  open                 (int index) = 0;
  virtual int  getID                () = 0;
  virtual int  getMemoryModel       () { return memoryModel; };
  virtual int  setMemoryModel       (MemoryModel memoryModel);
  virtual bool ready                () = 0;
  virtual int  stop                 ();
  virtual int  close                () = 0;

  virtual int  reset                () = 0;
  virtual int  calibrateWrist       () = 0;
  virtual int  getStatus            (uchar *status) = 0;
  virtual int  getVersion           (double *ver) = 0;
  virtual int  getSystemType        () = 0;
  virtual int  getButtons           (uchar *buttons) = 0;
  virtual int  getEncoder           (int index);
  virtual int  getAllEncoders       (int enc[DHD_MAX_ENC]);
  virtual int  getDeltaEncoders     (int *enc0, int *enc1, int *enc2) = 0;
  virtual int  getWristEncoders     (int *enc0, int *enc1, int *enc2) = 0;
  virtual int  getDeltaMotor        (short *mot0, short *mot1, short *mot2);

  virtual int  setMotor             (int index, short val) = 0;
  virtual int  setDeltaMotor        (short mot0, short mot1, short mot2) = 0;
  virtual int  setWristMotor        (short mot0, short mot1, short mot2) = 0;
  virtual int  setBrakesOn          () = 0;
  virtual int  setBrakesOff         () = 0;
  virtual int  setVelocityThreshold (uchar val) = 0;

  virtual int  setEMemory           (ushort addr, uchar *data, uchar length) = 0;
  virtual int  getEMemory           (ushort addr, uchar *data, uchar length) = 0;
  virtual int  setParameter         (int index, short   val);
  virtual int  getParameter         (int index, short  *val);
  virtual int  setParameter         (int index, double  val);
  virtual int  getParameter         (int index, double *val);
  virtual int  writeFirmware        (char *filename, int *progress);
  virtual int  readFirmware         (char *filename, int *progress);
  virtual int  setFirmware          (ushort addr, uchar *data, uchar length) = 0;
  virtual int  getFirmware          (ushort addr, uchar *data, uchar length) = 0;
};


#endif
