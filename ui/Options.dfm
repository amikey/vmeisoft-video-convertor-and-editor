object frmOptions: TfrmOptions
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Options ...'
  ClientHeight = 211
  ClientWidth = 449
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 433
    Height = 185
    ActivePage = TabSheet2
    TabOrder = 0
    OnChange = PageControl1Change
    object TabSheet1: TTabSheet
      Caption = 'General'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label1: TLabel
        Left = 17
        Top = 29
        Width = 83
        Height = 13
        Caption = 'Publish to Folder:'
      end
      object btnBrowse: TButton
        Left = 359
        Top = 24
        Width = 34
        Height = 21
        Caption = '...'
        TabOrder = 1
        OnClick = btnBrowseClick
      end
      object edFolder: TEdit
        Left = 104
        Top = 24
        Width = 249
        Height = 21
        ReadOnly = True
        TabOrder = 0
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Business'
      ImageIndex = 2
      object GroupBox2: TGroupBox
        Left = 3
        Top = 3
        Width = 414
        Height = 145
        Caption = 'Default watermark add to output video'
        TabOrder = 0
        object Label5: TLabel
          Left = 199
          Top = 111
          Width = 92
          Height = 13
          Caption = 'Horizontal Position:'
        end
        object Label8: TLabel
          Left = 16
          Top = 111
          Width = 92
          Height = 13
          Caption = 'Horizontal Position:'
        end
        object lblTransparency: TLabel
          Left = 16
          Top = 81
          Width = 70
          Height = 13
          Caption = 'Transparency:'
        end
        object Label6: TLabel
          Left = 16
          Top = 27
          Width = 56
          Height = 13
          Caption = 'Picture File:'
        end
        object lalWaterWidth: TLabel
          Left = 16
          Top = 54
          Width = 32
          Height = 13
          Caption = 'Width:'
        end
        object Label3: TLabel
          Left = 199
          Top = 56
          Width = 35
          Height = 13
          Caption = 'Height:'
        end
        object btnWaterDelete: TGraphicsButton
          Left = 365
          Top = 22
          Width = 25
          Height = 25
          ImageIndex = 0
          TabOrder = 2
          OnClick = btnWaterDeleteClick
        end
        object btnWaterAdd: TGraphicsButton
          Left = 334
          Top = 22
          Width = 25
          Height = 25
          ImageIndex = 0
          TabOrder = 1
          OnClick = btnWaterAddClick
        end
        object cmbWaterHPosition: TComboBox
          Left = 114
          Top = 108
          Width = 70
          Height = 21
          Style = csDropDownList
          TabOrder = 3
          OnChange = cmbWaterHPositionChange
          Items.Strings = (
            'Left'
            'Center'
            'Right')
        end
        object cmbWaterVPosition: TComboBox
          Left = 297
          Top = 108
          Width = 70
          Height = 21
          Style = csDropDownList
          TabOrder = 4
          OnChange = cmbWaterVPositionChange
          Items.Strings = (
            'Top'
            'Center'
            'Bottom')
        end
        object edWatermark: TEdit
          Left = 86
          Top = 24
          Width = 242
          Height = 21
          ReadOnly = True
          TabOrder = 0
        end
        object tbTransparency: TTrackBar
          Left = 92
          Top = 75
          Width = 218
          Height = 21
          Max = 240
          ShowSelRange = False
          TabOrder = 5
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbTransparencyChange
        end
        object edWaterHeight: TEdit
          Left = 251
          Top = 51
          Width = 40
          Height = 21
          TabOrder = 6
          Text = '0'
        end
        object edWaterWidth: TEdit
          Left = 86
          Top = 51
          Width = 40
          Height = 21
          TabOrder = 7
          Text = '0'
        end
        object udWaterHeight: TUpDown
          Left = 291
          Top = 51
          Width = 16
          Height = 21
          Associate = edWaterHeight
          TabOrder = 8
          OnClick = udWaterHeightClick
        end
        object udWaterWidth: TUpDown
          Left = 126
          Top = 51
          Width = 16
          Height = 21
          Associate = edWaterWidth
          TabOrder = 9
          OnClick = udWaterWidthClick
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Resize Modth'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object btnPreResize: TGraphicsSpeedButton
        Left = 16
        Top = 224
        Width = 30
        Height = 30
        OnClick = btnPreResizeClick
      end
      object btnNextResize: TGraphicsSpeedButton
        Left = 52
        Top = 224
        Width = 30
        Height = 30
        OnClick = btnNextResizeClick
      end
      object rgResizeModth: TRadioGroup
        Left = 3
        Top = 16
        Width = 110
        Height = 129
        Items.Strings = (
          'Letter Box'
          'Crop'
          'Center'
          'Fill')
        TabOrder = 0
        OnClick = rgResizeModthClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Background'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object pcBackground: TPageControl
        Left = 124
        Top = 6
        Width = 110
        Height = 123
        ActivePage = TabSheet7
        TabOrder = 0
        object TabSheet5: TTabSheet
          Caption = 'Solid'
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object ctbColor: TColorToolBar
            Left = 8
            Top = 8
            Width = 61
            Height = 23
            OnDropdown = ctbColorDropdown
            OnColorChanged = ctbColorColorChanged
            SelectedColor = 0
          end
        end
        object TabSheet6: TTabSheet
          Caption = 'Gradient'
          ImageIndex = 1
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object cmbFillDirect: TComboBox
            Left = 8
            Top = 66
            Width = 86
            Height = 21
            Style = csDropDownList
            TabOrder = 0
            OnChange = cmbFillDirectChange
            Items.Strings = (
              'Top'
              'Left'
              'TopLeft'
              'TopRight')
          end
          object ctbColor1: TColorToolBar
            Left = 8
            Top = 8
            Width = 61
            Height = 23
            OnDropdown = ctbColor1Dropdown
            OnColorChanged = ctbColor1ColorChanged
            SelectedColor = 0
          end
          object ctbColor2: TColorToolBar
            Left = 8
            Top = 37
            Width = 61
            Height = 23
            OnDropdown = ctbColor2Dropdown
            OnColorChanged = ctbColor2ColorChanged
            SelectedColor = 0
          end
        end
        object TabSheet7: TTabSheet
          Caption = 'Picture'
          ImageIndex = 2
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnBrowser: TButton
            Left = 8
            Top = 15
            Width = 75
            Height = 25
            Caption = '...'
            TabOrder = 0
            OnClick = btnBrowserClick
          end
        end
      end
      object gbType: TGroupBox
        Left = 3
        Top = 3
        Width = 110
        Height = 126
        Caption = 'Type'
        TabOrder = 1
        object btnSolid: TRadioButton
          Left = 12
          Top = 32
          Width = 80
          Height = 17
          Caption = 'Solid'
          Checked = True
          TabOrder = 0
          TabStop = True
          OnClick = OnFillTypeClick
        end
        object btnGradient: TRadioButton
          Tag = 1
          Left = 12
          Top = 55
          Width = 80
          Height = 17
          Caption = 'Gradient'
          TabOrder = 1
          OnClick = OnFillTypeClick
        end
        object btnPicture: TRadioButton
          Tag = 2
          Left = 12
          Top = 78
          Width = 80
          Height = 17
          Caption = 'Picture'
          TabOrder = 2
          OnClick = OnFillTypeClick
        end
      end
    end
    object TabSheet8: TTabSheet
      Caption = 'Performance'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lblParallel: TLabel
        Left = 16
        Top = 32
        Width = 146
        Height = 13
        Caption = 'Number of parallel conversion:'
      end
      object cmbParallel: TComboBox
        Left = 176
        Top = 29
        Width = 73
        Height = 21
        Style = csDropDownList
        TabOrder = 0
        OnChange = cmbParallelChange
      end
    end
  end
  object ImageList1: TImageList
    Height = 30
    Width = 30
    Left = 64
    Top = 184
  end
  object OpenDialog1: TOpenDialog
    Options = [ofFileMustExist, ofEnableSizing]
    Left = 16
    Top = 184
  end
end
