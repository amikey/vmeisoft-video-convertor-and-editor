object frmRecording: TfrmRecording
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Recording'
  ClientHeight = 93
  ClientWidth = 283
  Color = clBtnFace
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
  object lblTime: TLabel
    Left = 81
    Top = 27
    Width = 32
    Height = 13
    Caption = 'lblTime'
    Visible = False
  end
  object imgMic: TGraphicsGPImage
    Left = 8
    Top = 8
    Width = 32
    Height = 32
    Visible = False
  end
  object btnMute: TGraphicsCheckButton
    Left = 252
    Top = 8
    Width = 23
    Height = 22
  end
  object GraphicsVolume1: TGraphicsVolume
    Left = 46
    Top = 8
    Width = 16
    Height = 32
    Visible = False
  end
  object btnStart: TGraphicsButton
    Left = 8
    Top = 54
    Width = 120
    Height = 25
    Caption = 'Start'
    ImageIndex = 0
    TabOrder = 0
    Visible = False
    OnClick = btnStartClick
  end
  object btnStop: TGraphicsButton
    Left = 155
    Top = 54
    Width = 120
    Height = 25
    Caption = 'Stop'
    Enabled = False
    ImageIndex = 0
    TabOrder = 1
    Visible = False
  end
  object btnCurrent: TButton
    Left = 144
    Top = 8
    Width = 120
    Height = 25
    Caption = 'From the current time'
    TabOrder = 2
    OnClick = btnCurrentClick
  end
  object btnBeginning: TButton
    Left = 151
    Top = 26
    Width = 120
    Height = 25
    Caption = 'From beginning'
    TabOrder = 3
    OnClick = btnBeginningClick
  end
end
