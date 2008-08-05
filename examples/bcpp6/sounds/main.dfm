object Form1: TForm1
  Left = 164
  Top = 323
  Width = 1356
  Height = 842
  Caption = 'CHAI 3D - Object Loader'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 1147
    Top = 0
    Width = 0
    Height = 808
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1147
    Height = 808
    Align = alClient
    Color = clNone
    TabOrder = 0
    OnMouseDown = Panel1MouseDown
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
    object StatusBar1: TStatusBar
      Left = 1
      Top = 788
      Width = 1145
      Height = 19
      Panels = <>
      SimplePanel = False
    end
  end
  object Panel4: TPanel
    Left = 1147
    Top = 0
    Width = 201
    Height = 808
    Align = alRight
    TabOrder = 1
    object GroupBox2: TGroupBox
      Left = 8
      Top = 16
      Width = 185
      Height = 65
      Caption = ' Commands '
      TabOrder = 0
      object Button2: TButton
        Left = 8
        Top = 24
        Width = 81
        Height = 25
        Caption = 'Toggle Haptics'
        TabOrder = 0
        OnClick = ToggleHapticsButtonClick
      end
    end
    object GroupBox7: TGroupBox
      Left = 8
      Top = 744
      Width = 185
      Height = 65
      Caption = ' Model Information '
      TabOrder = 1
      object Label5: TLabel
        Left = 16
        Top = 36
        Width = 43
        Height = 13
        Caption = 'Triangles'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label4: TLabel
        Left = 16
        Top = 20
        Width = 41
        Height = 13
        Caption = 'Vertices:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object NumVerticesLabel: TLabel
        Left = 72
        Top = 20
        Width = 6
        Height = 13
        Caption = '0'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
      end
      object NumTrianglesLabel: TLabel
        Left = 72
        Top = 36
        Width = 6
        Height = 13
        Caption = '0'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
      end
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
