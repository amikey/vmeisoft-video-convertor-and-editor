object frmSubtitleSettings: TfrmSubtitleSettings
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Subtitle Default Settings ...'
  ClientHeight = 250
  ClientWidth = 427
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 10
    Top = 7
    Width = 409
    Height = 202
    ActivePage = TabSheet2
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Postion'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 165
      object lblHorizontal: TLabel
        Left = 8
        Top = 20
        Width = 52
        Height = 13
        Caption = 'Horizontal:'
      end
      object lblVertical: TLabel
        Left = 8
        Top = 50
        Width = 52
        Height = 13
        Caption = 'Horizontal:'
      end
      object cmbHorizontal: TComboBox
        Left = 72
        Top = 17
        Width = 120
        Height = 21
        Style = csDropDownList
        TabOrder = 0
        Items.Strings = (
          'Left'
          'Center'
          'Right')
      end
      object cmbVertical: TComboBox
        Left = 72
        Top = 47
        Width = 120
        Height = 21
        Style = csDropDownList
        TabOrder = 1
        Items.Strings = (
          'Top'
          'Center'
          'Bottom')
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Text'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 28
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lblFont: TLabel
        Left = 3
        Top = 16
        Width = 26
        Height = 13
        Caption = 'Font:'
      end
      object lblFontColor: TLabel
        Left = 3
        Top = 45
        Width = 29
        Height = 13
        Caption = 'Color:'
      end
      object lblBorderColor: TLabel
        Left = 3
        Top = 73
        Width = 36
        Height = 13
        Caption = 'Border:'
      end
      object Label1: TLabel
        Left = 136
        Top = 45
        Width = 70
        Height = 13
        Caption = 'Transparency:'
      end
      object Label2: TLabel
        Left = 136
        Top = 73
        Width = 70
        Height = 13
        Caption = 'Transparency:'
      end
      object Label3: TLabel
        Left = 3
        Top = 102
        Width = 42
        Height = 13
        Caption = 'Shadow:'
      end
      object Label4: TLabel
        Left = 136
        Top = 102
        Width = 70
        Height = 13
        Caption = 'Transparency:'
      end
      object lblAlignment: TLabel
        Left = 3
        Top = 137
        Width = 51
        Height = 13
        Caption = 'Alignment:'
      end
      object btnFont: TButton
        Left = 238
        Top = 11
        Width = 81
        Height = 25
        Caption = 'Change'
        TabOrder = 0
        OnClick = btnFontClick
      end
      object tbFont: TTrackBar
        Left = 212
        Top = 38
        Width = 180
        Height = 25
        Max = 255
        ShowSelRange = False
        TabOrder = 1
        TickMarks = tmBoth
        TickStyle = tsNone
        OnChange = tbFontChange
      end
      object tbBorder: TTrackBar
        Left = 212
        Top = 67
        Width = 180
        Height = 25
        Max = 255
        ShowSelRange = False
        TabOrder = 2
        TickMarks = tmBoth
        TickStyle = tsNone
        OnChange = tbBorderChange
      end
      object ctbFont: TColorToolBar
        Left = 60
        Top = 38
        Width = 61
        Height = 23
        OnDropdown = ctbFontDropdown
        OnColorChanged = ctbFontColorChanged
        SelectedColor = 0
      end
      object ctbBorder: TColorToolBar
        Left = 60
        Top = 67
        Width = 61
        Height = 23
        OnDropdown = ctbBorderDropdown
        OnColorChanged = ctbBorderColorChanged
        SelectedColor = 0
      end
      object ctbShadow: TColorToolBar
        Left = 60
        Top = 96
        Width = 61
        Height = 23
        OnDropdown = ctbShadowDropdown
        OnColorChanged = ctbShadowColorChanged
        SelectedColor = 0
      end
      object tbShadow: TTrackBar
        Left = 212
        Top = 96
        Width = 180
        Height = 25
        Max = 255
        ShowSelRange = False
        TabOrder = 6
        TickMarks = tmBoth
        TickStyle = tsNone
        OnChange = tbShadowChange
      end
      object cmbAlignment: TComboBox
        Left = 60
        Top = 134
        Width = 120
        Height = 21
        Style = csDropDownList
        TabOrder = 7
        OnChange = cmbAlignmentChange
        Items.Strings = (
          'Left'
          'Center'
          'Right')
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Entrance'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 165
      object lblEffectEnter: TLabel
        Left = 8
        Top = 20
        Width = 33
        Height = 13
        Caption = 'Effect:'
      end
      object lblSpeedEnter: TLabel
        Left = 8
        Top = 47
        Width = 34
        Height = 13
        Caption = 'Speed:'
      end
      object cmbEffectEnter: TComboBox
        Left = 60
        Top = 17
        Width = 100
        Height = 21
        Style = csDropDownList
        TabOrder = 0
        OnChange = cmbEffectEnterChange
        Items.Strings = (
          'None'
          'Fade In'
          'Slide')
      end
      object cmbDirectEnter: TComboBox
        Left = 166
        Top = 17
        Width = 100
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
      object cmbSpeedEnter: TComboBox
        Left = 60
        Top = 44
        Width = 100
        Height = 21
        Style = csDropDownList
        TabOrder = 2
        OnChange = cmbSpeedEnterChange
        Items.Strings = (
          'Fast'
          'Normal'
          'Slow')
      end
      object ckbEnterOnce: TCheckBox
        Left = 60
        Top = 87
        Width = 97
        Height = 17
        Caption = 'At Once'
        TabOrder = 3
        OnClick = ckbEnterOnceClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Exit'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 165
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
        Left = 60
        Top = 17
        Width = 100
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
        Left = 60
        Top = 44
        Width = 100
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
        Left = 166
        Top = 17
        Width = 100
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
        Left = 60
        Top = 87
        Width = 97
        Height = 17
        Caption = 'At Once'
        TabOrder = 3
        OnClick = ckbExitOnceClick
      end
    end
  end
  object Button1: TButton
    Left = 344
    Top = 215
    Width = 75
    Height = 25
    Caption = 'Apply to all'
    TabOrder = 1
    OnClick = Button1Click
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Options = [fdEffects, fdScalableOnly]
    Left = 16
    Top = 208
  end
end
