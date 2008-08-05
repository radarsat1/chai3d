object Form2: TForm2
  Left = 243
  Top = 197
  BorderStyle = bsDialog
  Caption = 'CHAI 3D - Math'
  ClientHeight = 470
  ClientWidth = 681
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 209
    Height = 121
    Caption = ' Matrix M0 '
    TabOrder = 0
    object edtM0V00: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 0
    end
    object edtM0V10: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 1
    end
    object edtM0V20: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 2
    end
    object edtM0V01: TEdit
      Left = 80
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 3
    end
    object edtM0V11: TEdit
      Left = 80
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 4
    end
    object edtM0V21: TEdit
      Left = 80
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 5
    end
    object edtM0V02: TEdit
      Left = 144
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 6
    end
    object edtM0V12: TEdit
      Left = 144
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 7
    end
    object edtM0V22: TEdit
      Left = 144
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 8
    end
  end
  object GroupBox2: TGroupBox
    Left = 232
    Top = 8
    Width = 209
    Height = 121
    Caption = ' Matrix M1'
    TabOrder = 1
    object edtM1V00: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 0
    end
    object edtM1V10: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 1
    end
    object edtM1V20: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 2
    end
    object edtM1V01: TEdit
      Left = 80
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 3
    end
    object edtM1V11: TEdit
      Left = 80
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 4
    end
    object edtM1V21: TEdit
      Left = 80
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 5
    end
    object edtM1V02: TEdit
      Left = 144
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 6
    end
    object edtM1V12: TEdit
      Left = 144
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 7
    end
    object edtM1V22: TEdit
      Left = 144
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 8
    end
  end
  object GroupBox3: TGroupBox
    Left = 456
    Top = 8
    Width = 209
    Height = 121
    Caption = ' Matrix MR '
    TabOrder = 2
    object edtMRV00: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 0
    end
    object edtMRV10: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 1
    end
    object edtMRV20: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 2
    end
    object edtMRV01: TEdit
      Left = 80
      Top = 24
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 3
    end
    object edtMRV11: TEdit
      Left = 80
      Top = 56
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 4
    end
    object edtMRV21: TEdit
      Left = 80
      Top = 88
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 5
    end
    object edtMRV02: TEdit
      Left = 144
      Top = 24
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 6
    end
    object edtMRV12: TEdit
      Left = 144
      Top = 56
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 7
    end
    object edtMRV22: TEdit
      Left = 144
      Top = 88
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 8
    end
  end
  object GroupBox4: TGroupBox
    Left = 72
    Top = 136
    Width = 81
    Height = 121
    Caption = ' Vector V0 '
    TabOrder = 3
    object edtV0X: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 0
    end
    object edtV0Y: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 1
    end
    object edtV0Z: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 2
    end
  end
  object GroupBox5: TGroupBox
    Left = 296
    Top = 136
    Width = 81
    Height = 121
    Caption = ' Vector V1 '
    TabOrder = 4
    object edtV1X: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      TabOrder = 0
    end
    object edtV1Y: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      TabOrder = 1
    end
    object edtV1Z: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      TabOrder = 2
    end
  end
  object GroupBox6: TGroupBox
    Left = 520
    Top = 136
    Width = 81
    Height = 121
    Caption = ' Vector VR'
    TabOrder = 5
    object edtVRX: TEdit
      Left = 16
      Top = 24
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 0
    end
    object edtVRY: TEdit
      Left = 16
      Top = 56
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 1
    end
    object edtVRZ: TEdit
      Left = 16
      Top = 88
      Width = 49
      Height = 21
      ReadOnly = True
      TabOrder = 2
    end
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 272
    Width = 665
    Height = 129
    ActivePage = TabSheet1
    TabIndex = 0
    TabOrder = 6
    object TabSheet1: TTabSheet
      Caption = 'Vector Operations'
      object Button2: TButton
        Left = 16
        Top = 56
        Width = 97
        Height = 25
        Caption = 'VR = V0 - V1'
        TabOrder = 0
        OnClick = Button2Click
      end
      object Button1: TButton
        Left = 16
        Top = 24
        Width = 97
        Height = 25
        Caption = 'VR = V0 + V1'
        TabOrder = 1
        OnClick = Button1Click
      end
      object Button3: TButton
        Left = 144
        Top = 24
        Width = 97
        Height = 25
        Caption = 'VR = 2 * VR'
        TabOrder = 2
        OnClick = Button3Click
      end
      object Button4: TButton
        Left = 144
        Top = 56
        Width = 97
        Height = 25
        Caption = 'VR = VR / 2'
        TabOrder = 3
        OnClick = Button4Click
      end
      object Button5: TButton
        Left = 400
        Top = 24
        Width = 97
        Height = 25
        Caption = 'Normalize (VR)'
        TabOrder = 4
        OnClick = Button5Click
      end
      object Button6: TButton
        Left = 264
        Top = 24
        Width = 113
        Height = 25
        Caption = 'VR = V0 X V1'
        TabOrder = 5
        OnClick = Button6Click
      end
      object Button7: TButton
        Left = 264
        Top = 56
        Width = 113
        Height = 25
        Caption = 'VR = VR X V1'
        TabOrder = 6
        OnClick = Button7Click
      end
      object Button8: TButton
        Left = 400
        Top = 56
        Width = 97
        Height = 25
        Caption = 'VR = V0 * V1'
        TabOrder = 7
        OnClick = Button8Click
      end
      object edtDot: TEdit
        Left = 520
        Top = 56
        Width = 105
        Height = 21
        TabOrder = 8
      end
      object Button16: TButton
        Left = 520
        Top = 24
        Width = 105
        Height = 25
        Caption = 'Length (VR)'
        TabOrder = 9
        OnClick = Button16Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Matrix Operations'
      ImageIndex = 1
      object Button9: TButton
        Left = 8
        Top = 24
        Width = 97
        Height = 25
        Caption = 'Identity'
        TabOrder = 0
        OnClick = Button9Click
      end
      object Button10: TButton
        Left = 8
        Top = 56
        Width = 97
        Height = 25
        Caption = 'MR = M0 * M1'
        TabOrder = 1
        OnClick = Button10Click
      end
      object Button11: TButton
        Left = 128
        Top = 24
        Width = 89
        Height = 25
        Caption = 'Invert M0'
        TabOrder = 2
        OnClick = Button11Click
      end
      object Button12: TButton
        Left = 128
        Top = 56
        Width = 89
        Height = 25
        Caption = 'M1 = M0'
        TabOrder = 3
        OnClick = Button12Click
      end
      object Button13: TButton
        Left = 240
        Top = 24
        Width = 97
        Height = 25
        Caption = 'Trans (MR)'
        TabOrder = 4
        OnClick = Button13Click
      end
      object Button14: TButton
        Left = 240
        Top = 56
        Width = 97
        Height = 25
        Caption = 'MR = Trans (M1)'
        TabOrder = 5
        OnClick = Button14Click
      end
      object Button15: TButton
        Left = 360
        Top = 24
        Width = 97
        Height = 25
        Caption = 'VR = M0 * V0'
        TabOrder = 6
        OnClick = Button15Click
      end
      object Button17: TButton
        Left = 360
        Top = 56
        Width = 97
        Height = 25
        Caption = 'Clear MR'
        TabOrder = 7
        OnClick = Button17Click
      end
      object edtMatrix: TEdit
        Left = 512
        Top = 24
        Width = 121
        Height = 21
        TabOrder = 8
      end
    end
  end
  object Memo1: TMemo
    Left = 8
    Top = 408
    Width = 665
    Height = 57
    BorderStyle = bsNone
    Color = cl3DDkShadow
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      ''
      
        'This example illustrates some of the vector and matrix capabilit' +
        'ies offered by the CHAI math library.'
      'Related classes: CVector3d and CMatrix3d')
    ParentFont = False
    ReadOnly = True
    TabOrder = 7
  end
end
