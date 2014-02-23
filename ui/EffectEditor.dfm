object frmEffect: TfrmEffect
  Left = 0
  Top = 0
  Caption = 'frmEffect'
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
    Height = 343
    Align = alClient
    TabOrder = 0
    OnResize = palRenderResize
  end
  object palConsole: TPanel
    Left = 0
    Top = 343
    Width = 784
    Height = 200
    Align = alBottom
    BevelOuter = bvNone
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
      Left = 0
      Top = 56
      Width = 784
      Height = 144
      ActivePage = TabSheet2
      Align = alBottom
      MultiLine = True
      TabOrder = 1
      object TabSheet1: TTabSheet
        Caption = 'Brightness  Contrast'
        ExplicitLeft = 7
        ExplicitTop = 22
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblBrightness: TLabel
          Left = 19
          Top = 20
          Width = 54
          Height = 13
          Caption = 'Brightness:'
        end
        object lblContrast: TLabel
          Left = 19
          Top = 48
          Width = 46
          Height = 13
          Caption = 'Contrast:'
        end
        object tbBrightness: TTrackBar
          Left = 96
          Top = 16
          Width = 408
          Height = 24
          Max = 200
          Min = -200
          ShowSelRange = False
          TabOrder = 0
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbBrightnessChange
        end
        object tbContrast: TTrackBar
          Left = 96
          Top = 44
          Width = 408
          Height = 24
          Max = 100
          Min = -100
          ShowSelRange = False
          TabOrder = 1
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbContrastChange
        end
        object btnReset1: TButton
          Left = 540
          Top = 27
          Width = 75
          Height = 25
          Caption = 'Reset'
          TabOrder = 2
          OnClick = btnReset1Click
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Hue Saturation Lightness'
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblHue: TLabel
          Left = 19
          Top = 20
          Width = 23
          Height = 13
          Caption = 'Hue:'
        end
        object lblSaturation: TLabel
          Left = 19
          Top = 48
          Width = 54
          Height = 13
          Caption = 'Saturation:'
        end
        object lblLightness: TLabel
          Left = 19
          Top = 76
          Width = 49
          Height = 13
          Caption = 'Lightness:'
        end
        object tbHue: TTrackBar
          Left = 96
          Top = 14
          Width = 408
          Height = 24
          Max = 180
          Min = -180
          ShowSelRange = False
          TabOrder = 0
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbHueChange
        end
        object tbSaturation: TTrackBar
          Left = 96
          Top = 44
          Width = 408
          Height = 24
          Max = 100
          Min = -100
          ShowSelRange = False
          TabOrder = 1
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbSaturationChange
        end
        object tbLightness: TTrackBar
          Left = 96
          Top = 72
          Width = 408
          Height = 24
          Max = 100
          Min = -100
          ShowSelRange = False
          TabOrder = 2
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbLightnessChange
        end
        object btnReset2: TButton
          Left = 540
          Top = 43
          Width = 75
          Height = 25
          Caption = 'Reset'
          TabOrder = 3
          OnClick = btnReset2Click
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'Rotate Flip'
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object lblRotateFlip: TLabel
          Left = 50
          Top = 80
          Width = 59
          Height = 13
          Caption = 'lblRotateFlip'
        end
        object btnRotateLeft: TGraphicsButton
          Left = 50
          Top = 24
          Width = 25
          Height = 25
          Hint = 'Rotate Left'
          ImageIndex = 0
          TabOrder = 0
          OnClick = btnRotateLeftClick
        end
        object btnRotateRight: TGraphicsButton
          Left = 81
          Top = 24
          Width = 25
          Height = 25
          Hint = 'Rotate Right'
          ImageIndex = 0
          TabOrder = 1
          OnClick = btnRotateRightClick
        end
        object btnFlipHorizontal: TGraphicsButton
          Left = 150
          Top = 24
          Width = 25
          Height = 25
          Hint = 'Flip Horizontal'
          ImageIndex = 0
          TabOrder = 2
          OnClick = btnFlipHorizontalClick
        end
        object btnFlipVertical: TGraphicsButton
          Left = 181
          Top = 24
          Width = 25
          Height = 25
          Hint = 'Flip Vertical'
          ImageIndex = 0
          TabOrder = 3
          OnClick = btnFlipVerticalClick
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
        Width = 80
      end
      item
        Width = 120
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Duration'
        Width = 80
      end
      item
        Width = 120
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Trim Start'
        Width = 80
      end
      item
        Width = 120
      end
      item
        Alignment = taRightJustify
        Bevel = pbRaised
        Text = 'Trim Stop'
        Width = 80
      end
      item
        Width = 120
      end>
  end
end
