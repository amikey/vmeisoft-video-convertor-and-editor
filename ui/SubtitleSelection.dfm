object frmSubtitleSelection: TfrmSubtitleSelection
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select Subtitle'
  ClientHeight = 67
  ClientWidth = 234
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ComboBox1: TComboBox
    Left = 8
    Top = 8
    Width = 217
    Height = 21
    Style = csDropDownList
    TabOrder = 0
  end
  object Button1: TButton
    Left = 150
    Top = 35
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
end
