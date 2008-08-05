object Form1: TForm1
  Left = -228
  Top = 646
  Width = 1244
  Height = 852
  Caption = 'CHAI 3D - Record Player'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  WindowState = wsMaximized
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 1236
    Top = 0
    Width = 0
    Height = 728
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1236
    Height = 728
    Align = alClient
    Color = clNone
    TabOrder = 0
  end
  object Panel4: TPanel
    Left = 0
    Top = 728
    Width = 1236
    Height = 97
    Align = alBottom
    TabOrder = 1
    object Label2: TLabel
      Left = 573
      Top = 12
      Width = 40
      Height = 13
      Caption = 'Stiffness'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label13: TLabel
      Left = 572
      Top = 44
      Width = 64
      Height = 13
      Caption = 'Static Friction'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label14: TLabel
      Left = 572
      Top = 68
      Width = 78
      Height = 13
      Caption = 'Dynamic Friction'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Memo1: TMemo
      Left = 0
      Top = 0
      Width = 393
      Height = 97
      Color = cl3DDkShadow
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Lines.Strings = (
        
          'This example lets you use your haptic device to spin a record, d' +
          'emonstrating the '
        'use of sound in CHAI, via the BASS audio library. '
        ''
        
          'It also demonstrates the multinational composition of the CHAI d' +
          'evelopment team.'
        ''
        '')
      ParentFont = False
      ReadOnly = True
      TabOrder = 0
    end
    object Button2: TButton
      Left = 408
      Top = 18
      Width = 121
      Height = 23
      Caption = 'Toggle Haptics'
      TabOrder = 1
      OnClick = ToggleHapticsButtonClick
    end
    object Button1: TButton
      Left = 408
      Top = 56
      Width = 121
      Height = 25
      Caption = 'Exit'
      TabOrder = 2
      OnClick = Button1Click
    end
    object StiffnessSlider: TTrackBar
      Left = 656
      Top = 8
      Width = 97
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 30
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      ThumbLength = 15
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object StaticFrictionSlider: TTrackBar
      Left = 656
      Top = 40
      Width = 97
      Height = 33
      Max = 1000
      Orientation = trHorizontal
      Frequency = 1
      Position = 100
      SelEnd = 0
      SelStart = 0
      TabOrder = 4
      ThumbLength = 15
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object DynamicFrictionSlider: TTrackBar
      Left = 656
      Top = 64
      Width = 97
      Height = 25
      Max = 1000
      Orientation = trHorizontal
      Frequency = 1
      Position = 100
      SelEnd = 0
      SelStart = 0
      TabOrder = 5
      ThumbLength = 15
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object RecordSelect: TComboBox
      Left = 784
      Top = 12
      Width = 129
      Height = 21
      ItemHeight = 13
      TabOrder = 6
      OnChange = RecordSelectChange
    end
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 8
    Top = 8
  end
  object OpenDialog1: TOpenDialog
    Filter = '*.obj|*.obj|*.3ds|*.3ds'
    Left = 72
    Top = 8
  end
end
