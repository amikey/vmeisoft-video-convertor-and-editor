object frmMediaInfo: TfrmMediaInfo
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'File Informaion ...'
  ClientHeight = 211
  ClientWidth = 495
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object lblFile: TLabel
    Left = 8
    Top = 11
    Width = 20
    Height = 13
    Caption = 'File:'
  end
  object btnClose: TButton
    Left = 412
    Top = 178
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Close'
    Default = True
    ModalResult = 8
    TabOrder = 0
  end
  object edFile: TEdit
    Left = 56
    Top = 8
    Width = 425
    Height = 21
    ReadOnly = True
    TabOrder = 1
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 42
    Width = 479
    Height = 121
    ActivePage = Audio
    MultiLine = True
    TabOrder = 2
    TabPosition = tpLeft
    object Video: TTabSheet
      Caption = 'Video'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lblResolution: TLabel
        Left = 11
        Top = 24
        Width = 54
        Height = 13
        Caption = 'Resolution:'
      end
      object lblFrameRate: TLabel
        Left = 11
        Top = 51
        Width = 60
        Height = 13
        Caption = 'Frame Rate:'
      end
      object lblVideoBitRate: TLabel
        Left = 11
        Top = 78
        Width = 42
        Height = 13
        Caption = 'Bit Rate:'
      end
      object edResolution: TEdit
        Left = 80
        Top = 16
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 0
      end
      object edFrameRate: TEdit
        Left = 80
        Top = 43
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 1
      end
      object edVideoBitRate: TEdit
        Left = 80
        Top = 70
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 2
      end
    end
    object Audio: TTabSheet
      Caption = 'Audio'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lbledSamples: TLabel
        Left = 11
        Top = 24
        Width = 64
        Height = 13
        Caption = 'Sample Rate:'
      end
      object lblChannels: TLabel
        Left = 11
        Top = 51
        Width = 43
        Height = 13
        Caption = 'Channel:'
      end
      object lblAudioBitRate: TLabel
        Left = 11
        Top = 78
        Width = 42
        Height = 13
        Caption = 'Bit Rate:'
      end
      object edSamples: TEdit
        Left = 80
        Top = 16
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 0
      end
      object edChannels: TEdit
        Left = 80
        Top = 43
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 1
      end
      object edAudioBitRate: TEdit
        Left = 80
        Top = 70
        Width = 209
        Height = 21
        ReadOnly = True
        TabOrder = 2
      end
    end
  end
end
