object Form1: TForm1
  Left = 290
  Top = 171
  Width = 925
  Height = 822
  Caption = 'CHAI 3D - Dynamics'
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
    Left = 917
    Top = 0
    Width = 0
    Height = 704
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 917
    Height = 704
    Align = alClient
    Color = clNone
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 0
    Top = 704
    Width = 917
    Height = 91
    Align = alBottom
    TabOrder = 1
    object Label1: TLabel
      Left = 768
      Top = 8
      Width = 40
      Height = 13
      Caption = 'Stiffness'
    end
    object Label2: TLabel
      Left = 768
      Top = 40
      Width = 64
      Height = 13
      Caption = 'Static Friction'
    end
    object Label3: TLabel
      Left = 770
      Top = 67
      Width = 78
      Height = 13
      Caption = 'Dynamic Friction'
    end
    object Button2: TButton
      Left = 480
      Top = 6
      Width = 121
      Height = 27
      Caption = 'Toggle Haptics'
      TabOrder = 0
      OnClick = ToggleHapticsButtonClick
    end
    object Memo1: TMemo
      Left = 1
      Top = 1
      Width = 465
      Height = 89
      Align = alLeft
      BorderStyle = bsNone
      Color = cl3DDkShadow
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Lines.Strings = (
        ''
        'This example connects CHAI with ODE (Open Dynamics Engine), an '
        
          'open-source physics engine (www.ode.org) that allows for the eas' +
          'y '
        
          'creation of dynamic virtual worlds.  In addition, this example s' +
          'hows '
        
          'how to use two devices within the same world. This example lets ' +
          'the user '
        
          'push around a box in a small room, similar to the SensAble Dice ' +
          'demo.')
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
    end
    object Button1: TButton
      Left = 480
      Top = 64
      Width = 121
      Height = 25
      Caption = 'Exit'
      TabOrder = 2
      OnClick = Button1Click
    end
    object StiffnessSlider: TTrackBar
      Left = 618
      Top = 4
      Width = 150
      Height = 26
      Max = 50
      Orientation = trHorizontal
      Frequency = 1
      Position = 20
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object StaticSlider: TTrackBar
      Left = 618
      Top = 32
      Width = 150
      Height = 26
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 30
      SelEnd = 0
      SelStart = 0
      TabOrder = 4
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object DynamicSlider: TTrackBar
      Left = 619
      Top = 61
      Width = 150
      Height = 26
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 30
      SelEnd = 0
      SelStart = 0
      TabOrder = 5
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object Button3: TButton
      Left = 480
      Top = 35
      Width = 121
      Height = 27
      Caption = 'Toggle Second Device'
      TabOrder = 6
      OnClick = Button3Click
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
