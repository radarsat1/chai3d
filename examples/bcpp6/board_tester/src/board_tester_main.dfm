object Form1: TForm1
  Left = 343
  Top = 371
  BorderStyle = bsDialog
  Caption = 'Board example'
  ClientHeight = 521
  ClientWidth = 699
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 16
  object Label2: TLabel
    Left = 404
    Top = 177
    Width = 113
    Height = 25
    Caption = 'Board Status'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object GroupBox3: TGroupBox
    Left = 20
    Top = 2
    Width = 336
    Height = 297
    Caption = 'Encoders'
    TabOrder = 4
    object Button3: TButton
      Left = 30
      Top = 39
      Width = 121
      Height = 31
      Caption = 'Get encoder 0'
      TabOrder = 0
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 30
      Top = 69
      Width = 121
      Height = 31
      Caption = 'Get encoder 1'
      TabOrder = 1
      OnClick = Button4Click
    end
    object Button5: TButton
      Left = 30
      Top = 98
      Width = 121
      Height = 31
      Caption = 'Get encoder 2'
      TabOrder = 2
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 30
      Top = 128
      Width = 121
      Height = 31
      Caption = 'Get encoder 3'
      TabOrder = 3
      OnClick = Button6Click
    end
    object Button7: TButton
      Left = 30
      Top = 158
      Width = 121
      Height = 30
      Caption = 'Get encoder 4'
      TabOrder = 4
      OnClick = Button7Click
    end
    object Button8: TButton
      Left = 30
      Top = 187
      Width = 121
      Height = 31
      Caption = 'Get encoder 5'
      TabOrder = 5
      OnClick = Button8Click
    end
    object Button9: TButton
      Left = 30
      Top = 217
      Width = 121
      Height = 30
      Caption = 'Get encoder 6'
      TabOrder = 6
      OnClick = Button9Click
    end
    object Button10: TButton
      Left = 30
      Top = 246
      Width = 121
      Height = 31
      Caption = 'Get encoder 7'
      TabOrder = 7
      OnClick = Button10Click
    end
    object Edit2: TEdit
      Left = 158
      Top = 42
      Width = 148
      Height = 24
      TabOrder = 8
    end
    object Edit3: TEdit
      Left = 158
      Top = 73
      Width = 148
      Height = 24
      TabOrder = 9
    end
    object Edit4: TEdit
      Left = 158
      Top = 103
      Width = 148
      Height = 24
      TabOrder = 10
    end
    object Edit5: TEdit
      Left = 158
      Top = 133
      Width = 148
      Height = 24
      TabOrder = 11
    end
    object Edit6: TEdit
      Left = 158
      Top = 162
      Width = 148
      Height = 24
      TabOrder = 12
    end
    object Edit7: TEdit
      Left = 158
      Top = 192
      Width = 148
      Height = 24
      TabOrder = 13
    end
    object Edit8: TEdit
      Left = 158
      Top = 222
      Width = 148
      Height = 24
      TabOrder = 14
    end
    object Edit9: TEdit
      Left = 158
      Top = 251
      Width = 148
      Height = 24
      TabOrder = 15
    end
  end
  object Button1: TButton
    Left = 414
    Top = 39
    Width = 168
    Height = 51
    Caption = 'Start Servotogo Board'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 414
    Top = 89
    Width = 168
    Height = 50
    Caption = 'Start Sensoray626 Board'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 404
    Top = 207
    Width = 267
    Height = 24
    TabOrder = 2
  end
  object GroupBox1: TGroupBox
    Left = 30
    Top = 305
    Width = 660
    Height = 208
    Caption = 'DAC'
    TabOrder = 3
    object Label1: TLabel
      Left = 325
      Top = 95
      Width = 113
      Height = 25
      Caption = 'Volts to DAC'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Button11: TButton
      Left = 20
      Top = 59
      Width = 122
      Height = 31
      Caption = 'Write DAC 0'
      TabOrder = 0
      OnClick = Button11Click
    end
    object Button12: TButton
      Left = 20
      Top = 89
      Width = 122
      Height = 30
      Caption = 'Write DAC 1'
      TabOrder = 1
      OnClick = Button12Click
    end
    object Button13: TButton
      Left = 20
      Top = 118
      Width = 122
      Height = 31
      Caption = 'Write DAC 2'
      TabOrder = 2
      OnClick = Button13Click
    end
    object Button14: TButton
      Left = 20
      Top = 148
      Width = 122
      Height = 29
      Caption = 'Write DAC 3'
      TabOrder = 3
      OnClick = Button14Click
    end
    object Button15: TButton
      Left = 148
      Top = 59
      Width = 122
      Height = 31
      Caption = 'Write DAC 4'
      TabOrder = 4
      OnClick = Button15Click
    end
    object Button16: TButton
      Left = 148
      Top = 89
      Width = 122
      Height = 30
      Caption = 'Write DAC 5'
      TabOrder = 5
      OnClick = Button16Click
    end
    object Button17: TButton
      Left = 148
      Top = 118
      Width = 122
      Height = 31
      Caption = 'Write DAC 6'
      TabOrder = 6
      OnClick = Button17Click
    end
    object Button18: TButton
      Left = 148
      Top = 148
      Width = 122
      Height = 30
      Caption = 'Write DAC 7'
      TabOrder = 7
      OnClick = Button18Click
    end
    object TckDACvolts: TTrackBar
      Left = 325
      Top = 39
      Width = 316
      Height = 41
      Min = -10
      Orientation = trHorizontal
      Frequency = 1
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 8
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = TckDACvoltsChange
    end
    object Edit18: TEdit
      Left = 453
      Top = 94
      Width = 188
      Height = 24
      TabOrder = 9
    end
    object Button19: TButton
      Left = 404
      Top = 138
      Width = 178
      Height = 50
      Caption = 'Zero All DACs'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 10
      OnClick = Button19Click
    end
  end
end
