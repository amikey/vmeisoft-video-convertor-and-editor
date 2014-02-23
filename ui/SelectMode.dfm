object frmSelectMode: TfrmSelectMode
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select Mode'
  ClientHeight = 122
  ClientWidth = 357
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object btnOK: TButton
    Left = 186
    Top = 80
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object btnCancel: TButton
    Left = 267
    Top = 80
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object btnEach: TRadioButton
    Left = 8
    Top = 8
    Width = 200
    Height = 17
    Caption = 'Convert each video file'
    Checked = True
    TabOrder = 2
    TabStop = True
    OnClick = RadioButtonClick
  end
  object btnMerge: TRadioButton
    Left = 8
    Top = 40
    Width = 200
    Height = 17
    Caption = 'Merge all into one video file'
    TabOrder = 3
    OnClick = RadioButtonClick
  end
end
