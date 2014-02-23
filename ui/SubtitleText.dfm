object frmSubtitleText: TfrmSubtitleText
  Left = 245
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Edit Line'
  ClientHeight = 93
  ClientWidth = 491
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object lblText: TLabel
    Left = 8
    Top = 9
    Width = 26
    Height = 13
    Caption = 'Text:'
  end
  object EditText: TEdit
    Left = 8
    Top = 27
    Width = 475
    Height = 21
    MaxLength = 255
    TabOrder = 0
  end
  object btnOK: TButton
    Left = 326
    Top = 60
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object btnCancel: TButton
    Left = 406
    Top = 60
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
end
