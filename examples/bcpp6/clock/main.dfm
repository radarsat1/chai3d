object Form1: TForm1
  Left = 288
  Top = 459
  BorderStyle = bsDialog
  Caption = 'CHAI 3D - Precision Clock'
  ClientHeight = 391
  ClientWidth = 320
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 24
    Width = 3
    Height = 13
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 305
    Height = 81
    Caption = ' Precision Time '
    TabOrder = 0
    object Button1: TButton
      Left = 16
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Start'
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 112
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Stop'
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 208
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Init'
      TabOrder = 2
      OnClick = Button3Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 96
    Width = 305
    Height = 81
    Caption = ' Current Time [micro seconds] '
    TabOrder = 1
    object labelTime: TLabel
      Left = 24
      Top = 24
      Width = 18
      Height = 37
      Caption = '0'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -32
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 184
    Width = 305
    Height = 105
    Caption = ' Timeout [micro seconds] '
    TabOrder = 2
    object labelPeriod: TLabel
      Left = 24
      Top = 24
      Width = 18
      Height = 37
      Caption = '0'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -32
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object labelMessage: TLabel
      Left = 24
      Top = 72
      Width = 3
      Height = 13
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
  object Memo1: TMemo
    Left = 8
    Top = 296
    Width = 305
    Height = 89
    BorderStyle = bsNone
    Color = cl3DDkShadow
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      'This example illustrates the use of class CPrecisionClock '
      'which provides a accurate way to measure time at a micro '
      'second  precision level. High precision clocks come in handy '
      'when  developing  dynamic simulations which need to run '
      'at a same speed on every type of computer regardless the '
      'CPU speed.')
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
  end
end
