unit ItemView;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.Buttons, Vcl.ExtCtrls,
    Vcl.ComCtrls, GraphicsGPImage, Vcl.StdCtrls, Winapi.Shellapi,
    Winapi.GDIPAPI, Winapi.GDIPOBJ,
    VideoConverterInt,
    GraphicsSpeedButton;

type
    TfrmItem = class(TForm)
        ProgressBar1: TProgressBar;
        lblName: TLabel;
        lblInfo: TLabel;
        lblInfo2: TLinkLabel;
        Thumbail: TGraphicsGPImage;
        btnInfo: TGraphicsSpeedButton;
        btnPlay: TGraphicsSpeedButton;
        btnCut: TGraphicsSpeedButton;
        btnEffect: TGraphicsSpeedButton;
        btnRemove: TGraphicsSpeedButton;
        btnRedo: TGraphicsSpeedButton;
        Timer1: TTimer;
        lblTime: TLabel;
        btnSubtitle: TGraphicsSpeedButton;
        btnAudio: TGraphicsSpeedButton;
        procedure FormCreate(Sender: TObject);
        procedure FormMouseEnter(Sender: TObject);
        procedure FormMouseLeave(Sender: TObject);
        procedure FormPaint(Sender: TObject);
        procedure btnInfoClick(Sender: TObject);
        procedure FormDestroy(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure btnCutClick(Sender: TObject);
        procedure btnEffectClick(Sender: TObject);
        procedure btnRemoveClick(Sender: TObject);
        procedure btnRedoClick(Sender: TObject);
        procedure Timer1Timer(Sender: TObject);
        procedure lblInfo2LinkClick(Sender: TObject; const Link: string; LinkType: TSysLinkType);
        procedure btnSubtitleClick(Sender: TObject);
        procedure btnAudioClick(Sender: TObject);
    private
        { Private declarations }
        FInfo      : PMediaInfo;
        FItem      : PVCItem;
        FBitmap    : TGPBitmap;
        FFileName  : String;
        FTimerCount: Integer;
        procedure OnStatChanged(var msg: TMessage); message VC_ITEM_STAT_CHANGED;
        procedure SetStatus(stat: Integer);
        procedure ShowButtons(isVisual: Boolean);
        procedure EnableTime(bEnable: Boolean);
        procedure ImportSubtitle();
    private
        MousIn: Boolean;
    public
        { Public declarations }
        function ImportFile(FileName: String): Boolean;
        function StartConvert(): Boolean;
        function GetItem(): PVCItem;
        procedure WaitForConvert();
    public
        m_Index: Integer;
    end;

var
    g_itemcount: Integer;
    g_itemviews: array [0 .. 1024 - 1] of TfrmItem;

implementation

{$R *.dfm}


uses ImageResource, FileInformation, Main, Functions,
    VideoPlayer, CutMedia, EffectEditor, Defines, Subtitle,
    AudioSettings, SubtitleInt, SubtitleSelection;

function TfrmItem.GetItem(): PVCItem;
begin
    Result := FItem;
end;

procedure TfrmItem.ShowButtons(isVisual: Boolean);
begin
    btnInfo.Visible := isVisual;
    btnPlay.Visible := isVisual;
    btnCut.Visible := isVisual;
    btnEffect.Visible := isVisual;
    btnRemove.Visible := isVisual;
    btnSubtitle.Visible := isVisual;
    btnAudio.Visible := isVisual;
end;

procedure TfrmItem.Timer1Timer(Sender: TObject);
var
    Estimated: Integer;
begin
    if FItem.m_Duration = 0 then exit;
    FTimerCount := FTimerCount + 1;
    ProgressBar1.Position := FItem.m_Time * 100 div FItem.m_Duration;
    if (ProgressBar1.Position > 2) and (FItem.m_Time < FItem.m_Duration) then
    begin
        Estimated := (FTimerCount * FItem.m_Duration div FItem.m_Time) - FTimerCount;
        lblTime.Caption := 'Elapsed:[' + GetDurationString(FTimerCount) + ']; Estimated:[' + GetDurationString(Estimated) + ']';
    end else begin
        lblTime.Caption := 'Elapsed:[' + GetDurationString(FTimerCount) + ']';
    end;

end;

procedure TfrmItem.btnAudioClick(Sender: TObject);
var
    frmAudioSettings: TfrmAudioSettings;
begin
    frmAudioSettings := TfrmAudioSettings.Create(frmMain);
    frmAudioSettings.InitForm(FItem);
    frmMain.Hide();
    frmAudioSettings.Show();
end;

procedure TfrmItem.btnCutClick(Sender: TObject);
var
    frmCut: TfrmCut;
begin
    frmCut := TfrmCut.Create(frmMain);
    frmCut.InitForm(FItem);
    frmMain.Hide();
    frmCut.Show();
end;

procedure TfrmItem.btnEffectClick(Sender: TObject);
var
    frmEffect: TfrmEffect;
begin
    frmEffect := TfrmEffect.Create(frmMain);
    frmEffect.InitForm(FItem);
    frmMain.Hide();
    frmEffect.Show();
end;

procedure TfrmItem.btnInfoClick(Sender: TObject);
var
    frmMediaInfo: TfrmMediaInfo;
begin
    frmMediaInfo := TfrmMediaInfo.Create(frmMain);
    frmMediaInfo.InitForm(FItem.m_MediaInfo);
    frmMediaInfo.ShowModal();
    frmMediaInfo.Free();
end;

procedure TfrmItem.btnPlayClick(Sender: TObject);
var
    frmPlayer: TfrmVideoPlayer;
begin
    frmPlayer := TfrmVideoPlayer.Create(frmMain);
    frmPlayer.InitForm(FItem);
    frmMain.Hide();
    frmPlayer.Show();
end;

procedure TfrmItem.btnRedoClick(Sender: TObject);
begin
    SetStatus(ClipStat_Ready_For_Convert);
end;

procedure TfrmItem.btnRemoveClick(Sender: TObject);
begin
    Self.Free();
end;

procedure TfrmItem.btnSubtitleClick(Sender: TObject);
var
    frmSubtitle: TfrmSubtitle;
begin
    frmSubtitle := TfrmSubtitle.Create(frmMain);
    frmSubtitle.InitForm(FItem);
    frmMain.Hide();
    frmSubtitle.Show();
end;

procedure TfrmItem.FormCreate(Sender: TObject);
var
    t, l, w: Integer;
begin

    l := btnInfo.Left;
    t := btnInfo.Top - 1;
    w := Self.Width - l - 8;

    ProgressBar1.Top := t;
    ProgressBar1.Left := l;
    ProgressBar1.Width := w;
    lblTime.Left := l;
    lblTime.Top := t + ProgressBar1.Height + 2;

    lblInfo2.Top := t;
    lblInfo2.Left := l;
    lblInfo2.Width := w - 27 - 4;

    btnRedo.Top := t - 1;
    btnRedo.Left := l + lblInfo2.Width + 8;

    btnInfo.SetImage(imageInfo);
    btnPlay.SetImage(imagePlay);
    btnCut.SetImage(imageCut);
    btnEffect.SetImage(imageEffect);
    btnRemove.SetImage(imageDelete);
    btnRedo.SetImage(imageRedo);
    btnSubtitle.SetImage(imageSubtitle);
    btnAudio.SetImage(imageAudio);
end;

procedure TfrmItem.FormDestroy(Sender: TObject);
begin
    if Application.Terminated then exit;
    if FBitmap <> nil then FBitmap.Free();
    if FItem <> nil then
    begin
        vcDeleteItem(FItem);
        frmMain.OnItemRemove(m_Index);
    end;
end;

procedure TfrmItem.EnableTime(bEnable: Boolean);
begin
    FTimerCount := 0;
    lblTime.Caption := '';
    Timer1.Enabled := bEnable;
    lblTime.Visible := bEnable;
end;

procedure TfrmItem.SetStatus(stat: Int32);
var
    ErrorString: WideString;
begin
    case stat of
        MediaStat_Importing:
            begin
                lblInfo.Caption := 'Importing';
                lblInfo2.Hide();
                btnRedo.Hide();
                ProgressBar1.Show();
                ShowButtons(False);
                ProgressBar1.Style := pbstMarquee;
                Thumbail.SetImage(imageProcess);
            end;

        MediaStat_Error_File:
            begin
                ProgressBar1.Hide();
                lblInfo.Caption := 'Unsupport this file.';
                ErrorString := 'Unsupport this file.' + #13#10 + FFileName;
                Application.MessageBox(PWideChar(ErrorString), PWideChar(APP_TITLE), MB_ICONERROR);
                Self.Free();
            end;

        MediaStat_Check_Media:
            begin
                if FInfo.m_bVideoStream <> 0 then
                begin
                    ProgressBar1.Style := pbstNormal;
                    lblInfo.Caption := GetMediaString(FItem.m_MediaInfo);
                    EnableTime(True);
                end else begin
                    ProgressBar1.Hide();
                    lblInfo.Caption := 'No Video Stream';
                    ErrorString := 'This file is not a video file.' + #13#10 + FFileName;
                    Application.MessageBox(PWideChar(ErrorString), PWideChar(APP_TITLE), MB_ICONERROR);
                    Self.Free();
                end;

            end;

        MediaStat_Thumbnail_Generated:
            begin
                if FBitmap <> nil then FBitmap.Free();
                FBitmap := LoadThumbailImage(FItem.m_MediaInfo.m_Demo, 96, 72);
                Thumbail.SetImage(FBitmap);
            end;

        MediaStat_Complete:
            begin
                FItem.m_Status := ClipStat_Ready_For_Convert;
                SetStatus(ClipStat_Ready_For_Convert);
                ImportSubtitle();
            end;

        ClipStat_Ready_For_Convert:
            begin
                EnableTime(False);
                lblInfo.Caption := GetMediaString(FItem.m_MediaInfo);
                lblInfo2.Hide();
                btnRedo.Hide();
                ProgressBar1.Hide();
                ShowButtons(True);
            end;

        ClipStat_Wait_For_Convert:
            begin
                ShowButtons(False);
                lblInfo2.Show();
                lblInfo2.Caption := 'Waiting for convert';
            end;

        ClipStat_Start_Convert:
            begin
                lblInfo.Caption := 'Start to convert';
                lblInfo2.Hide();
                ProgressBar1.Show();
                ShowButtons(False);
                ProgressBar1.Style := pbstMarquee;
            end;

        ClipStat_Converting:
            begin
                EnableTime(True);
                lblInfo.Caption := 'Converting';
                ProgressBar1.Style := pbstNormal;
                ProgressBar1.Position := 0;
            end;

        ClipStat_Converted:
            begin
                EnableTime(False);
                lblInfo.Caption := GetMediaString(FItem.m_MediaInfo);
                lblInfo2.Caption := 'Converted:<a>' + FItem.m_OutFile + '</a>';
                lblInfo2.Show();
                btnRedo.Show();
                ProgressBar1.Hide();
            end;

        ClipStat_Failed:
            begin
                EnableTime(False);
                lblInfo.Caption := GetMediaString(FItem.m_MediaInfo);
                lblInfo2.Caption := 'Failed to convert';
                lblInfo2.Show();
                btnRedo.Show();
                ProgressBar1.Hide();
            end;

        ClipStat_Abort:
            begin
                EnableTime(False);
                lblInfo.Caption := GetMediaString(FItem.m_MediaInfo);
                lblInfo2.Caption := 'Abort';
                lblInfo2.Show();
                btnRedo.Show();
                ProgressBar1.Hide();
            end;
    else
        begin
            raise Exception.CreateFmt('Error Status=%d', [Integer(stat)]);
        end;
    end;
end;

procedure TfrmItem.OnStatChanged(var msg: TMessage);
begin
    SetStatus(msg.LParam);
end;

procedure TfrmItem.FormMouseEnter(Sender: TObject);
begin
    if MousIn = False then
    begin
        MousIn := True;
        Self.Invalidate();
    end;
end;

procedure TfrmItem.FormMouseLeave(Sender: TObject);
begin
    if PtInRect(Self.ClientRect, ScreenToClient(Mouse.CursorPos)) = False then
    begin
        MousIn := False;
        Self.Invalidate();
    end;
end;

procedure TfrmItem.FormPaint(Sender: TObject);
var
    Graphics: TGPGraphics;
    Brush   : TGPLinearGradientBrush;
    Pen     : TGPPen;
    pt1     : TGPPoint;
    pt2     : TGPPoint;
const
    colors: Array [0 .. 2] of TGPColor  = ($FFFAFCFD, $FFFFFFFF, $FFE4EFFB);
    Positions: Array [0 .. 2] of Single = (0, 0.5, 1);
begin
    if PtInRect(Self.ClientRect, ScreenToClient(Mouse.CursorPos)) then
    begin
        pt1 := MakePoint(0, 0);
        pt2 := MakePoint(0, Height);
        Graphics := TGPGraphics.Create(Self.Canvas.Handle);
        Brush := TGPLinearGradientBrush.Create(pt1, pt2, 0, 0);
        Brush.SetInterpolationColors(@colors, @Positions, 3);
        Graphics.FillRectangle(Brush, 0, 0, Width, Height);
        Brush.Free();
        Graphics.Free();
    end else
    begin
        Pen := TGPPen.Create($FF808080);
        Graphics := TGPGraphics.Create(Self.Canvas.Handle);
        Graphics.DrawLine(Pen, 0, Height - 1, Width - 1, Height - 1);
        Pen.Free();
        Graphics.Free();
    end;
end;

function TfrmItem.ImportFile(FileName: String): Boolean;
begin
    FFileName := FileName;
    FInfo := vcImportMediaInfo(Self.Handle, PWideChar(WideString(FileName)));
    if FInfo = nil then
    begin
        Result := False;
        exit;
    end;

    FItem := vcCreateItem(Self.Handle, FInfo);
    lblName.Caption := FItem.m_Name;
    SetStatus(MediaStat_Importing);
    Result := True;
end;

procedure TfrmItem.ImportSubtitle();
var
    Count               : Integer;
    frmSubtitleSelection: TfrmSubtitleSelection;
    i                   : Integer;
begin
    Count := SubtitleFind(PWideChar(FFileName));
    if Count = 0 then exit;
    frmSubtitleSelection := TfrmSubtitleSelection.Create(frmMain);
    frmSubtitleSelection.ComboBox1.AddItem('None', nil);
    for i := 0 to Count - 1 do
    begin
        frmSubtitleSelection.ComboBox1.AddItem(SubtitleFindGetName(i), nil);
    end;
    frmSubtitleSelection.ComboBox1.ItemIndex := 1;
    frmSubtitleSelection.ShowModal();
    if frmSubtitleSelection.ComboBox1.ItemIndex <> 0 then
    begin
        SubtitleImport(FInfo, SubtitleFindGetPath(frmSubtitleSelection.ComboBox1.ItemIndex - 1));
    end;
    frmSubtitleSelection.Free();
    SubtitleFindFree();
end;

procedure TfrmItem.lblInfo2LinkClick(Sender: TObject; const Link: string; LinkType: TSysLinkType);
var
    Parameters: WideString;
begin
    Parameters := '/select,' + FItem.m_OutFile;
    ShellExecute(0, nil, 'explorer.exe', PWideChar(Parameters), nil, SW_SHOWNORMAL);
end;

function TfrmItem.StartConvert(): Boolean;
begin
    if FItem.m_Status = ClipStat_Wait_For_Convert then
    begin
        vcStartConvert(FItem);
        FItem.m_Status := ClipStat_Start_Convert;
        SetStatus(ClipStat_Start_Convert);
        Result := True;
    end else begin
        Result := False;
    end;
end;

procedure TfrmItem.WaitForConvert();
begin
    FItem.m_Status := ClipStat_Wait_For_Convert;
    SetStatus(ClipStat_Wait_For_Convert);
end;

end.
