object frmSubtitleEdit: TfrmSubtitleEdit
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Subtitle Edit'
  ClientHeight = 482
  ClientWidth = 902
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 642
    Top = 0
    Width = 260
    Height = 482
    Align = alRight
    BevelEdges = [beLeft]
    BevelKind = bkFlat
    BevelOuter = bvNone
    TabOrder = 0
    object lbLines: TListBox
      Left = 4
      Top = 6
      Width = 245
      Height = 99
      BevelInner = bvNone
      BevelOuter = bvNone
      ItemHeight = 13
      TabOrder = 0
      OnClick = lbLinesClick
    end
    object PageControl2: TPageControl
      Left = 8
      Top = 173
      Width = 245
      Height = 229
      ActivePage = TabSheet6
      TabOrder = 1
      object TabSheet5: TTabSheet
        Caption = 'Font'
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblFont: TLabel
          Left = 8
          Top = 16
          Width = 26
          Height = 13
          Caption = 'Font:'
        end
        object btnFont: TButton
          Left = 8
          Top = 35
          Width = 75
          Height = 25
          Caption = 'Change'
          TabOrder = 0
          OnClick = btnFontClick
        end
      end
      object TabSheet6: TTabSheet
        Caption = 'Color'
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblFontColor: TLabel
          Left = 3
          Top = 21
          Width = 26
          Height = 13
          Caption = 'Font:'
        end
        object lblBorderColor: TLabel
          Left = 3
          Top = 80
          Width = 36
          Height = 13
          Caption = 'Border:'
        end
        object lblTransparencyFont: TLabel
          Left = 23
          Top = 44
          Width = 70
          Height = 13
          Caption = 'Transparency:'
        end
        object lblTransparencyBorder: TLabel
          Left = 23
          Top = 105
          Width = 70
          Height = 13
          Caption = 'Transparency:'
        end
        object lblShadowColor: TLabel
          Left = 3
          Top = 139
          Width = 42
          Height = 13
          Caption = 'Shadow:'
        end
        object lblTransparencyShadow: TLabel
          Left = 23
          Top = 161
          Width = 70
          Height = 13
          Caption = 'Transparency:'
        end
        object ctbFont: TColorToolBar
          Left = 55
          Top = 11
          Width = 61
          Height = 23
          OnDropdown = ctbFontDropdown
          OnColorChanged = ctbFontColorChanged
          SelectedColor = 0
        end
        object ctbBorder: TColorToolBar
          Left = 55
          Top = 70
          Width = 61
          Height = 23
          OnDropdown = ctbBorderDropdown
          OnColorChanged = ctbBorderColorChanged
          SelectedColor = 0
        end
        object tbFont: TTrackBar
          Left = 102
          Top = 40
          Width = 136
          Height = 24
          Max = 255
          ShowSelRange = False
          TabOrder = 1
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbFontChange
        end
        object tbBorder: TTrackBar
          Left = 102
          Top = 99
          Width = 136
          Height = 24
          Max = 255
          ShowSelRange = False
          TabOrder = 3
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbBorderChange
        end
        object ctbShadow: TColorToolBar
          Left = 55
          Top = 129
          Width = 61
          Height = 23
          OnDropdown = ctbShadowDropdown
          OnColorChanged = ctbShadowColorChanged
          SelectedColor = 0
        end
        object tbShadow: TTrackBar
          Left = 102
          Top = 158
          Width = 136
          Height = 24
          Max = 255
          ShowSelRange = False
          TabOrder = 5
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbShadowChange
        end
      end
      object TabSheet7: TTabSheet
        Caption = 'Entrance'
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblSpeedEnter: TLabel
          Left = 8
          Top = 47
          Width = 34
          Height = 13
          Caption = 'Speed:'
        end
        object lblEffectEnter: TLabel
          Left = 8
          Top = 20
          Width = 33
          Height = 13
          Caption = 'Effect:'
        end
        object cmbEffectEnter: TComboBox
          Left = 50
          Top = 17
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cmbEffectEnterChange
          Items.Strings = (
            'None'
            'Fade In'
            'Slide')
        end
        object cmbSpeedEnter: TComboBox
          Left = 50
          Top = 44
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cmbSpeedEnterChange
          Items.Strings = (
            'Fast'
            'Normal'
            'Slow')
        end
        object cmbDirectEnter: TComboBox
          Left = 136
          Top = 17
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cmbDirectEnterChange
          Items.Strings = (
            'Left'
            'Top'
            'Right'
            'Bottom'
            'Left Top'
            'Right Top'
            'Left Bottom'
            'Right Bottom')
        end
        object ckbEnterOnce: TCheckBox
          Left = 50
          Top = 80
          Width = 97
          Height = 17
          Caption = 'At Once'
          TabOrder = 3
          OnClick = ckbEnterOnceClick
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Exit'
        ImageIndex = 3
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblEffectExit: TLabel
          Left = 8
          Top = 20
          Width = 33
          Height = 13
          Caption = 'Effect:'
        end
        object lblSpeedExit: TLabel
          Left = 8
          Top = 47
          Width = 34
          Height = 13
          Caption = 'Speed:'
        end
        object cmbEffectExit: TComboBox
          Left = 50
          Top = 17
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cmbEffectExitChange
          Items.Strings = (
            'None'
            'Fade Out'
            'Slide')
        end
        object cmbSpeedExit: TComboBox
          Left = 50
          Top = 44
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cmbSpeedExitChange
          Items.Strings = (
            'Fast'
            'Normal'
            'Slow')
        end
        object cmbDirectExit: TComboBox
          Left = 136
          Top = 17
          Width = 80
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cmbDirectExitChange
          Items.Strings = (
            'Left'
            'Top'
            'Right'
            'Bottom'
            'Left Top'
            'Right Top'
            'Left Bottom'
            'Right Bottom')
        end
        object ckbExitOnce: TCheckBox
          Left = 50
          Top = 80
          Width = 97
          Height = 17
          Caption = 'At Once'
          TabOrder = 3
          OnClick = ckbExitOnceClick
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'Position'
        ImageIndex = 4
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblAlignment: TLabel
          Left = 3
          Top = 20
          Width = 51
          Height = 13
          Caption = 'Alignment:'
        end
        object cmbAlignment: TComboBox
          Left = 60
          Top = 17
          Width = 120
          Height = 21
          BevelInner = bvNone
          BevelOuter = bvNone
          Style = csDropDownList
          TabOrder = 0
          OnChange = cmbAlignmentChange
          Items.Strings = (
            'Left'
            'Center'
            'Right')
        end
      end
    end
    object btnEditLine: TGraphicsButton
      Left = 12
      Top = 111
      Width = 28
      Height = 28
      ImageIndex = 0
      TabOrder = 2
      OnClick = btnEditLineClick
    end
    object btnDeleteLine: TGraphicsButton
      Left = 221
      Top = 111
      Width = 28
      Height = 28
      ImageIndex = 0
      TabOrder = 3
      OnClick = btnDeleteLineClick
    end
    object btnAddLine: TGraphicsButton
      Left = 46
      Top = 111
      Width = 28
      Height = 28
      ImageIndex = 0
      TabOrder = 4
      OnClick = btnAddLineClick
    end
  end
  object palRender1: TPanel
    Left = 0
    Top = 0
    Width = 642
    Height = 482
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object palRender: TPanel
      Left = 0
      Top = 0
      Width = 642
      Height = 402
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      ExplicitTop = -4
    end
    object Panel3: TPanel
      Left = 0
      Top = 402
      Width = 642
      Height = 80
      Align = alBottom
      BevelEdges = [beTop]
      BevelKind = bkFlat
      BevelOuter = bvNone
      TabOrder = 1
      object btnPlay: TGraphicsSpeedButton
        Left = 282
        Top = 44
        Width = 30
        Height = 30
        Enabled = False
        OnClick = btnPlayClick
      end
      object PageControl1: TPageControl
        Left = 0
        Top = 0
        Width = 247
        Height = 78
        ActivePage = TabSheet1
        Align = alLeft
        TabOrder = 0
        object TabSheet1: TTabSheet
          Caption = 'Start Time'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object TimeEditStart: TTimeEdit
            Left = 16
            Top = 8
            Width = 104
            Height = 28
            OnChange = TimeEditStartChange
            MaxTime = 86400000000
          end
        end
        object TabSheet8: TTabSheet
          Caption = 'Duration'
          ImageIndex = 2
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object TimeEditDuration: TTimeEdit
            Left = 16
            Top = 8
            Width = 104
            Height = 28
            OnChange = TimeEditDurationChange
            MaxTime = 86400000000
          end
        end
        object TabSheet4: TTabSheet
          Caption = 'Horizontal'
          ImageIndex = 3
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object lblHorizontal: TLabel
            Left = 8
            Top = 13
            Width = 41
            Height = 13
            Caption = 'Position:'
          end
          object cmbHorizontal: TComboBox
            Left = 72
            Top = 10
            Width = 120
            Height = 21
            Style = csDropDownList
            TabOrder = 0
            OnChange = cmbHorizontalChange
            Items.Strings = (
              'Left'
              'Center'
              'Right')
          end
        end
        object TabSheet9: TTabSheet
          Caption = 'Vertical'
          ImageIndex = 3
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object lblVertical: TLabel
            Left = 8
            Top = 13
            Width = 52
            Height = 13
            Caption = 'Horizontal:'
          end
          object cmbVertical: TComboBox
            Left = 72
            Top = 10
            Width = 120
            Height = 21
            Style = csDropDownList
            TabOrder = 0
            OnChange = cmbVerticalChange
            Items.Strings = (
              'Top'
              'Center'
              'Bottom')
          end
        end
      end
    end
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Options = [fdEffects, fdScalableOnly]
    Left = 104
    Top = 160
  end
end
