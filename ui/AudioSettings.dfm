object frmAudioSettings: TfrmAudioSettings
  Left = 0
  Top = 0
  Caption = 'frmAudioSettings'
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
  object palConsole: TPanel
    Left = 0
    Top = 408
    Width = 784
    Height = 154
    Align = alBottom
    BevelEdges = [beTop]
    BevelOuter = bvLowered
    TabOrder = 0
    object btnPlay: TGraphicsSpeedButton
      Left = 7
      Top = 6
      Width = 33
      Height = 32
      OnClick = btnPlayClick
    end
    object lblTime: TLabel
      Left = 632
      Top = 45
      Width = 139
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = '0:00:00.000/0:00:00.000'
    end
    object tbSeek: TMediaPlayTrackBar
      Left = 46
      Top = 1
      Width = 733
      Height = 37
      OnChange = tbSeekChange
      ShowSelRange = False
      TabOrder = 0
    end
    object PageControl1: TPageControl
      Left = 1
      Top = 56
      Width = 782
      Height = 97
      ActivePage = TabSheet1
      Align = alBottom
      MultiLine = True
      TabOrder = 1
      object TabSheet1: TTabSheet
        Caption = 'Audio Stream'
        object lblVolumeStream: TLabel
          Left = 16
          Top = 24
          Width = 38
          Height = 13
          Caption = 'Volume:'
        end
        object tbVolumeStream: TTrackBar
          Left = 60
          Top = 20
          Width = 150
          Height = 29
          Max = 100
          PositionToolTip = ptRight
          ShowSelRange = False
          TabOrder = 0
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbVolumeStreamChange
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Background Music'
        ImageIndex = 1
        object Label1: TLabel
          Left = 5
          Top = 16
          Width = 30
          Height = 13
          Caption = 'Music:'
        end
        object Label2: TLabel
          Left = 5
          Top = 42
          Width = 38
          Height = 13
          Caption = 'Volume:'
        end
        object edMusic: TEdit
          Left = 49
          Top = 11
          Width = 184
          Height = 21
          ReadOnly = True
          TabOrder = 0
          Text = 'edMusic'
        end
        object tbVolumeMusic: TTrackBar
          Left = 49
          Top = 38
          Width = 150
          Height = 29
          Max = 100
          PositionToolTip = ptRight
          ShowSelRange = False
          TabOrder = 1
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbVolumeMusicChange
        end
        object btnImport: TButton
          Left = 239
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Import'
          TabOrder = 2
        end
        object btnRemove: TButton
          Left = 320
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Remove'
          TabOrder = 3
        end
        object btnClip: TButton
          Left = 239
          Top = 42
          Width = 75
          Height = 25
          Caption = 'Clip'
          TabOrder = 4
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'Recording'
        ImageIndex = 2
        object Label3: TLabel
          Left = 5
          Top = 42
          Width = 38
          Height = 13
          Caption = 'Volume:'
        end
        object lblRecord: TLabel
          Left = 5
          Top = 17
          Width = 44
          Height = 13
          Caption = 'lblRecord'
        end
        object tbVolumeRecord: TTrackBar
          Left = 49
          Top = 38
          Width = 150
          Height = 29
          Max = 100
          ParentShowHint = False
          PositionToolTip = ptRight
          ShowHint = True
          ShowSelRange = False
          TabOrder = 0
          TickMarks = tmBoth
          TickStyle = tsNone
          OnChange = tbVolumeRecordChange
        end
        object Button1: TButton
          Left = 288
          Top = 33
          Width = 75
          Height = 25
          Caption = 'Recording'
          TabOrder = 1
          OnClick = Button1Click
        end
        object btnRemoveRecord: TButton
          Left = 369
          Top = 33
          Width = 75
          Height = 25
          Caption = 'Remove'
          TabOrder = 2
        end
      end
    end
  end
  object palRender: TPanel
    Left = 0
    Top = 0
    Width = 784
    Height = 408
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    OnResize = palRenderResize
  end
end
