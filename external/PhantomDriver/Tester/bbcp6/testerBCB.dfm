object Form1: TForm1
  Left = 240
  Top = 200
  Width = 678
  Height = 336
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 288
    Top = 40
    Width = 121
    Height = 57
    Caption = 'start haptic loop'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 416
    Top = 40
    Width = 113
    Height = 57
    Caption = 'stop haptic loop'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 24
    Top = 56
    Width = 241
    Height = 21
    TabOrder = 2
  end
  object Edit2: TEdit
    Left = 24
    Top = 96
    Width = 81
    Height = 21
    TabOrder = 3
    Text = 'X'
  end
  object Edit3: TEdit
    Left = 112
    Top = 96
    Width = 81
    Height = 21
    TabOrder = 4
    Text = 'Y'
  end
  object Edit4: TEdit
    Left = 200
    Top = 96
    Width = 81
    Height = 21
    TabOrder = 5
    Text = 'Z'
  end
  object Edit5: TEdit
    Left = 24
    Top = 120
    Width = 81
    Height = 21
    TabOrder = 6
    Text = 'X'
  end
  object Edit6: TEdit
    Left = 112
    Top = 120
    Width = 81
    Height = 21
    TabOrder = 7
    Text = 'X'
  end
  object Edit7: TEdit
    Left = 200
    Top = 120
    Width = 81
    Height = 21
    TabOrder = 8
    Text = 'X'
  end
  object Edit8: TEdit
    Left = 24
    Top = 144
    Width = 81
    Height = 21
    TabOrder = 9
    Text = 'X'
  end
  object Edit9: TEdit
    Left = 112
    Top = 144
    Width = 81
    Height = 21
    TabOrder = 10
    Text = 'X'
  end
  object Edit10: TEdit
    Left = 200
    Top = 144
    Width = 81
    Height = 21
    TabOrder = 11
    Text = 'X'
  end
  object Edit11: TEdit
    Left = 24
    Top = 176
    Width = 81
    Height = 21
    TabOrder = 12
    Text = 'X'
  end
  object Edit12: TEdit
    Left = 112
    Top = 176
    Width = 81
    Height = 21
    TabOrder = 13
    Text = 'X'
  end
  object Edit13: TEdit
    Left = 200
    Top = 176
    Width = 81
    Height = 21
    TabOrder = 14
    Text = 'X'
  end
  object Button3: TButton
    Left = 288
    Top = 104
    Width = 121
    Height = 57
    Caption = 'Stop forces'
    TabOrder = 15
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 416
    Top = 104
    Width = 113
    Height = 57
    Caption = 'Start Forces'
    TabOrder = 16
    OnClick = Button4Click
  end
end
