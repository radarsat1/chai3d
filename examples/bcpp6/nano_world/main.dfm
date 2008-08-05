object Form1: TForm1
  Left = 285
  Top = 197
  Width = 1080
  Height = 820
  Caption = 'CHAI 3D - Nano World'
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
    Left = 871
    Top = 0
    Width = 0
    Height = 793
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 871
    Height = 793
    Align = alClient
    Color = clNone
    TabOrder = 0
    OnMouseDown = Panel1MouseDown
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
    OnResize = Panel1Resize
  end
  object Panel4: TPanel
    Left = 871
    Top = 0
    Width = 201
    Height = 793
    Align = alRight
    TabOrder = 1
    object Button3: TButton
      Left = 8
      Top = 96
      Width = 185
      Height = 25
      Caption = 'Exit Application'
      TabOrder = 0
      OnClick = Button3Click
    end
    object Button1: TButton
      Left = 9
      Top = 56
      Width = 184
      Height = 25
      Caption = 'Load Image Map'
      TabOrder = 1
      OnClick = LoadModelButtonClick
    end
    object btnToggleHaptics: TButton
      Left = 8
      Top = 16
      Width = 185
      Height = 25
      Caption = 'Toggle Haptics'
      TabOrder = 2
      OnClick = ToggleHapticsButtonClick
    end
    object Panel2: TPanel
      Left = 8
      Top = 358
      Width = 185
      Height = 193
      TabOrder = 3
      object Image1: TImage
        Left = 1
        Top = 1
        Width = 183
        Height = 191
        Align = alClient
      end
    end
    object Memo1: TMemo
      Left = 8
      Top = 136
      Width = 185
      Height = 209
      Align = alCustom
      BorderStyle = bsNone
      Color = cl3DDkShadow
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Lines.Strings = (
        ''
        'This example creates a triangle based '
        'height map from any bitmap image.'
        ''
        'By clicking on the button [Toggle '
        'Haptics]  the haptic thread is launched. '
        ''
        'The application will connect to the first '
        'available haptic device; if no device is '
        'found the program starts the Virtual '
        'Device. '
        ''
        'To zoom or rotate the camera, use '
        'computer mouse [LEFT] and [RIGHT] '
        'button.'
        '')
      ParentFont = False
      ReadOnly = True
      TabOrder = 4
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 20
    OnTimer = Timer1Timer
    Left = 8
    Top = 8
  end
  object OpenPictureDialog1: TOpenPictureDialog
    Filter = 'Bitmaps (*.bmp)|*.bmp'
    Left = 48
    Top = 8
  end
end
