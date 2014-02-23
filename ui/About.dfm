object frmAbout: TfrmAbout
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'About '#39'VMeisoft Video Converter and Video Editor'#39
  ClientHeight = 367
  ClientWidth = 617
  Color = clBtnFace
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
  object imgLogo: TImage
    Left = 176
    Top = 8
    Width = 256
    Height = 256
  end
  object LinkLabel1: TLinkLabel
    Left = 8
    Top = 270
    Width = 601
    Height = 89
    AutoSize = False
    Caption = 'LinkLabel1'
    TabOrder = 1
    OnLinkClick = lblCopyrightLinkClick
  end
  object btnClose: TButton
    Left = 534
    Top = 334
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Close'
    Default = True
    ModalResult = 8
    TabOrder = 0
  end
end
