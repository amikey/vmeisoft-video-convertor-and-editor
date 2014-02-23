object frmSubtitle: TfrmSubtitle
  Left = 0
  Top = 0
  Caption = 'Subtitle'
  ClientHeight = 495
  ClientWidth = 916
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 273
    Height = 495
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 0
    object ListView1: TListView
      Left = 0
      Top = 0
      Width = 273
      Height = 415
      Align = alClient
      Columns = <
        item
          Caption = 'Start'
          Width = 80
        end
        item
          Caption = 'Duration'
          Width = 80
        end
        item
          AutoSize = True
          Caption = 'Title'
        end>
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnCustomDrawItem = ListView1CustomDrawItem
      OnMouseDown = ListView1MouseDown
      OnSelectItem = ListView1SelectItem
    end
    object Panel3: TPanel
      Left = 0
      Top = 415
      Width = 273
      Height = 80
      Align = alBottom
      TabOrder = 1
      object btnEdit: TGraphicsSpeedButton
        Left = 8
        Top = 6
        Width = 28
        Height = 28
        Enabled = False
        OnClick = btnEditClick
      end
      object btnAdd: TGraphicsSpeedButton
        Left = 39
        Top = 6
        Width = 28
        Height = 28
        OnClick = btnAddClick
      end
      object btnImport: TGraphicsSpeedButton
        Left = 8
        Top = 48
        Width = 73
        Height = 28
        Caption = '       Import'
        OnClick = btnImportClick
      end
      object btnDelete: TGraphicsSpeedButton
        Left = 231
        Top = 6
        Width = 28
        Height = 28
        Enabled = False
        OnClick = btnDeleteClick
      end
      object btnDefault: TGraphicsSpeedButton
        Left = 121
        Top = 6
        Width = 28
        Height = 28
        OnClick = btnDefaultClick
      end
    end
  end
  object Panel2: TPanel
    Left = 273
    Top = 0
    Width = 643
    Height = 495
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object palPlayer: TPanel
      Left = 0
      Top = 0
      Width = 643
      Height = 422
      Align = alClient
      TabOrder = 0
      OnResize = palPlayerResize
    end
    object palCosole: TPanel
      Left = 0
      Top = 422
      Width = 643
      Height = 73
      Align = alBottom
      AutoSize = True
      TabOrder = 1
      DesignSize = (
        643
        73)
      object btnPlay: TGraphicsSpeedButton
        Left = 7
        Top = 6
        Width = 33
        Height = 32
        OnClick = btnPlayClick
      end
      object btnSetStart: TGraphicsSpeedButton
        Left = 72
        Top = 44
        Width = 28
        Height = 28
      end
      object btnSetStop: TGraphicsSpeedButton
        Left = 106
        Top = 44
        Width = 28
        Height = 28
      end
      object lblTime: TLabel
        Left = 483
        Top = 44
        Width = 150
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        AutoSize = False
        Caption = '00:00:00.0/00:00:00.0'
      end
      object tbSeek: TMediaPlayTrackBar
        Left = 46
        Top = 1
        Width = 587
        Height = 37
        OnChange = tbSeekChange
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 160
    Top = 448
    object miEdit: TMenuItem
      Caption = 'Edit ...'
      OnClick = miEditClick
    end
    object miAdd: TMenuItem
      Caption = 'Add...'
      OnClick = miAddClick
    end
    object miDelete: TMenuItem
      Caption = 'Delete'
      OnClick = miDeleteClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object miImport: TMenuItem
      Caption = 'Import'
      OnClick = miImportClick
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 368
    Top = 288
  end
end
