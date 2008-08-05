object Form1: TForm1
  Left = 317
  Top = 195
  Width = 933
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
  WindowState = wsMaximized
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 724
    Top = 0
    Width = 0
    Height = 815
    Cursor = crHSplit
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 724
    Height = 815
    Align = alClient
    Color = clNone
    TabOrder = 0
    OnMouseDown = Panel1MouseDown
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
  end
  object Panel4: TPanel
    Left = 724
    Top = 0
    Width = 201
    Height = 815
    Align = alRight
    TabOrder = 1
    object GroupBox2: TGroupBox
      Left = 8
      Top = 16
      Width = 185
      Height = 65
      Caption = ' Commands '
      TabOrder = 0
      object Button1: TButton
        Left = 9
        Top = 24
        Width = 80
        Height = 25
        Caption = 'Load Model'
        TabOrder = 0
        OnClick = LoadModelButtonClick
      end
      object Button2: TButton
        Left = 96
        Top = 24
        Width = 81
        Height = 25
        Caption = 'Toggle Haptics'
        TabOrder = 1
        OnClick = ToggleHapticsButtonClick
      end
    end
    object GroupBox3: TGroupBox
      Left = 8
      Top = 88
      Width = 185
      Height = 233
      Caption = ' Display Options '
      TabOrder = 1
      object Label16: TLabel
        Left = 20
        Top = 192
        Width = 36
        Height = 13
        Caption = 'Opacity'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object WireFrameBox: TCheckBox
        Left = 16
        Top = 16
        Width = 97
        Height = 17
        Caption = 'Wire Frame'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
      object ShowFrameBox: TCheckBox
        Left = 16
        Top = 40
        Width = 97
        Height = 17
        Caption = 'Show Frame'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object ShowBoundaryBoxBox: TCheckBox
        Left = 16
        Top = 64
        Width = 121
        Height = 17
        Caption = 'Show Boundary Box'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
      object ShowNormalsBox: TCheckBox
        Left = 16
        Top = 88
        Width = 97
        Height = 17
        Caption = 'Show Normals'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
      end
      object UseTextureMappingBox: TCheckBox
        Left = 16
        Top = 112
        Width = 121
        Height = 17
        Caption = 'Use Texture Mapping'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 4
      end
      object UseVertexColorsBox: TCheckBox
        Left = 16
        Top = 136
        Width = 113
        Height = 17
        Caption = 'Use Vertex Colors'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 5
      end
      object UseMaterialPropertiesBox: TCheckBox
        Left = 16
        Top = 160
        Width = 129
        Height = 17
        Caption = 'Use Material Properties'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 6
      end
      object TransparencyLevelSlider: TTrackBar
        Left = 72
        Top = 188
        Width = 97
        Height = 25
        Orientation = trHorizontal
        Frequency = 1
        Position = 10
        SelEnd = 0
        SelStart = 0
        TabOrder = 7
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
    end
    object GroupBox5: TGroupBox
      Left = 8
      Top = 440
      Width = 185
      Height = 145
      Caption = ' Collision Detection '
      TabOrder = 2
      object Label17: TLabel
        Left = 12
        Top = 108
        Width = 29
        Height = 13
        Caption = 'Depth'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object AABBTreeButton: TRadioButton
        Left = 14
        Top = 48
        Width = 113
        Height = 17
        Caption = 'Use AABB Tree'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        TabStop = True
        OnClick = AABBTreeButtonClick
      end
      object SphereTreeButton: TRadioButton
        Left = 14
        Top = 24
        Width = 113
        Height = 17
        Caption = 'Use Sphere Tree'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = AABBTreeButtonClick
      end
      object ShowCollisionDetectorBox: TCheckBox
        Left = 16
        Top = 80
        Width = 137
        Height = 17
        Caption = 'Show Collision Detector'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
      object CollisionDepthSlider: TTrackBar
        Left = 72
        Top = 104
        Width = 105
        Height = 33
        Max = 20
        Orientation = trHorizontal
        Frequency = 1
        Position = 0
        SelEnd = 0
        SelStart = 0
        TabOrder = 3
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = CollisionDepthSliderChange
      end
    end
    object GroupBox6: TGroupBox
      Left = 8
      Top = 592
      Width = 185
      Height = 145
      Caption = ' Haptics Options '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      object Label2: TLabel
        Left = 13
        Top = 28
        Width = 40
        Height = 13
        Caption = 'Stiffness'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label13: TLabel
        Left = 20
        Top = 76
        Width = 27
        Height = 13
        Caption = 'Static'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label14: TLabel
        Left = 20
        Top = 108
        Width = 41
        Height = 13
        Caption = 'Dynamic'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label1: TLabel
        Left = 12
        Top = 52
        Width = 37
        Height = 13
        Caption = 'Friction:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object StiffnessSlider: TTrackBar
        Left = 72
        Top = 24
        Width = 97
        Height = 33
        Max = 100
        Orientation = trHorizontal
        Frequency = 1
        Position = 35
        SelEnd = 0
        SelStart = 0
        TabOrder = 0
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
      object StaticFrictionSlider: TTrackBar
        Left = 72
        Top = 72
        Width = 97
        Height = 33
        Max = 100
        Orientation = trHorizontal
        Frequency = 1
        Position = 0
        SelEnd = 0
        SelStart = 0
        TabOrder = 1
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
      object DynamicFrictionSlider: TTrackBar
        Left = 72
        Top = 104
        Width = 97
        Height = 33
        Max = 100
        Orientation = trHorizontal
        Frequency = 1
        Position = 0
        SelEnd = 0
        SelStart = 0
        TabOrder = 2
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
    end
    object GroupBox4: TGroupBox
      Left = 8
      Top = 328
      Width = 185
      Height = 105
      Caption = ' Stereo Settings '
      TabOrder = 4
      object Label7: TLabel
        Left = 8
        Top = 48
        Width = 72
        Height = 13
        Caption = 'Eye Separation'
      end
      object Label11: TLabel
        Left = 8
        Top = 74
        Width = 62
        Height = 13
        Caption = 'Focal Length'
      end
      object UseStereoBox: TCheckBox
        Left = 9
        Top = 24
        Width = 96
        Height = 17
        Caption = 'Use Stereo'
        TabOrder = 0
      end
      object EyeSeparationSlider: TTrackBar
        Left = 88
        Top = 40
        Width = 89
        Height = 33
        Max = 100
        Orientation = trHorizontal
        Frequency = 1
        Position = 25
        SelEnd = 0
        SelStart = 0
        TabOrder = 1
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
      object FocalLengthSlider: TTrackBar
        Left = 88
        Top = 72
        Width = 89
        Height = 25
        Orientation = trHorizontal
        Frequency = 1
        Position = 4
        SelEnd = 0
        SelStart = 0
        TabOrder = 2
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
      end
    end
    object GroupBox7: TGroupBox
      Left = 8
      Top = 744
      Width = 185
      Height = 65
      Caption = ' Model Information '
      TabOrder = 5
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
    Filter = '*.3ds *.obj|*.obj;*.3ds| | '
    Left = 40
    Top = 8
  end
end
