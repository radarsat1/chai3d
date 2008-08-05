object Form1: TForm1
  Left = 290
  Top = 238
  Width = 925
  Height = 822
  Caption = 'CHAI 3D - Implicit Functions'
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
    Height = 697
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 917
    Height = 697
    Align = alClient
    Color = clNone
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 0
    Top = 697
    Width = 917
    Height = 91
    Align = alBottom
    TabOrder = 1
    object Label1: TLabel
      Left = 512
      Top = 19
      Width = 40
      Height = 14
      Caption = 'Stiffness'
    end
    object Label2: TLabel
      Left = 656
      Top = 19
      Width = 64
      Height = 14
      Caption = 'Static Friction'
    end
    object Label3: TLabel
      Left = 800
      Top = 19
      Width = 78
      Height = 14
      Caption = 'Dynamic Friction'
    end
    object Button2: TButton
      Left = 328
      Top = 14
      Width = 121
      Height = 27
      Caption = 'Toggle Haptics'
      TabOrder = 0
      OnClick = ToggleHapticsButtonClick
    end
    object Memo1: TMemo
      Left = 1
      Top = 1
      Width = 320
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
        ''
        
          'This example illustrates rendering using implicit functions, acc' +
          'ording '
        
          'to the algorithm in Salisbury and Tarr'#39's paper "Haptic Rendering' +
          ' of '
        'Surfaces Defined by Implicit Functions".')
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
    end
    object Button1: TButton
      Left = 328
      Top = 56
      Width = 121
      Height = 25
      Caption = 'Exit'
      TabOrder = 2
      OnClick = Button1Click
    end
    object Stiffness: TTrackBar
      Left = 458
      Top = 43
      Width = 150
      Height = 29
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 30
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object DynamicFriction: TTrackBar
      Left = 762
      Top = 43
      Width = 150
      Height = 29
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 40
      SelEnd = 0
      SelStart = 0
      TabOrder = 4
      TickMarks = tmBottomRight
      TickStyle = tsAuto
    end
    object StaticFriction: TTrackBar
      Left = 610
      Top = 43
      Width = 150
      Height = 29
      Max = 100
      Orientation = trHorizontal
      Frequency = 1
      Position = 60
      SelEnd = 0
      SelStart = 0
      TabOrder = 5
      TickMarks = tmBottomRight
      TickStyle = tsAuto
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
