object Form1: TForm1
  Left = 273
  Top = 191
  BorderStyle = bsDialog
  Caption = 'CHAI 3D - Mass Springs Example'
  ClientHeight = 544
  ClientWidth = 732
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
    Left = 3
    Top = 0
    Width = 0
    Height = 544
    Cursor = crHSplit
  end
  object Splitter1: TSplitter
    Left = 0
    Top = 0
    Width = 3
    Height = 544
    Cursor = crHSplit
  end
  object Panel1: TPanel
    Left = 10
    Top = 8
    Width = 713
    Height = 489
    BorderWidth = 3
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 0
    OnResize = Panel1Resize
  end
  object ToggleHapticsButton: TButton
    Left = 10
    Top = 504
    Width = 105
    Height = 25
    Caption = 'Toggle Haptics'
    TabOrder = 1
    OnClick = ToggleHapticsButtonClick
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 894
    Top = 8
  end
end
