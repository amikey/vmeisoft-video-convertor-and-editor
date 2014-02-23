unit SubtitleEdit;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.StdCtrls,
    Vcl.Mask, Vcl.ExtCtrls, Vcl.Buttons, GraphicsCheckButton, GraphicsButton,
    VideoConverterInt, SubtitleInt, TimeEdit, GraphicsSpeedButton,
    ColorToolBar;

type
    TfrmSubtitleEdit = class(TForm)
        FontDialog1: TFontDialog;
        PageControl1: TPageControl;
        TabSheet1: TTabSheet;
        Panel1: TPanel;
        palRender1: TPanel;
        TabSheet4: TTabSheet;
        lblHorizontal: TLabel;
        lblVertical: TLabel;
        lbLines: TListBox;
        PageControl2: TPageControl;
        TabSheet5: TTabSheet;
        TabSheet6: TTabSheet;
        TabSheet7: TTabSheet;
        lblSpeedEnter: TLabel;
        lblEffectEnter: TLabel;
        cmbEffectEnter: TComboBox;
        cmbSpeedEnter: TComboBox;
        TabSheet2: TTabSheet;
        lblEffectExit: TLabel;
        cmbEffectExit: TComboBox;
        lblSpeedExit: TLabel;
        cmbSpeedExit: TComboBox;
        TabSheet3: TTabSheet;
        cmbHorizontal: TComboBox;
        cmbVertical: TComboBox;
        cmbAlignment: TComboBox;
        lblAlignment: TLabel;
        TimeEditStart: TTimeEdit;
        TimeEditDuration: TTimeEdit;
        lblFont: TLabel;
        btnFont: TButton;
        btnEditLine: TGraphicsButton;
        btnDeleteLine: TGraphicsButton;
        lblFontColor: TLabel;
        lblBorderColor: TLabel;
        btnAddLine: TGraphicsButton;
        cmbDirectExit: TComboBox;
        cmbDirectEnter: TComboBox;
        palRender: TPanel;
        Panel3: TPanel;
        TabSheet8: TTabSheet;
        TabSheet9: TTabSheet;
        btnPlay: TGraphicsSpeedButton;
        ctbFont: TColorToolBar;
        ctbBorder: TColorToolBar;
        tbFont: TTrackBar;
        tbBorder: TTrackBar;
        lblTransparencyFont: TLabel;
        lblTransparencyBorder: TLabel;
        ckbEnterOnce: TCheckBox;
        ckbExitOnce: TCheckBox;
        lblShadowColor: TLabel;
        ctbShadow: TColorToolBar;
        lblTransparencyShadow: TLabel;
        tbShadow: TTrackBar;
        procedure FormCreate(Sender: TObject);
        procedure lbLinesClick(Sender: TObject);
        procedure btnEditLineClick(Sender: TObject);
        procedure FormClose(Sender: TObject; var Action: TCloseAction);
        procedure btnPlayClick(Sender: TObject);
        procedure cmbHorizontalChange(Sender: TObject);
        procedure cmbVerticalChange(Sender: TObject);
        procedure TimeEditDurationChange(Sender: TObject; time: Int64);
        procedure TimeEditStartChange(Sender: TObject; time: Int64);
        procedure btnFontClick(Sender: TObject);
        procedure cmbEffectEnterChange(Sender: TObject);
        procedure ctbFontColorChanged(Sender: TColorToolBar; Color: Cardinal);
        procedure ctbFontDropdown(Sender: TColorToolBar);
        procedure ctbBorderDropdown(Sender: TColorToolBar);
        procedure ctbBorderColorChanged(Sender: TColorToolBar; Color: Cardinal);
        procedure tbFontChange(Sender: TObject);
        procedure tbBorderChange(Sender: TObject);
        procedure cmbSpeedEnterChange(Sender: TObject);
        procedure cmbDirectEnterChange(Sender: TObject);
        procedure cmbEffectExitChange(Sender: TObject);
        procedure cmbDirectExitChange(Sender: TObject);
        procedure cmbSpeedExitChange(Sender: TObject);
        procedure cmbAlignmentChange(Sender: TObject);
        procedure btnAddLineClick(Sender: TObject);
        procedure btnDeleteLineClick(Sender: TObject);
        procedure ckbEnterOnceClick(Sender: TObject);
        procedure ckbExitOnceClick(Sender: TObject);
        procedure ctbShadowColorChanged(Sender: TColorToolBar;
          Color: Cardinal);
        procedure ctbShadowDropdown(Sender: TColorToolBar);
    procedure tbShadowChange(Sender: TObject);
    private
        { Private declarations }
        FSubtitleItem: PSubtitleItem;
        FSubtitleLine: PSubtitleLine;
        FSelectLine  : Integer;
        FPlayer      : Pointer;
        FChanging    : Boolean;
        procedure LoadLines();
        procedure LoadLineOptions();
        procedure OnPlayEnd(var msg: TMessage); message CM_SUBTITLE_PLAYER_END;
    public
        { Public declarations }
        procedure InitForm(Item: PSubtitleItem);
    end;

implementation

{$R *.dfm}


uses
    Functions, SubtitleText, ImageResource;

procedure TfrmSubtitleEdit.LoadLines();
var
    i: Integer;
begin
    lbLines.Clear();
    for i := 0 to FSubtitleItem.m_Count - 1 do
    begin
        lbLines.Items.Add(FSubtitleItem.m_Lines[i].m_Title);
    end;
end;

procedure TfrmSubtitleEdit.OnPlayEnd(var msg: TMessage);
begin
    btnPlay.Enabled := TRUE;
end;

procedure TfrmSubtitleEdit.tbBorderChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_BorderColor.A := $FF - tbBorder.Position;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.tbFontChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_FontColor.A := $FF - tbFont.Position;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.tbShadowChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_ShadowColor.A := $FF - tbShadow.Position;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.TimeEditDurationChange(Sender: TObject; time: Int64);
begin
    if FChanging then Exit;
    FSubtitleItem.m_Duration := time;
end;

procedure TfrmSubtitleEdit.TimeEditStartChange(Sender: TObject;
  time: Int64);
begin
    if FChanging then Exit;
    FSubtitleItem.m_StartTime := time;
end;

procedure TfrmSubtitleEdit.LoadLineOptions();
var
    bEnabled: Boolean;
begin
    if FSubtitleLine = nil then
    begin
        bEnabled := FALSE;

        lblFont.Caption := '';

        ctbFont.SelectedColor := 0;
        ctbBorder.SelectedColor := 0;
        ctbShadow.SelectedColor := 0;
        tbFont.Position := 0;
        tbBorder.Position := 0;
        tbShadow.Position := 0;

        cmbEffectEnter.ItemIndex := -1;
        cmbDirectEnter.ItemIndex := -1;
        cmbSpeedEnter.ItemIndex := -1;

        cmbEffectExit.ItemIndex := -1;
        cmbDirectExit.ItemIndex := -1;
        cmbSpeedExit.ItemIndex := -1;

        cmbAlignment.ItemIndex := -1;

    end else begin
        bEnabled := TRUE;

        lblFont.Caption := GetFontString(FSubtitleLine.m_FontName, FSubtitleLine.m_FontSize, FSubtitleLine.m_FontStyle);

        ctbFont.SelectedColor := FSubtitleLine.m_FontColor.Color;
        ctbBorder.SelectedColor := FSubtitleLine.m_BorderColor.Color;
        ctbShadow.SelectedColor := FSubtitleLine.m_ShadowColor.Color;
        tbFont.Position := $FF - FSubtitleLine.m_FontColor.A;
        tbBorder.Position := $FF - FSubtitleLine.m_FontColor.A;
        tbShadow.Position := $FF - FSubtitleLine.m_ShadowColor.A;

        cmbEffectEnter.ItemIndex := GetSubtitleEffectIndex(FSubtitleLine.m_Entrance);
        cmbDirectEnter.ItemIndex := FSubtitleLine.m_EntranceParam;
        cmbSpeedEnter.ItemIndex := GetSubtitleSpeedIndex(FSubtitleLine.m_EntranceTime);

        cmbEffectExit.ItemIndex := GetSubtitleEffectIndex(FSubtitleLine.m_Exit);
        cmbDirectExit.ItemIndex := FSubtitleLine.m_ExitParam;
        cmbSpeedExit.ItemIndex := GetSubtitleSpeedIndex(FSubtitleLine.m_ExitTime);

        cmbAlignment.ItemIndex := FSubtitleLine.m_Alignment;

    end;

    btnEditLine.Enabled := bEnabled;
    btnAddLine.Enabled := bEnabled;

    lblFont.Enabled := bEnabled;
    btnFont.Enabled := bEnabled;

    ctbFont.Enabled := bEnabled;
    ctbBorder.Enabled := bEnabled;
    ctbShadow.Enabled := bEnabled;
    tbFont.Enabled := bEnabled;
    tbBorder.Enabled := bEnabled;
    tbShadow.Enabled := bEnabled;

    cmbAlignment.Enabled := bEnabled;

    cmbEffectEnter.Enabled := bEnabled;
    cmbDirectEnter.Visible := bEnabled and (cmbEffectEnter.ItemIndex = 2);
    cmbSpeedEnter.Enabled := bEnabled and (cmbEffectEnter.ItemIndex <> 0);
    cmbEffectExit.Enabled := bEnabled;
    cmbDirectExit.Visible := bEnabled and (cmbEffectExit.ItemIndex = 2);
    cmbSpeedExit.Enabled := bEnabled and (cmbEffectExit.ItemIndex <> 0);

end;

procedure TfrmSubtitleEdit.btnAddLineClick(Sender: TObject);
var
    frmSubtitleText: TfrmSubtitleText;
begin
    frmSubtitleText := TfrmSubtitleText.Create(self);
    frmSubtitleText.EditText.Text := '';
    if (frmSubtitleText.ShowModal() = mrOK) and (frmSubtitleText.EditText.Text <> '') then
    begin
        FSubtitleLine := SubtitleAddLine(FSubtitleItem);
        FSelectLine := FSubtitleLine.m_index;
        lstrcpy(FSubtitleLine.m_Title, PWideChar(WideString(frmSubtitleText.EditText.Text)));
        SubtitleSetFocus(FPlayer, FSelectLine);
        SubtitleRedraw(FPlayer);
        FChanging := TRUE;
        lbLines.ItemIndex := FSubtitleLine.m_index;
        LoadLineOptions();
        btnEditLine.Enabled := TRUE;
        btnDeleteLine.Enabled := TRUE;
        FChanging := FALSE;
    end;
    frmSubtitleText.Free();
end;

procedure TfrmSubtitleEdit.btnDeleteLineClick(Sender: TObject);
begin
    if FSelectLine = -1 then Exit;
    SubtitleDeleteLine(FSubtitleItem, FSelectLine);
    if FSelectLine < FSubtitleItem.m_Count then
    begin
        FSubtitleLine := FSubtitleItem.m_Lines[FSelectLine];
    end else begin
        FSelectLine := FSelectLine - 1;
        if FSelectLine = -1 then
        begin
            FSubtitleLine := nil;
            btnEditLine.Enabled := FALSE;
            btnDeleteLine.Enabled := FALSE;
        end else
        begin
            FSubtitleLine := FSubtitleItem.m_Lines[FSelectLine];
        end;
    end;
    SubtitleRedraw(FPlayer);
    FChanging := TRUE;
    lbLines.ItemIndex := FSelectLine;
    LoadLineOptions();
    FChanging := FALSE;
end;

procedure TfrmSubtitleEdit.btnEditLineClick(Sender: TObject);
var
    frmSubtitleText: TfrmSubtitleText;
begin
    frmSubtitleText := TfrmSubtitleText.Create(self);
    frmSubtitleText.EditText.Text := FSubtitleLine.m_Title;
    if (frmSubtitleText.ShowModal() = mrOK) and (frmSubtitleText.EditText.Text <> FSubtitleLine.m_Title) then
    begin
        lstrcpy(FSubtitleLine.m_Title, PWideChar(WideString(frmSubtitleText.EditText.Text)));
        SubtitleRedraw(FPlayer);
    end;
    frmSubtitleText.Free();
end;

procedure TfrmSubtitleEdit.btnFontClick(Sender: TObject);
var
    font: TFont;
begin
    font := FontDialog1.font;
    font.Name := FSubtitleLine.m_FontName;
    font.Size := FSubtitleLine.m_FontSize;
    font.Style := FontStyleFromMask(FSubtitleLine.m_FontStyle);
    font.Color := BGRToTColor(FSubtitleLine.m_FontColor);
    if FontDialog1.Execute(self.Handle) = FALSE then Exit;
    lstrcpy(FSubtitleLine.m_FontName, PWideChar(WideString(font.Name)));
    FSubtitleLine.m_FontSize := font.Size;
    FSubtitleLine.m_FontStyle := FontStyleToMask(font.Style);
    TColorToBGR(FSubtitleLine.m_FontColor, font.Color);
    ctbFont.SelectedColor := FSubtitleLine.m_FontColor.Color;
    lblFont.Caption := GetFontString(FSubtitleLine.m_FontName, FSubtitleLine.m_FontSize, FSubtitleLine.m_FontStyle);
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.btnPlayClick(Sender: TObject);
begin
    btnPlay.Enabled := FALSE;
    SubtitlePlay(FPlayer);
end;

procedure TfrmSubtitleEdit.ckbEnterOnceClick(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_EntranceOnce := ckbEnterOnce.Checked;
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, FALSE);
end;

procedure TfrmSubtitleEdit.ckbExitOnceClick(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_ExitOnce := ckbExitOnce.Checked;
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, TRUE);
end;

procedure TfrmSubtitleEdit.cmbAlignmentChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_Alignment := cmbAlignment.ItemIndex;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.cmbDirectEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_EntranceParam := cmbDirectEnter.ItemIndex;
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, FALSE);
end;

procedure TfrmSubtitleEdit.cmbDirectExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_ExitParam := cmbDirectExit.ItemIndex;
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, TRUE);
end;

procedure TfrmSubtitleEdit.cmbEffectEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    case cmbEffectEnter.ItemIndex of
        0:
            begin
                FSubtitleLine.m_Entrance := IID_Subtitle_None;
                cmbDirectEnter.Visible := FALSE;
                cmbSpeedEnter.Enabled := FALSE;
                SubtitleRedraw(FPlayer);
            end;
        1:
            begin
                FSubtitleLine.m_Entrance := IID_Subtitle_Fade;
                cmbDirectEnter.Visible := FALSE;
                cmbSpeedEnter.Enabled := TRUE;
                SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, FALSE);
            end;
        2:
            begin
                FSubtitleLine.m_Entrance := IID_Subtitle_Slide;
                cmbDirectEnter.Visible := TRUE;
                cmbSpeedEnter.Enabled := TRUE;
                SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, FALSE);
            end;
    end;
end;

procedure TfrmSubtitleEdit.cmbEffectExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    case cmbEffectExit.ItemIndex of
        0:
            begin
                FSubtitleLine.m_Exit := IID_Subtitle_None;
                cmbDirectExit.Visible := FALSE;
                cmbSpeedExit.Enabled := FALSE;
                SubtitleRedraw(FPlayer);
            end;
        1:
            begin
                FSubtitleLine.m_Exit := IID_Subtitle_Fade;
                cmbDirectExit.Visible := FALSE;
                cmbSpeedExit.Enabled := TRUE;
                SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, TRUE);
            end;
        2:
            begin
                FSubtitleLine.m_Exit := IID_Subtitle_Slide;
                cmbDirectExit.Visible := TRUE;
                cmbSpeedExit.Enabled := TRUE;
                SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, TRUE);
            end;
    end;
end;

procedure TfrmSubtitleEdit.cmbHorizontalChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleItem.m_HPosition := cmbHorizontal.ItemIndex;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.cmbSpeedEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_EntranceTime := SubtitleSpeeds[cmbSpeedEnter.ItemIndex];
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, FALSE);
end;

procedure TfrmSubtitleEdit.cmbSpeedExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleLine.m_ExitTime := SubtitleSpeeds[cmbSpeedExit.ItemIndex];
    SubtitlePlayEx(FPlayer, FSubtitleLine.m_index, TRUE);
end;

procedure TfrmSubtitleEdit.cmbVerticalChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSubtitleItem.m_VPosition := cmbVertical.ItemIndex;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.ctbBorderColorChanged(Sender: TColorToolBar; Color: Cardinal);
begin
    FSubtitleLine.m_BorderColor.Color := (FSubtitleLine.m_BorderColor.Color and $FF000000) or Color;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.ctbBorderDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbBorder.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSubtitleLine.m_BorderColor.Color);
end;

procedure TfrmSubtitleEdit.ctbFontColorChanged(Sender: TColorToolBar; Color: Cardinal);
begin
    FSubtitleLine.m_FontColor.Color := (FSubtitleLine.m_FontColor.Color and $FF000000) or Color;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.ctbFontDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbFont.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSubtitleLine.m_FontColor.Color);
end;

procedure TfrmSubtitleEdit.ctbShadowColorChanged(Sender: TColorToolBar; Color: Cardinal);
begin
    FSubtitleLine.m_ShadowColor.Color := (FSubtitleLine.m_ShadowColor.Color and $FF000000) or Color;
    SubtitleRedraw(FPlayer);
end;

procedure TfrmSubtitleEdit.ctbShadowDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbShadow.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSubtitleLine.m_ShadowColor.Color);
end;

procedure TfrmSubtitleEdit.FormClose(Sender: TObject; var Action: TCloseAction);
begin
    if FPlayer <> nil then
    begin
        SubtitleDestroyPlayer(FPlayer);
        FPlayer := nil;
    end;
end;

procedure TfrmSubtitleEdit.FormCreate(Sender: TObject);
begin
    PageControl1.ActivePageIndex := 0;
    PageControl2.ActivePageIndex := 0;
    btnPlay.SetImage(imagePlay);
    btnEditLine.SetImage(imageEditSubtitle);
    btnAddLine.SetImage(imageAddSubtitle);
    btnDeleteLine.SetImage(imageDelete);
end;

procedure TfrmSubtitleEdit.InitForm(Item: PSubtitleItem);
begin
    FChanging := TRUE;
    FSubtitleItem := Item;
    FPlayer := SubtitleCreatePlayer(palRender.Handle, self.Handle, FSubtitleItem);
    TimeEditStart.TimeValue := FSubtitleItem.m_StartTime;
    TimeEditDuration.TimeValue := FSubtitleItem.m_Duration;
    cmbHorizontal.ItemIndex := FSubtitleItem.m_HPosition;
    cmbVertical.ItemIndex := FSubtitleItem.m_VPosition;
    LoadLines();
    if FSubtitleItem.m_Count > 0 then
    begin
        FSubtitleLine := FSubtitleItem.m_Lines[0];
        FSelectLine := 0;
        lbLines.ItemIndex := 0;
        btnEditLine.Enabled := TRUE;
        btnDeleteLine.Enabled := TRUE;
    end else
    begin
        FSubtitleLine := nil;
        FSelectLine := -1;
        btnEditLine.Enabled := FALSE;
        btnDeleteLine.Enabled := FALSE;
    end;
    LoadLineOptions();
    FChanging := FALSE;
end;

procedure TfrmSubtitleEdit.lbLinesClick(Sender: TObject);
begin
    FSelectLine := lbLines.ItemIndex;
    btnEditLine.Enabled := FSelectLine <> -1;
    if FSelectLine = -1 then
    begin
        btnEditLine.Enabled := FALSE;
        btnDeleteLine.Enabled := FALSE;
        FSubtitleLine := nil;
        Exit;
    end;
    FChanging := TRUE;
    FSubtitleLine := FSubtitleItem.m_Lines[FSelectLine];
    LoadLineOptions();
    SubtitleSetFocus(FPlayer, FSelectLine);
    FChanging := FALSE;
end;

end.
