object frmMerge: TfrmMerge
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Merge'
  ClientHeight = 106
  ClientWidth = 475
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblTime: TLabel
    Left = 8
    Top = 56
    Width = 90
    Height = 13
    Caption = 'Elapsed: [0:00:00]'
  end
  object btnCancel: TButton
    Left = 392
    Top = 67
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 0
    OnClick = btnCancelClick
  end
  object ProgressBar1: TProgressBar
    Left = 8
    Top = 24
    Width = 459
    Height = 17
    TabOrder = 1
  end
  object lblOutput: TLinkLabel
    Left = 16
    Top = 80
    Width = 53
    Height = 17
    Caption = 'Complete:'
    TabOrder = 2
    Visible = False
    OnLinkClick = lblOutputLinkClick
  end
  object btnOK: TButton
    Left = 160
    Top = 73
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    Visible = False
    OnClick = btnOKClick
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 264
    Top = 64
  end
end
