unit Subtitle;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.Menus,
    Vcl.StdCtrls, GraphicsButton, Vcl.ComCtrls, MediaPlayTrackBar,
    Vcl.Buttons, GraphicsSpeedButton,
    VideoConverterInt, SubtitleInt;

type
    TfrmSubtitle = class(TForm)
        Panel1: TPanel;
        Panel2: TPanel;
        ListView1: TListView;
        Panel3: TPanel;
        btnEdit: TGraphicsSpeedButton;
        PopupMenu1: TPopupMenu;
        miImport: TMenuItem;
        miAdd: TMenuItem;
        miDelete: TMenuItem;
        N1: TMenuItem;
        miEdit: TMenuItem;
        btnAdd: TGraphicsSpeedButton;
        btnImport: TGraphicsSpeedButton;
        btnDelete: TGraphicsSpeedButton;
        palPlayer: TPanel;
        palCosole: TPanel;
        btnPlay: TGraphicsSpeedButton;
        tbSeek: TMediaPlayTrackBar;
        btnSetStart: TGraphicsSpeedButton;
        btnSetStop: TGraphicsSpeedButton;
        OpenDialog1: TOpenDialog;
        lblTime: TLabel;
        btnDefault: TGraphicsSpeedButton;
        procedure btnEditClick(Sender: TObject);
        procedure FormCreate(Sender: TObject);
        procedure btnImportClick(Sender: TObject);
        procedure ListView1SelectItem(Sender: TObject; Item: TListItem; Selected: Boolean);
        procedure ListView1MouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
        procedure miImportClick(Sender: TObject);
        procedure miEditClick(Sender: TObject);
        procedure miAddClick(Sender: TObject);
        procedure btnAddClick(Sender: TObject);
        procedure miDeleteClick(Sender: TObject);
        procedure btnDeleteClick(Sender: TObject);
        procedure btnPlayClick(Sender: TObject);
        procedure tbSeekChange(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure ListView1CustomDrawItem(Sender: TCustomListView; Item: TListItem; State: TCustomDrawState; var DefaultDraw: Boolean);
        procedure btnDefaultClick(Sender: TObject);
    procedure palPlayerResize(Sender: TObject);
    private
        { Private declarations }
        FItem        : PVCItem;
        FSubtitle    : PSubtitle;
        FSubtitleItem: PSubtitleItem;
        FPlayer      : Pointer;
        FPlaying     : Boolean;
        FDuration    : String;
        FMaxTime     : Int64;
        procedure CreateParams(var Params: TCreateParams); override;
        procedure SetTimeInfo(time: Int64);
        procedure Pause();
        procedure OnPause();
        procedure OnProgress(var msg: TMessage); message VC_VIDEO_RENDER_PROGRESS;
        procedure OnPlayEnd(var msg: TMessage); message VC_VIDEO_RENDER_END;
    public
        { Public declarations }
        procedure LoadSubtitleItem();
        procedure ReloadSubtitleItem();
        procedure InitForm(Item: PVCItem);
    end;

implementation

{$R *.dfm}


uses
    ImageResource, Defines, Functions, SubtitleEdit, Main, SubtitleSettings;

procedure TfrmSubtitle.LoadSubtitleItem();
var
    i     : Integer;
    lvItem: TListItem;
begin
    ListView1.Clear();
    if FSubtitle = nil then exit;
    if FSubtitle.m_Count = 0 then exit;
    for i := 0 to FSubtitle.m_Count - 1 do
    begin
        lvItem := ListView1.Items.Add();
        lvItem.Caption := GetAVTimeString(FSubtitle.m_Items[i].m_StartTime);
        lvItem.SubItems.Add(GetAVTimeString(FSubtitle.m_Items[i].m_Duration));
        lvItem.SubItems.Add(FSubtitle.m_Items[i].m_Lines[0].m_Title);
    end;
    if FSubtitleItem <> nil then
    begin
        ListView1.Items[FSubtitleItem.m_index].Selected := TRUE;
    end else begin
        ListView1.Items[0].Selected := TRUE;
    end;
end;

procedure TfrmSubtitle.ReloadSubtitleItem();
var
    index: Integer;
begin
    index := FSubtitleItem.m_index;
    SubtitleSort(FSubtitle);
    if FSubtitleItem.m_index = index then exit;
    LoadSubtitleItem();
end;

procedure TfrmSubtitle.miAddClick(Sender: TObject);
begin
    btnAddClick(nil);
end;

procedure TfrmSubtitle.miDeleteClick(Sender: TObject);
begin
    btnDeleteClick(nil);
end;

procedure TfrmSubtitle.miEditClick(Sender: TObject);
begin
    btnEditClick(nil);
end;

procedure TfrmSubtitle.miImportClick(Sender: TObject);
begin
    btnImportClick(nil);
end;

procedure TfrmSubtitle.SetTimeInfo(time: Int64);
begin
    lblTime.Caption := GetAVTimeString(time) + '/' + FDuration;
end;

procedure TfrmSubtitle.tbSeekChange(Sender: TObject);
var
    Pos: Integer;
begin
    Pos := tbSeek.Position;
    SetTimeInfo(Pos * AV_TIME_BASE_LL);
    vcVideoPlayerSeekTo(FPlayer, Pos * AV_TIME_BASE_LL);
    OnPause();
end;

procedure TfrmSubtitle.btnAddClick(Sender: TObject);
var
    frmSubtitleEdit: TfrmSubtitleEdit;
begin
    Pause();

    FSubtitleItem := SubtitleAddItem(FSubtitle);
    frmSubtitleEdit := TfrmSubtitleEdit.Create(self);
    frmSubtitleEdit.InitForm(FSubtitleItem);
    frmSubtitleEdit.ShowModal();
    frmSubtitleEdit.Free();

    if FSubtitleItem.m_Count = 0 then
    begin
        SubtitleDelete(FSubtitle, FSubtitleItem.m_index);
        FSubtitleItem := nil;
    end else begin
        ReloadSubtitleItem();
    end;
end;

procedure TfrmSubtitle.btnDefaultClick(Sender: TObject);
var
    frmSubtitleSettings: TfrmSubtitleSettings;
begin
    Pause();
    frmSubtitleSettings := TfrmSubtitleSettings.Create(self);
    frmSubtitleSettings.InitForm(FItem);
    frmSubtitleSettings.ShowModal();
    frmSubtitleSettings.Free();
end;

procedure TfrmSubtitle.btnDeleteClick(Sender: TObject);
begin
    if FSubtitleItem = nil then exit;
    Pause();
    SubtitleDelete(FSubtitle, FSubtitleItem.m_index);
    ListView1.DeleteSelected;
    FSubtitleItem := nil;
end;

procedure TfrmSubtitle.btnEditClick(Sender: TObject);
var
    frmSubtitleEdit: TfrmSubtitleEdit;
begin
    Pause();
    frmSubtitleEdit := TfrmSubtitleEdit.Create(self);
    frmSubtitleEdit.InitForm(FSubtitleItem);
    frmSubtitleEdit.ShowModal();
    frmSubtitleEdit.Free();
    ReloadSubtitleItem();
end;

procedure TfrmSubtitle.btnImportClick(Sender: TObject);
var
    Subtitle: PSubtitle;
begin
    if OpenDialog1.Execute(self.Handle) = FALSE then exit;
    Subtitle := SubtitleImport(FItem, PWideChar(WideString(OpenDialog1.FileName)));
    if Subtitle = nil then
    begin
        Application.MessageBox('This file format is not supported.', PWideChar(APP_TITLE), MB_ICONERROR);
        exit;
    end;
    FSubtitle := Subtitle;
    FSubtitleItem := nil;
    LoadSubtitleItem();
end;

procedure TfrmSubtitle.btnPlayClick(Sender: TObject);
begin
    if FPlaying then
    begin
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
        vcVideoPlayerPause(FPlayer);
    end else begin
        FPlaying := TRUE;
        btnPlay.SetImage(imagePause);
        vcVideoPlayerResume(FPlayer);
    end;
end;

procedure TfrmSubtitle.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.WndParent := 0;
    Params.ExStyle := Params.ExStyle or WS_EX_APPWINDOW;
end;

procedure TfrmSubtitle.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerDestroy(FPlayer);
        FPlayer := nil;
    end;
    frmMain.Show();
    Action := caFree;
end;

procedure TfrmSubtitle.FormCreate(Sender: TObject);
begin
    OpenDialog1.Filter := SUBTITLE_FILE_FILTER;
    btnPlay.SetImage(imagePlay);
    btnEdit.SetImage(imageEditSubtitle);
    btnAdd.SetImage(imageAddSubtitle);
    btnDelete.SetImage(imageDelete);
    btnImport.SetImage(imageImport);
    btnDefault.SetImage(imageSettings);
    btnImport.Flat := FALSE;
end;

procedure TfrmSubtitle.InitForm(Item: PVCItem);
begin
    self.HandleNeeded;
    FItem := Item;
    FSubtitle := SubtitleGet(FItem);
    FMaxTime := Item.m_MediaInfo.m_Duration;
    FDuration := GetAVTimeString(FMaxTime);
    tbSeek.Max := (FMaxTime + AV_TIME_BASE_LL - 1) div AV_TIME_BASE_LL;
    FPlayer := vcCreateVideoPlayer(palPlayer.Handle, self.Handle, FItem, VC_VIDEO_RENDER_MODE_DEST);
    LoadSubtitleItem();
end;

procedure TfrmSubtitle.ListView1CustomDrawItem(Sender: TCustomListView; Item: TListItem; State: TCustomDrawState; var DefaultDraw: Boolean);
const
    AlternatColor = $FAF6F3;
    WhiteColor    = $FFFFFF;
    TextColor     = $000000;
    GrayTextColor = $808080;
var
    i: PSubtitleItem;
begin
    DefaultDraw := TRUE;
    if Odd(Item.index) then
    begin
        ListView1.Canvas.Brush.Color := AlternatColor;
    end else
    begin
        ListView1.Canvas.Brush.Color := WhiteColor;
    end;

    if Item.index >= FSubtitle.m_Count then exit;
    i := FSubtitle.m_Items[Item.index];
    if (i.m_StartTime > 0) and (i.m_StartTime <= FMaxTime) then
    begin
        ListView1.Canvas.Font.Color := TextColor;
    end else begin
        ListView1.Canvas.Font.Color := GrayTextColor;
    end;

end;

procedure TfrmSubtitle.ListView1MouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
    MousePos: TPoint;
begin
    if Button <> mbRight then exit;
    if ListView1.Selected = nil then exit;
    MousePos := Mouse.CursorPos;
    PopupMenu1.Popup(MousePos.X, MousePos.Y);
end;

procedure TfrmSubtitle.ListView1SelectItem(Sender: TObject; Item: TListItem; Selected: Boolean);
begin
    if Selected then
    begin
        FSubtitleItem := FSubtitle.m_Items[Item.index];
        btnEdit.Enabled := TRUE;
        btnDelete.Enabled := TRUE;
        if (FSubtitleItem.m_StartTime > 0) and (FSubtitleItem.m_StartTime <= FMaxTime) then
        begin
            vcVideoPlayerSeekTo(FPlayer, FSubtitleItem.m_StartTime + 1000);
        end;
    end else begin
        FSubtitleItem := nil;
        btnEdit.Enabled := FALSE;
        btnDelete.Enabled := FALSE;
    end;
end;

procedure TfrmSubtitle.Pause();
begin
    if FPlaying then
    begin
        vcVideoPlayerPause(FPlayer);
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmSubtitle.OnPause();
begin
    if FPlaying then
    begin
        FPlaying := FALSE;
        btnPlay.SetImage(imagePlay);
    end;
end;

procedure TfrmSubtitle.OnPlayEnd(var msg: TMessage);
begin
    FPlaying := FALSE;
    btnPlay.SetImage(imagePlay);
end;

procedure TfrmSubtitle.OnProgress(var msg: TMessage);
begin
    tbSeek.Position := msg.LParam;
    SetTimeInfo(msg.LParam * AV_TIME_BASE_LL + msg.WParam);
end;

procedure TfrmSubtitle.palPlayerResize(Sender: TObject);
begin
    if FPlayer <> nil then
    begin
        vcVideoPlayerResize(FPlayer);
    end;
end;

end.
