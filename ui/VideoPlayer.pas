unit VideoPlayer;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.Buttons,
    GraphicsSpeedButton, Vcl.ExtCtrls, Vcl.StdCtrls, GraphicsTrackBar,
    VideoConverterInt;

type
    TfrmVideoPlayer = class(TForm)
        palRender: TPanel;
        palConsole: TPanel;
        btnPlay: TGraphicsSpeedButton;
        tbSeek: TGraphicsTrackBar;
        lblTime: TLabel;
        procedure FormCreate(Sender: TObject);
        procedure palRenderResize(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure tbSeekChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
    private
        { Private declarations }
        FPlayer  : Pointer;
        FPlaying : Boolean;
        FDuration: Integer;
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


uses ImageResource, Functions, Main;

procedure TfrmVideoPlayer.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.WndParent := 0;
    Params.ExStyle := Params.ExStyle or WS_EX_APPWINDOW;
end;

procedure TfrmVideoPlayer.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerDestroy(FPlayer);
        FPlayer := nil;
    end;
    frmMain.Show();
    Action := caFree;
end;

procedure TfrmVideoPlayer.FormCreate(Sender: TObject);
begin
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmVideoPlayer.palRenderResize(Sender: TObject);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerResize(FPlayer);
    end;
end;

procedure TfrmVideoPlayer.tbSeekChange(Sender: TObject);
var
    Pos: Integer;
begin
    Pos := tbSeek.Position;
    SetTimeInfo(Pos);
    vcVideoPlayerSeekTo(FPlayer, Pos * AV_TIME_BASE_LL);
    OnPause();
end;

procedure TfrmVideoPlayer.Pause();
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := False;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmVideoPlayer.OnPause();
begin
    if FPlaying then
    begin
        FPlaying := False;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmVideoPlayer.btnPlayClick(Sender: TObject);
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := False;
        btnPlay.SetImage(imagePlay);
    end else
    begin
        vcVideoPlayerResume(FPlayer);
        FPlaying := True;
        btnPlay.SetImage(imagePause);
    end;
end;

procedure TfrmVideoPlayer.InitForm(Item: PVCItem);
begin
    Self.HandleNeeded;
    Self.Caption := item.m_MediaInfo.m_szFileName;
    FPlayer := vcCreateVideoPlayer(palRender.Handle, Self.Handle, Item, VC_VIDEO_RENDER_MODE_SOURCE);
    FDuration := (Item.m_MediaInfo.m_Duration + AV_TIME_BASE_LL - 1) div AV_TIME_BASE_LL;
    tbSeek.Max := FDuration;
end;

procedure TfrmVideoPlayer.OnProgress(var msg: TMessage);
begin
    tbSeek.Position := msg.LParam;
    SetTimeInfo(msg.LParam);
end;

procedure TfrmVideoPlayer.OnPlayEnd(var msg: TMessage);
begin
    FPlaying := False;
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmVideoPlayer.SetTimeInfo(sec: Integer);
begin
    lblTime.Caption := GetDurationProcString(sec, FDuration);
end;

end.
