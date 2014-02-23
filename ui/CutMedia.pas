unit CutMedia;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.ExtCtrls,
    Vcl.Buttons, GraphicsSpeedButton, MediaPlayTrackBar,
    GraphicsButton, Vcl.StdCtrls, Vcl.Menus, Vcl.Samples.Spin,
    VideoConverterInt, VideoEffectInt;

type
    TfrmCut = class(TForm)
        palRender: TPanel;
        palConsole: TPanel;
        StatusBar1: TStatusBar;
        tbSeek: TMediaPlayTrackBar;
        PageControl1: TPageControl;
        btnPlay: TGraphicsSpeedButton;
        TabSheet1: TTabSheet;
        TabSheet2: TTabSheet;
        btnTrimStart: TGraphicsButton;
        btnTrimStop: TGraphicsButton;
        btnNextFrame: TGraphicsButton;
        btnNextTime: TGraphicsButton;
        btnPreTime: TGraphicsButton;
        Label5: TLabel;
        Label6: TLabel;
        Label7: TLabel;
        Label9: TLabel;
        Label4: TLabel;
        lblClipAspect: TLinkLabel;
        btnCropReset: TGraphicsButton;
        spCropLeft: TSpinEdit;
        spCropTop: TSpinEdit;
        spCropWidth: TSpinEdit;
        spCropHeight: TSpinEdit;
        PopupMenu3: TPopupMenu;
        ClipAspect4x3: TMenuItem;
        ClipAspect16x9: TMenuItem;
        btnTrimReset: TGraphicsButton;
        procedure FormCreate(Sender: TObject);
        procedure palRenderResize(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure tbSeekChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure PageControl1Change(Sender: TObject);
        procedure btnTrimStartClick(Sender: TObject);
        procedure btnTrimResetClick(Sender: TObject);
        procedure btnTrimStopClick(Sender: TObject);
        procedure btnCropResetClick(Sender: TObject);
        procedure spCropLeftChange(Sender: TObject);
        procedure spCropTopChange(Sender: TObject);
        procedure spCropWidthChange(Sender: TObject);
        procedure spCropHeightChange(Sender: TObject);
        procedure lblClipAspectLinkClick(Sender: TObject; const Link: string; LinkType: TSysLinkType);
        procedure btnNextFrameClick(Sender: TObject);
        procedure btnNextTimeClick(Sender: TObject);
        procedure btnPreTimeClick(Sender: TObject);
    private
        { Private declarations }
        FItem       : PVCItem;
        FPlayer     : Pointer;
        FPlaying    : Boolean;
        FDuration   : Integer;
        FCropRect   : CRect;
        FChanging   : Boolean;
        FVideoWidth : Integer;
        FVideoHeight: Integer;
        procedure Pause();
        procedure OnPause();
        procedure SetTimeInfo(sec: Integer);
        procedure ShowMediaStartStop();
        procedure SetCropValues();

    private
        procedure OnProgress(var msg: TMessage); message VC_VIDEO_RENDER_PROGRESS;
        procedure OnPlayEnd(var msg: TMessage); message VC_VIDEO_RENDER_END;
        procedure OnCropChanged(var msg: TMessage); message VC_VIDEO_PLAYER_CROP;

    protected
        { protected declarations }
        procedure CreateParams(var Params: TCreateParams); override;

    public
        { Public declarations }
        procedure InitForm(Item: PVCItem);
    end;

implementation

{$R *.dfm}


uses ImageResource, Functions, Profile, Main;

procedure TfrmCut.SetCropValues();
    function ZeroValue(a, b: Integer): Integer;
    begin
        if a = 0 then ZeroValue := b
        else ZeroValue := a;
    end;

begin
    spCropLeft.Value := FCropRect.X;
    spCropTop.Value := FCropRect.Y;
    spCropWidth.Value := ZeroValue(FCropRect.Width, FVideoWidth);
    spCropHeight.Value := ZeroValue(FCropRect.Height, FVideoHeight);
    lblClipAspect.Caption := GetAspectString(spCropWidth.Value, spCropHeight.Value, TRUE);
end;

procedure TfrmCut.ShowMediaStartStop();
var
    s, e, d: Integer;
begin
    s := FItem.m_ClipStart div AV_TIME_BASE_LL;
    e := FItem.m_ClipStop div AV_TIME_BASE_LL;
    d := FDuration;

    if e >= d then
    begin
        e := 0;
        FItem.m_ClipStop := 0;
    end;

    if s = 0 then
    begin
        StatusBar1.Panels[5].Text := 'Beginning';
    end else begin
        StatusBar1.Panels[5].Text := GetDurationString(s);
    end;

    if e = 0 then
    begin
        StatusBar1.Panels[7].Text := 'Ending';
    end else begin
        StatusBar1.Panels[7].Text := GetDurationString(e);
    end;

    if (e <> 0) then
    begin
        tbSeek.SetSelection(s, e);
        StatusBar1.Panels[9].Text := GetDurationString(e - s);
    end else if (s <> 0) then
    begin
        tbSeek.SetSelection(s, d);
        StatusBar1.Panels[9].Text := GetDurationString(d - s);
    end else begin
        tbSeek.SetSelection(0, 0);
        StatusBar1.Panels[9].Text := '';
    end;

    btnTrimReset.Enabled := (FItem.m_ClipStart <> 0) or (FItem.m_ClipStop <> 0);
    vcVideoPlayerStartStop(FPlayer, FItem.m_ClipStart, FItem.m_ClipStop);
end;

procedure TfrmCut.btnTrimResetClick(Sender: TObject);
begin
    FItem.m_ClipStart := 0;
    FItem.m_ClipStop := 0;
    ShowMediaStartStop();
end;

procedure TfrmCut.btnTrimStartClick(Sender: TObject);
begin
    if tbSeek.Position >= tbSeek.Max then Exit;
    FItem.m_ClipStart := vcVideoPlayerCurrentTime(FPlayer);
    if (FItem.m_ClipStart >= FItem.m_ClipStop) then
    begin
        FItem.m_ClipStop := 0;
    end;
    ShowMediaStartStop();
end;

procedure TfrmCut.btnTrimStopClick(Sender: TObject);
begin
    if tbSeek.Position = 0 then Exit;
    FItem.m_ClipStop := vcVideoPlayerCurrentTime(FPlayer);
    if (FItem.m_ClipStart >= FItem.m_ClipStop) then
    begin
        FItem.m_ClipStart := 0;
    end;
    ShowMediaStartStop();

end;

procedure TfrmCut.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.WndParent := 0;
    Params.ExStyle := Params.ExStyle or WS_EX_APPWINDOW;
end;

procedure TfrmCut.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerDestroy(FPlayer);
        FPlayer := nil;
    end;
    frmMain.Show();
    Action := caFree;
end;

procedure TfrmCut.FormCreate(Sender: TObject);
begin
    btnPlay.SetImage(imagePlay);
    PageControl1.ActivePageIndex := 0;
end;

procedure TfrmCut.lblClipAspectLinkClick(Sender: TObject;
  const Link: string; LinkType: TSysLinkType);
var
    LPoint: TPoint;
begin
    if FCropRect.Width * 3 = FCropRect.Height * 4 then
    begin
        ClipAspect4x3.Enabled := FALSE;
        ClipAspect4x3.Checked := TRUE;
    end else begin
        ClipAspect4x3.Enabled := TRUE;
        ClipAspect4x3.Checked := FALSE;
    end;
    if FCropRect.Width * 9 = FCropRect.Height * 16 then
    begin
        ClipAspect16x9.Enabled := FALSE;
        ClipAspect16x9.Checked := TRUE;
    end else begin
        ClipAspect16x9.Enabled := TRUE;
        ClipAspect16x9.Checked := FALSE;
    end;
    LPoint := lblClipAspect.ClientToScreen(Point(0, lblClipAspect.Height));
    PopupMenu3.Popup(LPoint.X, LPoint.Y);

end;

procedure TfrmCut.palRenderResize(Sender: TObject);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerResize(FPlayer);
    end;
end;

procedure TfrmCut.spCropHeightChange(Sender: TObject);
begin
    if FChanging then Exit;
    if spCropHeight.Text = '' then Exit;

    FChanging := TRUE;
    FCropRect.Height := spCropHeight.Value;
    if FCropRect.Y + FCropRect.Height > FVideoHeight then
    begin
        FCropRect.Height := FVideoHeight - FCropRect.Y;
    end;

    vcVideoPlayerCrop(FPlayer, FCropRect);
    SetCropValues();
    FChanging := FALSE;

end;

procedure TfrmCut.spCropLeftChange(Sender: TObject);
begin
    if FChanging then Exit;
    if spCropLeft.Text = '' then Exit;

    FChanging := TRUE;
    FCropRect.X := spCropLeft.Value;
    if FCropRect.X + FCropRect.Width > FVideoWidth then
    begin
        FCropRect.Width := FVideoWidth - FCropRect.X;
    end;

    vcVideoPlayerCrop(FPlayer, FCropRect);
    SetCropValues();
    FChanging := FALSE;

end;

procedure TfrmCut.spCropTopChange(Sender: TObject);
begin
    if FChanging then Exit;
    if spCropTop.Text = '' then Exit;

    FChanging := TRUE;
    FCropRect.Y := spCropTop.Value;
    if FCropRect.Y + FCropRect.Height > FVideoHeight then
    begin
        FCropRect.Height := FVideoHeight - FCropRect.Y;
    end;

    vcVideoPlayerCrop(FPlayer, FCropRect);
    SetCropValues();
    FChanging := FALSE;

end;

procedure TfrmCut.spCropWidthChange(Sender: TObject);
begin
    if FChanging then Exit;
    if spCropWidth.Text = '' then Exit;

    FChanging := TRUE;
    FCropRect.Width := spCropWidth.Value;
    if FCropRect.X + FCropRect.Width > FVideoWidth then
    begin
        FCropRect.Width := FVideoWidth - FCropRect.X;
    end;

    vcVideoPlayerCrop(FPlayer, FCropRect);
    SetCropValues();
    FChanging := FALSE;

end;

procedure TfrmCut.tbSeekChange(Sender: TObject);
var
    Pos: Integer;
begin
    Pos := tbSeek.Position;
    SetTimeInfo(Pos);
    vcVideoPlayerSeekTo(FPlayer, Pos * AV_TIME_BASE_LL);
    OnPause();
    btnTrimStart.Enabled := (Pos > 0) and (Pos < FDuration);
    btnTrimStop.Enabled := (Pos > 0) and (Pos < FDuration);
end;

procedure TfrmCut.Pause();
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmCut.OnCropChanged(var msg: TMessage);
begin
    FChanging := TRUE;
    FCropRect.X := msg.WParam and $FFFF;
    FCropRect.Y := (msg.WParam shr 16) and $FFFF;
    FCropRect.Width := msg.LParam and $FFFF;
    FCropRect.Height := (msg.LParam shr 16) and $FFFF;
    SetCropValues();
    FChanging := FALSE;
end;

procedure TfrmCut.OnPause();
begin
    if FPlaying then
    begin
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmCut.btnCropResetClick(Sender: TObject);
begin
    if FChanging then Exit;
    FChanging := TRUE;
    FCropRect.X := 0;
    FCropRect.Y := 0;
    FCropRect.Width := 0;
    FCropRect.Height := 0;

    SetCropValues();
    FChanging := FALSE;

end;

procedure TfrmCut.btnNextFrameClick(Sender: TObject);
begin
    vcVideoPlayerNextFrame(FPlayer);
end;

procedure TfrmCut.btnNextTimeClick(Sender: TObject);
begin
    vcVideoPlayerNextTime(FPlayer);
end;

procedure TfrmCut.btnPreTimeClick(Sender: TObject);
begin
    vcVideoPlayerPreTime(FPlayer);
end;

procedure TfrmCut.btnPlayClick(Sender: TObject);
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

procedure TfrmCut.InitForm(Item: PVCItem);
begin
    FChanging := TRUE;
    FItem := Item;
    vcGetVideoEffect(FItem, IID_Crop, @FCropRect, sizeof(FCropRect));

    Self.HandleNeeded;
    Self.Caption := Item.m_MediaInfo.m_szFileName;
    FPlayer := vcCreateVideoPlayer(palRender.Handle, Self.Handle, Item, VC_VIDEO_RENDER_MODE_SOURCE);
    FDuration := (Item.m_MediaInfo.m_Duration + AV_TIME_BASE_LL - 1) div AV_TIME_BASE_LL;
    StatusBar1.Panels[3].Text := GetDurationString(FDuration);
    tbSeek.Max := FDuration;

    FVideoWidth := Item.m_MediaInfo.m_Width;
    FVideoHeight := Item.m_MediaInfo.m_Height;
    spCropTop.MinValue := 0;
    spCropHeight.MinValue := 16;
    spCropLeft.MinValue := 0;
    spCropWidth.MinValue := 16;
    spCropTop.MaxValue := FVideoHeight - 16;
    spCropHeight.MaxValue := FVideoHeight;
    spCropLeft.MaxValue := FVideoWidth - 16;
    spCropWidth.MaxValue := FVideoWidth;

    ShowMediaStartStop();
    SetCropValues();
    FChanging := FALSE;
end;

procedure TfrmCut.OnProgress(var msg: TMessage);
begin
    SetTimeInfo(msg.LParam);
end;

procedure TfrmCut.PageControl1Change(Sender: TObject);
begin
    if FPlayer = nil then Exit;
    if PageControl1.ActivePageIndex = 1 then
    begin
        vcVideoPlayerCrop(FPlayer, FCropRect);
    end else begin
        vcVideoPlayerCropStop(FPlayer);
    end;
end;

procedure TfrmCut.OnPlayEnd(var msg: TMessage);
begin
    FPlaying := FALSE;
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmCut.SetTimeInfo(sec: Integer);
begin
    tbSeek.Position := sec;
    StatusBar1.Panels[1].Text := GetDurationString(sec);
    btnTrimStart.Enabled := (sec > 0) and (sec < FDuration);
    btnTrimStop.Enabled := (sec > 0) and (sec < FDuration);
end;

end.
