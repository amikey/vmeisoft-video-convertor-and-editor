object frmCodecSettings: TfrmCodecSettings
  Left = 8
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Options'
  ClientHeight = 489
  ClientWidth = 657
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
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 636
    Height = 425
    ActivePage = TabSheet2
    TabOrder = 0
    object TabSheet2: TTabSheet
      Caption = 'Export Video Settings'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 169
        Height = 397
        Align = alLeft
        BevelOuter = bvNone
        BorderWidth = 4
        TabOrder = 0
        object sbHD: TGraphicsGroupRadio
          Left = 4
          Top = 4
          Width = 161
          Height = 60
          Hint = 'HD Video File'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          Down = True
          OnClick = SpeedButtonClick
          ImageLeft = 16
        end
        object sbVideo: TGraphicsGroupRadio
          Tag = 1
          Left = 4
          Top = 64
          Width = 161
          Height = 68
          Hint = 'Video File'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          OnClick = SpeedButtonClick
          ImageLeft = 16
          ExplicitTop = 68
        end
        object sbDevice: TGraphicsGroupRadio
          Tag = 2
          Left = 4
          Top = 132
          Width = 161
          Height = 68
          Hint = 'Device'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          OnClick = SpeedButtonClick
          ImageLeft = 16
        end
        object sbWeb: TGraphicsGroupRadio
          Tag = 3
          Left = 4
          Top = 200
          Width = 161
          Height = 68
          Hint = 'Web'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          OnClick = SpeedButtonClick
          ImageLeft = 16
          ExplicitTop = 196
        end
        object sbApp: TGraphicsGroupRadio
          Tag = 4
          Left = 4
          Top = 268
          Width = 161
          Height = 68
          Hint = 'Application'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          OnClick = SpeedButtonClick
          ImageLeft = 16
          ExplicitTop = 260
        end
        object sbAudio: TGraphicsGroupRadio
          Tag = 5
          Left = 4
          Top = 336
          Width = 161
          Height = 60
          Hint = 'Audio'
          Align = alTop
          AllowAllUp = True
          GroupIndex = 1
          OnClick = SpeedButtonClick
          ImageLeft = 16
          ExplicitTop = 344
        end
      end
      object Panel2: TPanel
        Left = 169
        Top = 0
        Width = 459
        Height = 397
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 1
        object lbFormat: TListBox
          Left = 0
          Top = 0
          Width = 459
          Height = 320
          Style = lbOwnerDrawFixed
          Align = alClient
          ItemHeight = 48
          TabOrder = 0
          OnClick = lbFormatClick
          OnDrawItem = lbFormatDrawItem
        end
        object memoDec: TMemo
          Left = 0
          Top = 320
          Width = 459
          Height = 77
          Align = alBottom
          ReadOnly = True
          TabOrder = 1
          WantTabs = True
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Advanced Video Settings'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lblFilter: TLabel
        Left = 18
        Top = 20
        Width = 36
        Height = 13
        Alignment = taRightJustify
        Caption = 'Target:'
      end
      object gbVideo: TGroupBox
        Left = 17
        Top = 44
        Width = 298
        Height = 317
        Caption = 'Video Settings'
        TabOrder = 0
        object lblVCodec: TLabel
          Left = 8
          Top = 32
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Codec:'
        end
        object lblResolution: TLabel
          Left = 8
          Top = 65
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Resolution:'
        end
        object lblWidth: TLabel
          Left = 19
          Top = 95
          Width = 49
          Height = 13
          Alignment = taRightJustify
          AutoSize = False
          Caption = 'Width:'
        end
        object lblHeight: TLabel
          Left = 140
          Top = 95
          Width = 35
          Height = 13
          AutoSize = False
          Caption = 'Height:'
        end
        object lblFrameRate: TLabel
          Left = 8
          Top = 139
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Frame Rate:'
        end
        object lblVBitrate: TLabel
          Left = 8
          Top = 208
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Bit Rate:'
        end
        object lblHintVBitrate: TLabel
          Left = 84
          Top = 232
          Width = 200
          Height = 26
          AutoSize = False
          Caption = 
            'The higher the bit rate the better the quality, but the larger t' +
            'he file.'
          WordWrap = True
        end
        object lblHitFrameRate: TLabel
          Left = 84
          Top = 163
          Width = 200
          Height = 26
          AutoSize = False
          Caption = 
            'Frame rate the higher the animation is smooth, but the larger fi' +
            'le.'
          WordWrap = True
        end
        object lblAspect: TLinkLabel
          Left = 242
          Top = 95
          Width = 56
          Height = 17
          Caption = '1920:1080'
          TabOrder = 6
        end
        object cbVCodec: TComboBox
          Left = 84
          Top = 29
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cbVCodecChange
        end
        object cbResolution: TComboBox
          Left = 84
          Top = 65
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbResolutionChange
        end
        object cbFrameRate: TComboBox
          Left = 84
          Top = 136
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 4
          OnChange = cbFrameRateChange
        end
        object cbVBitrate: TComboBox
          Left = 84
          Top = 205
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 5
          OnChange = cbVBitrateChange
        end
        object spWidth: TSpinEdit
          Left = 74
          Top = 92
          Width = 55
          Height = 22
          Increment = 16
          MaxValue = 1920
          MinValue = 160
          TabOrder = 2
          Value = 1920
          OnChange = spWidthChange
        end
        object spHeight: TSpinEdit
          Left = 181
          Top = 92
          Width = 55
          Height = 22
          Increment = 16
          MaxValue = 1088
          MinValue = 80
          TabOrder = 3
          Value = 1080
          OnChange = spHeightChange
        end
      end
      object gbAudio: TGroupBox
        Left = 321
        Top = 44
        Width = 289
        Height = 317
        Caption = 'Audio Settings'
        TabOrder = 1
        object lblACodec: TLabel
          Left = 8
          Top = 32
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Codec:'
        end
        object lblChannels: TLabel
          Left = 8
          Top = 136
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Channels:'
        end
        object lblABitRate: TLabel
          Left = 8
          Top = 174
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Bit Rate:'
        end
        object lblHintABitrate: TLabel
          Left = 84
          Top = 198
          Width = 200
          Height = 26
          AutoSize = False
          Caption = 
            'The higher the bit rate the better the quality, but the larger t' +
            'he file.'
          WordWrap = True
        end
        object lblSampleRate: TLabel
          Left = 8
          Top = 65
          Width = 70
          Height = 13
          AutoSize = False
          Caption = 'Sample Rate:'
        end
        object lblHintSampleRate: TLabel
          Left = 84
          Top = 89
          Width = 200
          Height = 26
          AutoSize = False
          Caption = 
            'The higher the Sample rate the better the quality, but the large' +
            'r the file.'
          WordWrap = True
        end
        object cbACodec: TComboBox
          Left = 84
          Top = 29
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cbACodecChange
        end
        object cbChannels: TComboBox
          Left = 84
          Top = 133
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cbChannelsChange
        end
        object cbABitrate: TComboBox
          Left = 84
          Top = 171
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 3
          OnChange = cbABitrateChange
        end
        object cbSampleRate: TComboBox
          Left = 84
          Top = 62
          Width = 180
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbSampleRateChange
        end
      end
      object cbTarget: TComboBox
        Left = 70
        Top = 17
        Width = 252
        Height = 21
        Style = csDropDownList
        DropDownCount = 32
        TabOrder = 2
        OnChange = cbTargetChange
      end
    end
  end
  object btnCancel: TButton
    Left = 565
    Top = 455
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&OK'
    Default = True
    ModalResult = 2
    TabOrder = 1
  end
  object PopupMenu3: TPopupMenu
    Left = 24
    Top = 440
    object ClipAspect4x3: TMenuItem
      Caption = '4:3'
      OnClick = ClipAspect4x3Click
    end
    object ClipAspect16x9: TMenuItem
      Caption = '16:9'
      OnClick = ClipAspect16x9Click
    end
  end
  object ImageList1: TImageList
    Height = 48
    Width = 48
    Left = 88
    Top = 440
  end
end
