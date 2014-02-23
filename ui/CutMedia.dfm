object frmCut: TfrmCut
  Left = 0
  Top = 0
  Caption = 'frmCut'
  ClientHeight = 562
  ClientWidth = 784
  Color = clBtnFace
  Constraints.MinHeight = 600
  Constraints.MinWidth = 800
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object palRender: TPanel
    Left = 0
    Top = 0
    Width = 784
    Height = 416
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    OnResize = palRenderResize
  end
  object palConsole: TPanel
    Left = 0
    Top = 416
    Width = 784
    Height = 127
    Align = alBottom
    BevelEdges = [beTop]
    BevelOuter = bvLowered
    TabOrder = 1
    object btnPlay: TGraphicsSpeedButton
      Left = 7
      Top = 6
      Width = 33
      Height = 32
      OnClick = btnPlayClick
    end
    object tbSeek: TMediaPlayTrackBar
      Left = 46
      Top = 1
      Width = 737
      Height = 37
      OnChange = tbSeekChange
      TabOrder = 0
    end
    object PageControl1: TPageControl
      Left = 1
      Top = 43
      Width = 782
      Height = 83
      ActivePage = TabSheet1
      Align = alBottom
      MultiLine = True
      TabOrder = 1
      TabPosition = tpLeft
      OnChange = PageControl1Change
      object TabSheet1: TTabSheet
        Caption = 'Trim'
        object btnTrimStart: TGraphicsButton
          Left = 8
          Top = 14
          Width = 80
          Height = 25
          Caption = 'Cut the front'
          ImageIndex = 0
          TabOrder = 0
          OnClick = btnTrimStartClick
        end
        object btnTrimStop: TGraphicsButton
          Left = 94
          Top = 16
          Width = 80
          Height = 25
          Caption = 'Cut the behind'
          ImageIndex = 0
          TabOrder = 1
          OnClick = btnTrimStopClick
        end
        object btnNextFrame: TGraphicsButton
          Left = 372
          Top = 16
          Width = 80
          Height = 25
          Caption = 'Next Frame'
          ImageIndex = 0
          TabOrder = 2
          OnClick = btnNextFrameClick
        end
        object btnNextTime: TGraphicsButton
          Left = 458
          Top = 16
          Width = 80
          Height = 25
          Caption = 'Next Second'
          ImageIndex = 0
          TabOrder = 3
          OnClick = btnNextTimeClick
        end
        object btnPreTime: TGraphicsButton
          Left = 552
          Top = 16
          Width = 80
          Height = 25
          Caption = 'Back second'
          ImageIndex = 0
          TabOrder = 4
          OnClick = btnPreTimeClick
        end
        object btnTrimReset: TGraphicsButton
          Left = 53
          Top = 47
          Width = 75
          Height = 25
          Caption = 'Reset'
          ImageIndex = 0
          TabOrder = 5
          OnClick = btnTrimResetClick
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Crop'
        ImageIndex = 1
        object Label5: TLabel
          Left = 4
          Top = 17
          Width = 45
          Height = 13
          Alignment = taRightJustify
          AutoSize = False
          Caption = 'Left:'
        end
        object Label6: TLabel
          Left = 121
          Top = 17
          Width = 45
          Height = 13
          Alignment = taRightJustify
          AutoSize = False
          Caption = 'Top:'
        end
        object Label7: TLabel
          Left = 4
          Top = 44
          Width = 45
          Height = 13
          Alignment = taRightJustify
          AutoSize = False
          Caption = 'Width:'
        end
        object Label9: TLabel
          Left = 121
          Top = 44
          Width = 45
          Height = 13
          Alignment = taRightJustify
          AutoSize = False
          Caption = 'Height:'
        end
        object Label4: TLabel
          Left = 289
          Top = 49
          Width = 192
          Height = 13
          Caption = 'You can drag the box to crop the video.'
        end
        object lblClipAspect: TLinkLabel
          Left = 227
          Top = 45
          Width = 14
          Height = 17
          Caption = 'lbl'
          TabOrder = 0
          OnLinkClick = lblClipAspectLinkClick
        end
        object btnCropReset: TGraphicsButton
          Left = 289
          Top = 18
          Width = 75
          Height = 25
          Hint = 'Reset crop'
          Caption = 'Reset'
          ImageIndex = 0
          TabOrder = 1
          OnClick = btnCropResetClick
        end
        object spCropLeft: TSpinEdit
          Left = 51
          Top = 14
          Width = 50
          Height = 22
          MaxValue = 0
          MinValue = 0
          TabOrder = 2
          Value = 0
          OnChange = spCropLeftChange
        end
        object spCropTop: TSpinEdit
          Left = 171
          Top = 14
          Width = 50
          Height = 22
          MaxValue = 0
          MinValue = 0
          TabOrder = 3
          Value = 0
          OnChange = spCropTopChange
        end
        object spCropWidth: TSpinEdit
          Left = 51
          Top = 41
          Width = 50
          Height = 22
          MaxValue = 0
          MinValue = 0
          TabOrder = 4
          Value = 1920
          OnChange = spCropWidthChange
        end
        object spCropHeight: TSpinEdit
          Left = 171
          Top = 41
          Width = 50
          Height = 22
          MaxValue = 0
          MinValue = 0
          TabOrder = 5
          Value = 1080
          OnChange = spCropHeightChange
        end
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 543
    Width = 784
    Height = 19
    Panels = <
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Current'
        Width = 76
      end
      item
        Text = '00:00:00'
        Width = 76
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Duration'
        Width = 76
      end
      item
        Text = '00:00:00'
        Width = 76
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Trim Start'
        Width = 76
      end
      item
        Text = '00:00:00'
        Width = 76
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Trim Stop'
        Width = 76
      end
      item
        Text = '00:00:00'
        Width = 76
      end
      item
        Alignment = taRightJustify
        Text = 'Duration'
        Width = 76
      end
      item
        Text = '00:00:00'
        Width = 76
      end>
  end
  object PopupMenu3: TPopupMenu
    Left = 80
    Top = 80
    object ClipAspect4x3: TMenuItem
      Caption = '4:3'
    end
    object ClipAspect16x9: TMenuItem
      Caption = '16:9'
    end
  end
end
