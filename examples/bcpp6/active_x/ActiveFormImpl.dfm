object ActiveFormX: TActiveFormX
  Left = 402
  Top = 228
  Width = 349
  Height = 289
  Caption = 'ActiveFormX'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 341
    Height = 255
    Align = alClient
    Caption = 'Panel1'
    TabOrder = 0
    OnDblClick = Panel1DblClick
    OnMouseDown = Panel1MouseDown
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 48
    Top = 32
  end
end
