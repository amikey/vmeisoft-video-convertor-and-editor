unit EffectEditor;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.ExtCtrls,
    Vcl.Buttons, GraphicsSpeedButton, MediaPlayTrackBar,
    GraphicsButton, Vcl.StdCtrls, Vcl.Menus, Vcl.Samples.Spin,
    VideoConverterInt, VideoEffectInt;

type
    TfrmEffect = class(TForm)
        palRender: TPanel;
        palConsole: TPanel;
        StatusBar1: TStatusBar;
        tbSeek: TMediaPlayTrackBar;
        PageControl1: TPageControl;
        btnPlay: TGraphicsSpeedButton;
        TabSheet1: TTabSheet;
        TabSheet2: TTabSheet;
        lblBrightness: TLabel;
        tbBrightness: TTrackBar;
        lblContrast: TLabel;
        tbContrast: TTrackBar;
        lblHue: TLabel;
        tbHue: TTrackBar;
        lblSaturation: TLabel;
        tbSaturation: TTrackBar;
        lblLightness: TLabel;
        tbLightness: TTrackBar;
        TabSheet3: TTabSheet;
        btnRotateLeft: TGraphicsButton;
        btnRotateRight: TGraphicsButton;
        btnFlipHorizontal: TGraphicsButton;
        btnFlipVertical: TGraphicsButton;
        lblRotateFlip: TLabel;
        btnReset1: TButton;
        btnReset2: TButton;
        procedure FormCreate(Sender: TObject);
        procedure palRenderResize(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure tbSeekChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure tbBrightnessChange(Sender: TObject);
        procedure tbContrastChange(Sender: TObject);
        procedure tbHueChange(Sender: TObject);
        procedure tbSaturationChange(Sender: TObject);
        procedure tbLightnessChange(Sender: TObject);
        procedure btnRotateLeftClick(Sender: TObject);
        procedure btnRotateRightClick(Sender: TObject);
        procedure btnFlipHorizontalClick(Sender: TObject);
        procedure btnFlipVerticalClick(Sender: TObject);
        procedure btnReset1Click(Sender: TObject);
        procedure btnReset2Click(Sender: TObject);
    private
        { Private declarations }
        FChanging: Boolean;
        FPlayer  : Pointer;
        FPlaying : Boolean;
        FDuration: Integer;
        FItem    : PVCItem;
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

procedure TfrmEffect.btnReset1Click(Sender: TObject);
var
    Params: BrightnessContrastParams;
begin
    FChanging := TRUE;
    tbBrightness.Position := 0;
    tbContrast.Position := 0;
    Params.m_Brightness := 0;
    Params.m_Contrast := 0;
    vcSetVideoEffect(FItem, IID_BrightnessContrast, @Params, sizeof(Params));
    FChanging := FALSE;
end;

procedure TfrmEffect.btnReset2Click(Sender: TObject);
var
    Params: HueSaturationLightnessParams;
begin
    FChanging := TRUE;
    tbHue.Position := 0;
    tbSaturation.Position := 0;
    tbLightness.Position := 0;
    Params.m_Hue := 0;
    Params.m_Saturation := 0;
    Params.m_Lightness := 0;
    vcSetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
    FChanging := FALSE;
end;

procedure TfrmEffect.btnRotateLeftClick(Sender: TObject);
var
    Params: Integer;
begin
    Params := 0;
    vcGetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    Params := CalcRotateFlip(Params, Rotate_Left);
    vcSetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    lblRotateFlip.Caption := RotateFlipString(Params);
end;

procedure TfrmEffect.btnRotateRightClick(Sender: TObject);
var
    Params: Integer;
begin
    if FChanging then exit;
    Params := 0;
    vcGetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    Params := CalcRotateFlip(Params, Rotate_Right);
    vcSetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    lblRotateFlip.Caption := RotateFlipString(Params);
end;

procedure TfrmEffect.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.WndParent := 0;
    Params.ExStyle := Params.ExStyle or WS_EX_APPWINDOW;
end;

procedure TfrmEffect.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerDestroy(FPlayer);
        FPlayer := nil;
    end;
    frmMain.Show();
    Action := caFree;
end;

procedure TfrmEffect.FormCreate(Sender: TObject);
begin
    btnPlay.SetImage(imagePlay);
    btnRotateLeft.SetImage(imageRotateLeft);
    btnRotateRight.SetImage(imageRotateRight);
    btnFlipHorizontal.SetImage(imageFlipHorizontal);
    btnFlipVertical.SetImage(imageFlipVertical);
    PageControl1.ActivePageIndex := 0;
end;

procedure TfrmEffect.palRenderResize(Sender: TObject);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerResize(FPlayer);
    end;
end;

procedure TfrmEffect.tbBrightnessChange(Sender: TObject);
var
    Params: BrightnessContrastParams;
begin
    if FChanging then exit;
    vcGetVideoEffect(FItem, IID_BrightnessContrast, @Params, sizeof(Params));
    Params.m_Brightness := tbBrightness.Position;
    vcSetVideoEffect(FItem, IID_BrightnessContrast, @Params, sizeof(Params));
end;

procedure TfrmEffect.tbContrastChange(Sender: TObject);
var
    Params: BrightnessContrastParams;
begin
    if FChanging then exit;
    vcGetVideoEffect(FItem, IID_BrightnessContrast, @Params, sizeof(Params));
    Params.m_Contrast := tbContrast.Position;
    vcSetVideoEffect(FItem, IID_BrightnessContrast, @Params, sizeof(Params));
end;

procedure TfrmEffect.tbHueChange(Sender: TObject);
var
    Params: HueSaturationLightnessParams;
begin
    if FChanging then exit;
    vcGetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
    Params.m_Hue := tbHue.Position;
    vcSetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
end;

procedure TfrmEffect.tbLightnessChange(Sender: TObject);
var
    Params: HueSaturationLightnessParams;
begin
    if FChanging then exit;
    vcGetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
    Params.m_Lightness := tbLightness.Position;
    vcSetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
end;

procedure TfrmEffect.tbSaturationChange(Sender: TObject);
var
    Params: HueSaturationLightnessParams;
begin
    if FChanging then exit;
    vcGetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
    Params.m_Saturation := tbSaturation.Position;
    vcSetVideoEffect(FItem, IID_HueSaturationLightness, @Params, sizeof(Params));
end;

procedure TfrmEffect.tbSeekChange(Sender: TObject);
var
    Pos: Integer;
begin
    Pos := tbSeek.Position;
    SetTimeInfo(Pos);
    vcVideoPlayerSeekTo(FPlayer, Pos * AV_TIME_BASE_LL);
    OnPause();
end;

procedure TfrmEffect.Pause();
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmEffect.OnPause();
begin
    if FPlaying then
    begin
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmEffect.btnFlipHorizontalClick(Sender: TObject);
var
    Params: Integer;
begin
    if FChanging then exit;
    Params := 0;
    vcGetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    Params := CalcRotateFlip(Params, Flip_Horizontal);
    vcSetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    lblRotateFlip.Caption := RotateFlipString(Params);
end;

procedure TfrmEffect.btnFlipVerticalClick(Sender: TObject);
var
    Params: Integer;
begin
    if FChanging then exit;
    Params := 0;
    vcGetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    Params := CalcRotateFlip(Params, Flip_Vertical);
    vcSetVideoEffect(FItem, IID_RotateFlip, @Params, sizeof(Params));
    lblRotateFlip.Caption := RotateFlipString(Params);
end;

procedure TfrmEffect.btnPlayClick(Sender: TObject);
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

procedure TfrmEffect.InitForm(Item: PVCItem);
var
    Params1: BrightnessContrastParams;
    Params2: HueSaturationLightnessParams;
    Params3: Integer;
begin
    FItem := Item;
    Self.HandleNeeded;
    Self.Caption := Item.m_MediaInfo.m_szFileName;
    FPlayer := vcCreateVideoPlayer(palRender.Handle, Self.Handle, Item, VC_VIDEO_RENDER_MODE_DEST);
    FDuration := (Item.m_MediaInfo.m_Duration + AV_TIME_BASE_LL - 1) div AV_TIME_BASE_LL;
    tbSeek.Max := FDuration;

    vcGetVideoEffect(FItem, IID_BrightnessContrast, @Params1, sizeof(Params1));
    tbBrightness.Position := Params1.m_Brightness;
    tbContrast.Position := Params1.m_Contrast;

    vcGetVideoEffect(FItem, IID_HueSaturationLightness, @Params2, sizeof(Params2));
    tbHue.Position := Params2.m_Hue;
    tbSaturation.Position := Params2.m_Saturation;
    tbLightness.Position := Params2.m_Lightness;

    vcGetVideoEffect(FItem, IID_RotateFlip, @Params3, sizeof(Params3));
    lblRotateFlip.Caption := RotateFlipString(Params3);

end;

procedure TfrmEffect.OnProgress(var msg: TMessage);
begin
    tbSeek.Position := msg.LParam;
    SetTimeInfo(msg.LParam);
end;

procedure TfrmEffect.OnPlayEnd(var msg: TMessage);
begin
    FPlaying := FALSE;
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmEffect.SetTimeInfo(sec: Integer);
begin
    StatusBar1.Panels[1].Text := GetDurationProcString(sec, FDuration);
end;

end.
