object frmMain: TfrmMain
  Left = 0
  Top = 0
  Caption = 'VMeisoft Video Converter'
  ClientHeight = 400
  ClientWidth = 640
  Color = clBtnFace
  Constraints.MinHeight = 438
  Constraints.MinWidth = 656
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ScrollBox1: TScrollBox
    Left = 0
    Top = 96
    Width = 640
    Height = 153
    HorzScrollBar.Visible = False
    VertScrollBar.Smooth = True
    VertScrollBar.Tracking = True
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 640
    Height = 58
    Align = alTop
    BevelOuter = bvNone
    BorderWidth = 4
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 1
    object btnAdd: TGraphicsSpeedButton
      Left = 8
      Top = 4
      Width = 50
      Height = 50
      OnClick = btnAddClick
    end
    object btnStart: TGraphicsSpeedButton
      Left = 84
      Top = 4
      Width = 50
      Height = 50
      Enabled = False
      OnClick = btnStartClick
    end
    object btnOptions: TGraphicsSpeedButton
      Left = 327
      Top = 4
      Width = 50
      Height = 50
      OnClick = btnOptionsClick
    end
    object btnFormat: TGraphicsCommandLink
      Left = 160
      Top = 4
      Width = 161
      Height = 50
      OnClick = btnFormatClick
    end
    object btnAbout: TGraphicsSpeedButton
      Left = 584
      Top = 4
      Width = 50
      Height = 50
      OnClick = btnAboutClick
    end
  end
  object WebBrowser1: TWebBrowser
    Left = 0
    Top = 272
    Width = 640
    Height = 128
    TabOrder = 2
    ControlData = {
      4C000000254200003B0D00000100000001020000000000000000000000000000
      000000004C000000000000000000000001000000E0D057007335CF11AE690800
      2B2E126208000000000000004C0000000114020000000000C000000000000046
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000100000000000000000000000000000000000000}
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 120
    Top = 168
  end
end
