object frmBuy: TfrmBuy
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'VMeisoft Flash to Video Converter'
  ClientHeight = 148
  ClientWidth = 382
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
  object Button1: TCommandLinkButton
    Left = 8
    Top = 8
    Width = 353
    Height = 57
    Caption = 'Buy this product'
    CommandLinkHint = 'You can still try this software for 0 days.'
    ImageIndex = 0
    Style = bsCommandLink
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TCommandLinkButton
    Left = 8
    Top = 83
    Width = 353
    Height = 57
    Caption = 'No, thanks'
    CommandLinkHint = 'Continue free trial.'
    ImageIndex = 0
    Style = bsCommandLink
    TabOrder = 1
    OnClick = Button2Click
  end
end
