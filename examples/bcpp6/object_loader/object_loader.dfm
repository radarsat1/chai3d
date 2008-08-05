object Form1: TForm1
  Left = 221
  Top = 181
  Width = 808
  Height = 694
  Caption = 'CHAI 3D - Basic Shapes Demo'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 639
    Top = 0
    Width = 0
    Height = 667
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 639
    Height = 667
    Align = alClient
    Color = clNone
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 639
    Top = 0
    Width = 161
    Height = 667
    Align = alRight
    TabOrder = 1
    object GroupBox1: TGroupBox
      Left = 10
      Top = 12
      Width = 143
      Height = 189
      Caption = ' Options '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      object Label8: TLabel
        Left = 12
        Top = 24
        Width = 78
        Height = 13
        Caption = 'Camera Zoom'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label9: TLabel
        Left = 12
        Top = 80
        Width = 99
        Height = 13
        Caption = 'Camera V - Angle'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label10: TLabel
        Left = 12
        Top = 136
        Width = 100
        Height = 13
        Caption = 'Camera H - Angle'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object CameraZoomSlider: TTrackBar
        Left = 8
        Top = 40
        Width = 129
        Height = 33
        Max = 50
        Min = 1
        Orientation = trHorizontal
        Frequency = 1
        Position = 25
        SelEnd = 0
        SelStart = 0
        TabOrder = 0
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = updateCamera
      end
      object VAngleSlider: TTrackBar
        Left = 8
        Top = 96
        Width = 129
        Height = 33
        Max = 80
        Min = -80
        Orientation = trHorizontal
        Frequency = 1
        Position = 25
        SelEnd = 0
        SelStart = 0
        TabOrder = 1
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = updateCamera
      end
      object HAngleSlider: TTrackBar
        Left = 8
        Top = 152
        Width = 129
        Height = 33
        Max = 180
        Min = -180
        Orientation = trHorizontal
        Frequency = 1
        Position = 30
        SelEnd = 0
        SelStart = 0
        TabOrder = 2
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = updateCamera
      end
    end
    object ToggleHapticsButton: TButton
      Left = 32
      Top = 216
      Width = 105
      Height = 25
      Caption = 'Toggle Haptics'
      TabOrder = 1
      OnClick = ToggleHapticsButtonClick
    end
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 8
    Top = 8
  end
end
