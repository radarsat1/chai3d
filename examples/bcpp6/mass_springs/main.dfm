object Form1: TForm1
  Left = 273
  Top = 191
  Width = 743
  Height = 576
  Caption = 'CHAI 3D - Mass Springs Example'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = MassSpringFormClose
  OnCreate = MassSpringFormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 0
    Top = 0
    Width = 0
    Height = 475
    Cursor = crHSplit
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 735
    Height = 475
    Align = alClient
    BorderWidth = 3
    Color = clNone
    TabOrder = 0
    OnResize = Panel1Resize
  end
  object Panel2: TPanel
    Left = 0
    Top = 475
    Width = 735
    Height = 74
    Align = alBottom
    TabOrder = 1
    object ToggleHapticsButton: TButton
      Left = 274
      Top = 8
      Width = 119
      Height = 25
      Caption = 'Toggle Haptics'
      TabOrder = 0
      OnClick = ToggleHapticsButtonClick
    end
    object Memo1: TMemo
      Left = 1
      Top = 1
      Width = 264
      Height = 72
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
        'This example illustrates the implementation of a simple '
        'spring mass model.')
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
    end
    object Button1: TButton
      Left = 272
      Top = 40
      Width = 121
      Height = 25
      Caption = 'Exit'
      TabOrder = 2
      OnClick = Button1Click
    end
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 894
    Top = 8
  end
end
