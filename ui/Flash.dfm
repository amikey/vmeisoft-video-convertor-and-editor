object frmFlash: TfrmFlash
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'frmFlash'
  ClientHeight = 350
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object lblVersion: TLabel
    Left = 24
    Top = 312
    Width = 65
    Height = 13
    Caption = 'Version:1.0.0'
  end
  object lblName: TLabel
    Left = 16
    Top = 286
    Width = 191
    Height = 18
    Caption = 'VMeisoft Video Converter'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object imgLogo: TImage
    Left = 40
    Top = 8
    Width = 256
    Height = 256
  end
end
