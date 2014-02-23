object frmItem: TfrmItem
  Left = 0
  Top = 0
  Align = alTop
  BorderStyle = bsNone
  ClientHeight = 85
  ClientWidth = 640
  Color = clWindow
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnMouseEnter = FormMouseEnter
  OnMouseLeave = FormMouseLeave
  OnPaint = FormPaint
  DesignSize = (
    640
    85)
  PixelsPerInch = 96
  TextHeight = 13
  object lblName: TLabel
    Left = 120
    Top = 6
    Width = 45
    Height = 13
    Caption = 'lblName'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblInfo: TLabel
    Left = 120
    Top = 25
    Width = 30
    Height = 13
    Caption = 'lblInfo'
  end
  object Thumbail: TGraphicsGPImage
    Left = 8
    Top = 4
    Width = 98
    Height = 74
  end
  object btnRedo: TGraphicsSpeedButton
    Left = 597
    Top = 18
    Width = 20
    Height = 20
    Anchors = [akTop, akRight]
    OnClick = btnRedoClick
  end
  object btnCut: TGraphicsSpeedButton
    Left = 189
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnCutClick
  end
  object btnEffect: TGraphicsSpeedButton
    Left = 223
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnEffectClick
  end
  object btnInfo: TGraphicsSpeedButton
    Left = 121
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnInfoClick
  end
  object btnPlay: TGraphicsSpeedButton
    Left = 155
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnPlayClick
  end
  object btnRemove: TGraphicsSpeedButton
    Left = 597
    Top = 50
    Width = 28
    Height = 28
    Anchors = [akTop, akRight]
    OnClick = btnRemoveClick
  end
  object lblTime: TLabel
    Left = 384
    Top = 64
    Width = 38
    Height = 13
    Caption = '0:00:00'
    Visible = False
  end
  object btnSubtitle: TGraphicsSpeedButton
    Left = 257
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnSubtitleClick
  end
  object btnAudio: TGraphicsSpeedButton
    Left = 291
    Top = 50
    Width = 28
    Height = 28
    OnClick = btnAudioClick
  end
  object ProgressBar1: TProgressBar
    Left = 384
    Top = 41
    Width = 128
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    BackgroundColor = clNone
    TabOrder = 0
  end
  object lblInfo2: TLinkLabel
    Left = 384
    Top = 18
    Width = 40
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'lblInfo2'
    TabOrder = 1
    OnLinkClick = lblInfo2LinkClick
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 320
    Top = 8
  end
end
