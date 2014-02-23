object frmVideoPlayer: TfrmVideoPlayer
  Left = 0
  Top = 0
  Caption = 'frmVideoPlayer'
  ClientHeight = 337
  ClientWidth = 635
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object palRender: TPanel
    Left = 0
    Top = 0
    Width = 635
    Height = 296
    Align = alClient
    TabOrder = 0
    OnResize = palRenderResize
  end
  object palConsole: TPanel
    Left = 0
    Top = 296
    Width = 635
    Height = 41
    Align = alBottom
    BorderWidth = 4
    TabOrder = 1
    DesignSize = (
      635
      41)
    object btnPlay: TGraphicsSpeedButton
      Left = 5
      Top = 5
      Width = 33
      Height = 31
      Align = alLeft
      OnClick = btnPlayClick
      ExplicitLeft = 9
      ExplicitTop = 6
      ExplicitHeight = 32
    end
    object lblTime: TLabel
      Left = 535
      Top = 14
      Width = 92
      Height = 13
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '00:00:00/00:00:00'
    end
    object tbSeek: TGraphicsTrackBar
      Left = 44
      Top = 8
      Width = 485
      Height = 24
      OnChange = tbSeekChange
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
    end
  end
end
