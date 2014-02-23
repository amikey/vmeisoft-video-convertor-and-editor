unit AudioSettings;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.ExtCtrls,
    Vcl.Buttons, GraphicsSpeedButton, MediaPlayTrackBar,
    GraphicsButton, Vcl.StdCtrls, Vcl.Menus, Vcl.Samples.Spin,
    VideoConverterInt, VideoEffectInt;

type
    TfrmAudioSettings = class(TForm)
        palRender: TPanel;
        palConsole: TPanel;
        tbSeek: TMediaPlayTrackBar;
        PageControl1: TPageControl;
        btnPlay: TGraphicsSpeedButton;
        TabSheet1: TTabSheet;
        TabSheet2: TTabSheet;
        TabSheet3: TTabSheet;
        lblVolumeStream: TLabel;
        tbVolumeStream: TTrackBar;
        Label1: TLabel;
        edMusic: TEdit;
        Label2: TLabel;
        tbVolumeMusic: TTrackBar;
        Label3: TLabel;
        tbVolumeRecord: TTrackBar;
        Button1: TButton;
        lblRecord: TLabel;
        btnImport: TButton;
        btnRemove: TButton;
        btnRemoveRecord: TButton;
        btnClip: TButton;
        lblTime: TLabel;
        procedure FormCreate(Sender: TObject);
        procedure palRenderResize(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure tbSeekChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure Button1Click(Sender: TObject);
        procedure tbVolumeStreamChange(Sender: TObject);
        procedure tbVolumeMusicChange(Sender: TObject);
        procedure tbVolumeRecordChange(Sender: TObject);
    private
        { Private declarations }
        FItem     : PVCItem;
        FMediaInfo: PMediaInfo;
        FPlayer   : Pointer;
        FPlaying  : Boolean;
        FDuration : Integer;
        FChanging : Boolean;
        procedure Pause();
        procedure OnPause();
        procedure SetTimeInfo(sec: Integer);

    private
        procedure OnProgress(var msg: TMessage); message VC_VIDEO_RENDER_PROGRESS;
        procedure OnPlayEnd(var msg: TMessage); message VC_VIDEO_RENDER_END;

    protected
        { protected declarations }
        procedure CreateParams(var Params: TCreateParams); override;

    public
        { Public declarations }
        procedure InitForm(Item: PVCItem);
    end;

implementation

{$R *.dfm}


uses ImageResource, Functions, Profile, Main, Recording;

procedure TfrmAudioSettings.Button1Click(Sender: TObject);
var
    frmRecording: TfrmRecording;
begin

end;

procedure TfrmAudioSettings.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.WndParent := 0;
    Params.ExStyle := Params.ExStyle or WS_EX_APPWINDOW;
end;

procedure TfrmAudioSettings.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerDestroy(FPlayer);
        FPlayer := nil;
    end;
    frmMain.Show();
    Action := caFree;
end;

procedure TfrmAudioSettings.FormCreate(Sender: TObject);
begin
    btnPlay.SetImage(imagePlay);
    PageControl1.ActivePageIndex := 0;
end;

procedure TfrmAudioSettings.palRenderResize(Sender: TObject);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerResize(FPlayer);
    end;
end;

procedure TfrmAudioSettings.tbSeekChange(Sender: TObject);
var
    Pos: Integer;
begin
    Pos := tbSeek.Position;
    SetTimeInfo(Pos);
    vcVideoPlayerSeekTo(FPlayer, Pos * AV_TIME_BASE_LL);
    OnPause();
end;

procedure TfrmAudioSettings.tbVolumeMusicChange(Sender: TObject);
begin
    if FChanging then Exit;
    vcVideoPlayerSetVolume(FPlayer, 1, tbVolumeStream.Position);
end;

procedure TfrmAudioSettings.tbVolumeRecordChange(Sender: TObject);
begin
    if FChanging then Exit;
    vcVideoPlayerSetVolume(FPlayer, 2, tbVolumeStream.Position);
end;

procedure TfrmAudioSettings.tbVolumeStreamChange(Sender: TObject);
begin
    if FChanging then Exit;
    vcVideoPlayerSetVolume(FPlayer, 0, tbVolumeStream.Position);
end;

procedure TfrmAudioSettings.Pause();
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmAudioSettings.OnPause();
begin
    if FPlaying then
    begin
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmAudioSettings.btnPlayClick(Sender: TObject);
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end else
    begin
        vcVideoPlayerResume(FPlayer);
        FPlaying := TRUE;
        btnPlay.SetImage(imagePause);
    end;
end;

procedure TfrmAudioSettings.InitForm(Item: PVCItem);
var
    Duration: Int64;
begin
    FChanging := TRUE;
    FItem := Item;
    FMediaInfo := Item.m_MediaInfo;

    Self.HandleNeeded;
    Self.Caption := FMediaInfo.m_szFileName;
    FPlayer := vcCreateVideoPlayer(palRender.Handle, Self.Handle, FItem, VC_VIDEO_RENDER_MODE_DEST);
    if FItem.m_ClipStop <> 0 then
    begin
        Duration := FItem.m_ClipStop - FItem.m_ClipStart;
    end else
    begin
        Duration := FMediaInfo.m_Duration - FItem.m_ClipStart;
    end;
    FDuration := (Duration + AV_TIME_BASE_LL - 1) div AV_TIME_BASE_LL;
    tbSeek.Max := FDuration;

    lblTime.Caption := '';

    // Page 1
    tbVolumeStream.Enabled := FMediaInfo.m_bAudioStream <> 0;
    tbVolumeStream.Position := FItem.m_AudioTrack[0].m_Volume;

    // Page 2
    if Item.m_AudioTrack[1].m_Media <> nil then
    begin
        edMusic.Text := ExtractFileName(FItem.m_AudioTrack[1].m_Media.m_szFileName);
        tbVolumeMusic.Position := FItem.m_AudioTrack[1].m_Volume;
    end else begin
        edMusic.Text := '';
        btnRemove.Enabled := FALSE;
        btnClip.Enabled := FALSE;
        tbVolumeMusic.Enabled := FALSE;
        tbVolumeMusic.Position := 0;
    end;

    // Page 3
    if Item.m_AudioTrack[2].m_Media <> nil then
    begin
        lblRecord.Caption := 'Recording duration:' + GetAVTimeString(FItem.m_AudioTrack[2].m_Media.m_Duration);
        tbVolumeRecord.Position := FItem.m_AudioTrack[2].m_Volume;
    end else begin
        lblRecord.Caption := 'No recording';
        btnRemoveRecord.Enabled := FALSE;
        tbVolumeRecord.Enabled := FALSE;
        tbVolumeRecord.Position := 0;;
    end;

    FChanging := FALSE;
end;

procedure TfrmAudioSettings.OnProgress(var msg: TMessage);
begin
    SetTimeInfo(msg.LParam);
end;

procedure TfrmAudioSettings.OnPlayEnd(var msg: TMessage);
begin
    FPlaying := FALSE;
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmAudioSettings.SetTimeInfo(sec: Integer);
begin
    tbSeek.Position := sec;
    lblTime.Caption := GetDurationProcString(sec, FDuration);
end;

end.
