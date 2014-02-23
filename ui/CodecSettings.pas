unit CodecSettings;

interface

{$WARN UNIT_PLATFORM OFF}


uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
    System.Classes, Vcl.Graphics, System.UITypes, Winapi.ShellAPI,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ImgList, Vcl.ComCtrls, Vcl.StdCtrls,
    Vcl.Samples.Spin, Vcl.Buttons,  Vcl.ExtCtrls, Vcl.FileCtrl,
    Vcl.Imaging.pngimage,     Vcl.Menus,
    Profile,
    GraphicsGroupRadio, GraphicsButton, VideoConverterInt;

type
    TfrmCodecSettings = class(TForm)

        PageControl1: TPageControl;
        TabSheet2: TTabSheet;
        Panel1: TPanel;
        sbHD: TGraphicsGroupRadio;
        sbVideo: TGraphicsGroupRadio;
        sbDevice: TGraphicsGroupRadio;
        sbWeb: TGraphicsGroupRadio;
        sbApp: TGraphicsGroupRadio;
    sbAudio: TGraphicsGroupRadio;
        Panel2: TPanel;
        lbFormat: TListBox;
        memoDec: TMemo;
        TabSheet3: TTabSheet;
        lblFilter: TLabel;
        gbVideo: TGroupBox;
        lblVCodec: TLabel;
        lblResolution: TLabel;
        lblWidth: TLabel;
        lblHeight: TLabel;
        lblFrameRate: TLabel;
        lblVBitrate: TLabel;
        lblHintVBitrate: TLabel;
        lblHitFrameRate: TLabel;
        lblAspect: TLinkLabel;
        cbVCodec: TComboBox;
        cbResolution: TComboBox;
        cbFrameRate: TComboBox;
        cbVBitrate: TComboBox;
        spWidth: TSpinEdit;
        spHeight: TSpinEdit;
        gbAudio: TGroupBox;
        lblACodec: TLabel;
        lblChannels: TLabel;
        lblABitRate: TLabel;
        lblHintABitrate: TLabel;
        lblSampleRate: TLabel;
        lblHintSampleRate: TLabel;
        cbACodec: TComboBox;
        cbChannels: TComboBox;
        cbABitrate: TComboBox;
        cbSampleRate: TComboBox;
        cbTarget: TComboBox;
        btnCancel: TButton;
        PopupMenu3: TPopupMenu;
        ClipAspect4x3: TMenuItem;
        ClipAspect16x9: TMenuItem;
        ImageList1: TImageList;

        procedure SpeedButtonClick(Sender: TObject);
        procedure lbFormatClick(Sender: TObject);
        procedure cbResolutionChange(Sender: TObject);
        procedure spWidthChange(Sender: TObject);
        procedure spHeightChange(Sender: TObject);
        procedure lbFormatDrawItem(Control: TWinControl; Index: Integer; Rect: TRect; State: TOwnerDrawState);
        procedure FormCreate(Sender: TObject);
        procedure cbTargetChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure ClipAspect4x3Click(Sender: TObject);
        procedure ClipAspect16x9Click(Sender: TObject);
        procedure cbVCodecChange(Sender: TObject);
        procedure cbFrameRateChange(Sender: TObject);
        procedure cbVBitrateChange(Sender: TObject);
        procedure cbACodecChange(Sender: TObject);
        procedure cbSampleRateChange(Sender: TObject);
        procedure cbChannelsChange(Sender: TObject);
        procedure cbABitrateChange(Sender: TObject);
    private
        { Private declarations }
        FChanging: Boolean;

        procedure SetProfileValues();
        procedure SetTargetValues();
        procedure SetVideoValues();
        procedure SetAudioValues();

    public
        { Public declarations }

    end;

implementation

uses
    Main, Defines, Functions, ImageResource, CommonData;

{$R *.dfm}


procedure TfrmCodecSettings.SpeedButtonClick(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    if sbHD.Down then
    begin
        _ProfleSettings.m_Group := GROUP_HD;
    end else if sbVideo.Down then
    begin
        _ProfleSettings.m_Group := GROUP_VIDEO;
    end else if sbDevice.Down then
    begin
        _ProfleSettings.m_Group := GROUP_DEVICE;
    end else if sbWeb.Down then
    begin
        _ProfleSettings.m_Group := GROUP_WEB;
    end else if sbApp.Down then
    begin
        _ProfleSettings.m_Group := GROUP_APP;
    end else if sbAudio.Down then
    begin
        _ProfleSettings.m_Group := GROUP_AUDIO;
    end;
    GetDefaultProfileSetting1(_ProfleSettings, _ProfleSettings.m_Group);
    SetProfileValues();
    SetTargetValues();
    SetVideoValues();
    SetAudioValues();
    FChanging := FALSE;
end;

procedure TfrmCodecSettings.lbFormatClick(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    _ProfleSettings.m_Profile := lbFormat.ItemIndex;
    GetDefaultProfileSetting2(_ProfleSettings, _ProfleSettings.m_Group, _ProfleSettings.m_Profile);
    memoDec.Text := GetProfileDesc(_ProfleSettings);
    SetTargetValues();
    SetVideoValues();
    SetAudioValues();
    FChanging := FALSE;
end;

procedure TfrmCodecSettings.lbFormatDrawItem(Control: TWinControl; Index: Integer; Rect: TRect; State: TOwnerDrawState);
var
    Profile: PProfileTarget;
    Text   : String;
begin
    Profile := PProfileTarget(lbFormat.Items.Objects[Index]);
    Text := lbFormat.Items[Index];

    lbFormat.Canvas.FillRect(Rect);

    DrawProfileThumbnail(Profile, lbFormat.Canvas.Handle, Rect.Left + 2, Rect.Top + 1);

    Rect.Left := Rect.Left + THUMBNAIL_WIDTH + 5;
    lbFormat.Canvas.TextRect(Rect,
      Rect.Left,
      Rect.Top + (Rect.Bottom - Rect.Top - lbFormat.Canvas.TextHeight(Text)) div 2,
      Text);
end;

procedure TfrmCodecSettings.cbABitrateChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_ABitRate := cbABitrate.ItemIndex;
end;

procedure TfrmCodecSettings.cbACodecChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_ACodec := cbACodec.ItemIndex;
end;

procedure TfrmCodecSettings.cbChannelsChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_Channel := cbChannels.ItemIndex;
end;

procedure TfrmCodecSettings.cbFrameRateChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_FrameRate := cbFrameRate.ItemIndex;
end;

procedure TfrmCodecSettings.cbResolutionChange(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;

    _ProfleSettings.m_Res := Integer(cbResolution.Items.Objects[cbResolution.ItemIndex]);
    case _ProfleSettings.m_Res of
        VIDEO_RES_CUSTOM:
            begin
                spWidth.Enabled := TRUE;
                spHeight.Enabled := TRUE;
            end;
    else
        begin
            SetResolutionByIndex(_ProfleSettings, _ProfleSettings.m_Res);
            spWidth.Enabled := FALSE;
            spHeight.Enabled := FALSE;
        end;
    end;

    spWidth.Value := OutputWidth();
    spHeight.Value := OutputHeight();
    lblAspect.Caption := GetVideoAspectString(_ProfleSettings);
    FChanging := FALSE;

end;

procedure TfrmCodecSettings.cbSampleRateChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_SampleRate := cbSampleRate.ItemIndex;
end;

procedure TfrmCodecSettings.spWidthChange(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    _ProfleSettings.m_ResCustom.m_Width := spWidth.Value;
    cbResolution.ItemIndex := GetVideoResolutionIndex(_ProfleSettings);
    lblAspect.Caption := GetVideoAspectString(_ProfleSettings);
    FChanging := FALSE;
end;

procedure TfrmCodecSettings.spHeightChange(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    _ProfleSettings.m_ResCustom.m_Height := spHeight.Value;
    cbResolution.ItemIndex := GetVideoResolutionIndex(_ProfleSettings);
    lblAspect.Caption := GetVideoAspectString(_ProfleSettings);
    FChanging := FALSE;
end;

procedure TfrmCodecSettings.SetProfileValues();
begin
    AddProfileStrins(_ProfleSettings, lbFormat.Items);
    lbFormat.ItemIndex := _ProfleSettings.m_Profile;
    memoDec.Text := GetProfileDesc(_ProfleSettings);
    SetTargetValues();
    SetVideoValues();
    SetAudioValues();
end;

procedure TfrmCodecSettings.SetTargetValues();
begin
    AddTargetStrins(_ProfleSettings, cbTarget.Items);
    cbTarget.ItemIndex := _ProfleSettings.m_Target;
end;

procedure TfrmCodecSettings.SetVideoValues();
begin
    if _ProfleSettings.m_VCodec = -1 then
    begin

        cbVCodec.Items.Clear();
        cbResolution.Items.Clear();
        cbFrameRate.Items.Clear();
        cbVBitrate.Items.Clear();
        spWidth.Value := 0;
        spHeight.Value := 0;
        lblAspect.Caption := '';

        cbVCodec.Enabled := FALSE;
        cbResolution.Enabled := FALSE;
        cbFrameRate.Enabled := FALSE;
        cbVBitrate.Enabled := FALSE;
        spWidth.Enabled := FALSE;
        spHeight.Enabled := FALSE;

    end else begin

        AddVideoCodecStrins(_ProfleSettings, cbVCodec.Items);
        AddVideoResolutionStrins(_ProfleSettings, cbResolution.Items);
        AddVideoFrameRateStrins(_ProfleSettings, cbFrameRate.Items);
        AddVideoBitRateStrins(_ProfleSettings, cbVBitrate.Items);
        spWidth.Value := OutputWidth();
        spHeight.Value := OutputHeight();
        spWidth.Enabled := _ProfleSettings.m_Res = VIDEO_RES_CUSTOM;
        spHeight.Enabled := _ProfleSettings.m_Res = VIDEO_RES_CUSTOM;
        lblAspect.Caption := GetVideoAspectString(_ProfleSettings);

        cbVCodec.Enabled := TRUE;
        cbFrameRate.Enabled := TRUE;
        cbVBitrate.Enabled := TRUE;
        cbResolution.Enabled := TRUE;

        cbVCodec.ItemIndex := _ProfleSettings.m_VCodec;
        cbResolution.ItemIndex := GetResolutionIndex(_ProfleSettings);
        cbFrameRate.ItemIndex := _ProfleSettings.m_FrameRate;
        cbVBitrate.ItemIndex := _ProfleSettings.m_VBitRate;
    end;
end;

procedure TfrmCodecSettings.SetAudioValues();
begin
    if _ProfleSettings.m_ACodec = -1 then
    begin
        cbACodec.Items.Clear();
        cbSampleRate.Items.Clear();
        cbChannels.Items.Clear();
        cbABitrate.Items.Clear();

        cbACodec.Enabled := FALSE;
        cbSampleRate.Enabled := FALSE;
        cbChannels.Enabled := FALSE;
        cbABitrate.Enabled := FALSE;

    end else begin
        AddAudioCodecStrins(_ProfleSettings, cbACodec.Items);
        AddAudioSampleRateStrins(_ProfleSettings, cbSampleRate.Items);
        AddAudioChannelStrins(_ProfleSettings, cbChannels.Items);
        AddAudioBitRateStrins(_ProfleSettings, cbABitrate.Items);

        cbACodec.Enabled := TRUE;
        cbSampleRate.Enabled := TRUE;
        cbChannels.Enabled := TRUE;
        cbABitrate.Enabled := TRUE;

        cbACodec.ItemIndex := _ProfleSettings.m_ACodec;
        cbSampleRate.ItemIndex := _ProfleSettings.m_SampleRate;
        cbChannels.ItemIndex := _ProfleSettings.m_Channel;
        cbABitrate.ItemIndex := _ProfleSettings.m_ABitRate;
    end;
end;

procedure TfrmCodecSettings.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    Action := caFree;
end;

procedure TfrmCodecSettings.FormCreate(Sender: TObject);
begin
    FChanging := TRUE;
    PageControl1.ActivePageIndex := 0;
    case _ProfleSettings.m_Group of
        GROUP_HD: sbHD.Down := TRUE;
        GROUP_VIDEO: sbVideo.Down := TRUE;
        GROUP_DEVICE: sbDevice.Down := TRUE;
        GROUP_WEB: sbWeb.Down := TRUE;
        GROUP_APP: sbApp.Down := TRUE;
        GROUP_AUDIO: sbAudio.Down := TRUE;
    end;
    SetProfileValues();

    sbHD.SetImage(imageHD, 'HD Video');
    sbVideo.SetImage(imageVideo, 'Video');
    sbDevice.SetImage(imageDevice, 'Device');
    sbWeb.SetImage(imageWeb, 'Web');
    sbApp.SetImage(imageApp, 'Application');
    sbAudio.SetImage(imageMusic, 'Music');

    FChanging := FALSE;
end;

procedure TfrmCodecSettings.cbTargetChange(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    _ProfleSettings.m_Target := cbTarget.ItemIndex;
    GetDefaultProfileSetting3(_ProfleSettings, _ProfleSettings.m_Group, _ProfleSettings.m_Profile, _ProfleSettings.m_Target);
    SetVideoValues();
    SetAudioValues();
    FChanging := FALSE;
end;

procedure TfrmCodecSettings.cbVBitrateChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_VBitRate := cbVBitrate.ItemIndex;
end;

procedure TfrmCodecSettings.cbVCodecChange(Sender: TObject);
begin
    if FChanging then Exit;
    _ProfleSettings.m_VCodec := cbVCodec.ItemIndex;
end;

procedure TfrmCodecSettings.ClipAspect16x9Click(Sender: TObject);
var
    n: Integer;
begin
    FChanging := TRUE;

    n := (_ProfleSettings.m_ResCustom.m_Width + 63) div 64;
    _ProfleSettings.m_ResCustom.m_Width := n * 64;
    _ProfleSettings.m_ResCustom.m_Height := n * 48;

    spWidth.Value := OutputWidth();
    spHeight.Value := OutputHeight();
    lblAspect.Caption := GetAspectString(OutputWidth(), OutputHeight(), TRUE);

    FChanging := FALSE;
end;

procedure TfrmCodecSettings.ClipAspect4x3Click(Sender: TObject);
var
    n: Integer;
begin
    FChanging := TRUE;

    n := (_ProfleSettings.m_ResCustom.m_Width + 255) div 256;
    _ProfleSettings.m_ResCustom.m_Width := n * 256;
    _ProfleSettings.m_ResCustom.m_Height := n * 144;

    spWidth.Value := OutputWidth();
    spHeight.Value := OutputHeight();
    lblAspect.Caption := GetAspectString(OutputWidth(), OutputHeight(), TRUE);

    FChanging := FALSE;
end;

end.
