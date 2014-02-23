unit Recording;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, GraphicsButton,
    GraphicsGPImage, GraphicsVolume, Vcl.Buttons, GraphicsCheckButton,
    VideoConverterInt;

type
    TfrmRecording = class(TForm)
        GraphicsVolume1: TGraphicsVolume;
        lblTime: TLabel;
        imgMic: TGraphicsGPImage;
        btnStart: TGraphicsButton;
        btnStop: TGraphicsButton;
        btnCurrent: TButton;
        btnBeginning: TButton;
        btnMute: TGraphicsCheckButton;
        procedure btnCurrentClick(Sender: TObject);
        procedure btnBeginningClick(Sender: TObject);
        procedure FormCreate(Sender: TObject);
        procedure btnStartClick(Sender: TObject);
    private
        { Private declarations }
        FItem     : PVCItem;
        FRecording: Boolean;
        FPlayer   : Pointer;
        FRecorder : Pointer;
        FMediaInfo: PMediaInfo;
        FStartTime: Int64;
        FStopTime : Int64;
        procedure ShowRecoder();
        procedure OnSample(var Message: TMessage); message VC_RECORDING_SAMPLE;
        procedure OnDuration(var Message: TMessage); message VC_RECORDING_DURATION;
    public
        { Public declarations }
        procedure InitForm(Item: PVCItem; Player: Pointer);
    end;

implementation

{$R *.dfm}


uses
    ImageResource, Functions;

procedure TfrmRecording.ShowRecoder();
begin
    imgMic.Visible := TRUE;
    GraphicsVolume1.Visible := TRUE;
    lblTime.Visible := TRUE;
    btnCurrent.Visible := FALSE;
    btnBeginning.Visible := FALSE;
    btnStart.Visible := TRUE;
    btnStop.Visible := TRUE;
end;

procedure TfrmRecording.btnBeginningClick(Sender: TObject);
begin
    ShowRecoder();
    vcVideoPlayerSeekTo(FPlayer, 0);
    FRecorder := AudioRecorderCreate(Self.Handle);
    FMediaInfo := AudioRecorderGetInfo(FRecorder);
    FStartTime := FItem.m_ClipStart;
    if FItem.m_ClipStop = 0 then
    begin
        FStopTime := FItem.m_ClipStop - FStartTime;
    end else begin
        FStopTime := FItem.m_MediaInfo.m_Duration - FItem.m_ClipStart;
    end;
end;

procedure TfrmRecording.btnCurrentClick(Sender: TObject);
begin
    ShowRecoder();
    FRecorder := AudioRecorderCreate(Self.Handle);
    FMediaInfo := AudioRecorderGetInfo(FRecorder);
    FStartTime := FItem.m_ClipStart + vcVideoPlayerCurrentTime(FPlayer);
    if FItem.m_ClipStop = 0 then
    begin
        FStopTime := FItem.m_ClipStop - FStartTime;
    end else begin
        FStopTime := FItem.m_MediaInfo.m_Duration - FStartTime;
    end;
end;

procedure TfrmRecording.btnStartClick(Sender: TObject);
begin
    if FRecording then
    begin
        btnStart.SetImage(imageRecord);
        FRecording := FALSE;
    end else begin
        btnStart.SetImage(imagePause);
        FRecording := TRUE;
    end;
    btnStop.Enabled := TRUE;
end;

procedure TfrmRecording.FormCreate(Sender: TObject);
begin
    btnCurrent.Left := btnStart.Left;
    btnCurrent.Top := btnStart.Top;
    btnBeginning.Left := btnStop.Left;
    btnBeginning.Top := btnStop.Top;
    btnStart.SetImage(imageRecord);
    btnStop.SetImage(imageStop);
end;

procedure TfrmRecording.InitForm(Item: PVCItem; Player: Pointer);
begin
    FPlayer := Player;
    if vcVideoPlayerCurrentTime(FPlayer) = 0 then
    begin
        btnBeginningClick(nil);
    end;
end;

procedure TfrmRecording.OnDuration(var Message: TMessage);
var
    t: Int64;
begin
    t := Message.LParam * AV_TIME_BASE_LL + Message.WParam;
    lblTime.Caption := GetAVTimeString(t);
    if t > FStopTime then
    begin

    end;
end;

procedure TfrmRecording.OnSample(var Message: TMessage);
begin
    GraphicsVolume1.SetVolume(Message.LParam);
end;

end.
