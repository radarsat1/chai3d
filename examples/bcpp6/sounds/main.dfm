object Form1: TForm1
  Left = 266
  Top = 290
  Width = 1248
  Height = 858
  Caption = 'CHAI 3D - The Sound Of Physical Shapes'
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
    Left = 1240
    Top = 0
    Width = 0
    Height = 733
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1240
    Height = 733
    Align = alClient
    Color = clNone
    TabOrder = 0
    OnMouseDown = Panel1MouseDown
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
  end
  object Panel2: TPanel
    Left = 0
    Top = 733
    Width = 1240
    Height = 91
    Align = alBottom
    TabOrder = 1
    object Button1: TButton
      Left = 480
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
        
          'This example generates sounds based on frequencies, amplitudes, ' +
          'and decay coefficients from a '
        'data file, convolved with the haptic normal force.'
        ''
        
          'This demonstration is based on the algorithm by Kies Van den Doe' +
          'l K and Dinesh Pai in "The '
        'sounds of physical shapes"')
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
    end
    object Button3: TButton
      Left = 480
      Top = 56
      Width = 121
      Height = 25
      Caption = 'Exit'
      TabOrder = 2
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
