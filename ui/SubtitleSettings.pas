unit SubtitleSettings;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.UITypes, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, VideoConverterInt, SubTitleInt,
    Vcl.ExtCtrls, Vcl.StdCtrls, ColorToolBar;

type
    TfrmSubtitleSettings = class(TForm)
        PageControl1: TPageControl;
        TabSheet1: TTabSheet;
        TabSheet2: TTabSheet;
        TabSheet3: TTabSheet;
        TabSheet4: TTabSheet;
        lblHorizontal: TLabel;
        cmbHorizontal: TComboBox;
        lblVertical: TLabel;
        cmbVertical: TComboBox;
        lblFont: TLabel;
        btnFont: TButton;
        lblFontColor: TLabel;
        lblBorderColor: TLabel;
        lblEffectEnter: TLabel;
        cmbEffectEnter: TComboBox;
        cmbDirectEnter: TComboBox;
        cmbSpeedEnter: TComboBox;
        lblSpeedEnter: TLabel;
        lblEffectExit: TLabel;
        lblSpeedExit: TLabel;
        cmbEffectExit: TComboBox;
        cmbSpeedExit: TComboBox;
        cmbDirectExit: TComboBox;
        lblAlignment: TLabel;
        cmbAlignment: TComboBox;
        tbFont: TTrackBar;
        Label1: TLabel;
        Label2: TLabel;
        tbBorder: TTrackBar;
        ctbFont: TColorToolBar;
        ctbBorder: TColorToolBar;
        FontDialog1: TFontDialog;
        ckbExitOnce: TCheckBox;
        ckbEnterOnce: TCheckBox;
        Button1: TButton;
        ctbShadow: TColorToolBar;
        Label3: TLabel;
        Label4: TLabel;
        tbShadow: TTrackBar;
        procedure FormCreate(Sender: TObject);
        procedure Button1Click(Sender: TObject);
        procedure Button2Click(Sender: TObject);
        procedure Button3Click(Sender: TObject);
        procedure Button4Click(Sender: TObject);
        procedure cmbEffectEnterChange(Sender: TObject);
        procedure cmbDirectEnterChange(Sender: TObject);
        procedure cmbSpeedEnterChange(Sender: TObject);
        procedure cmbEffectExitChange(Sender: TObject);
        procedure cmbDirectExitChange(Sender: TObject);
        procedure cmbSpeedExitChange(Sender: TObject);
        procedure btnFontClick(Sender: TObject);
        procedure ctbFontColorChanged(Sender: TColorToolBar; Color: Cardinal);
        procedure ctbFontDropdown(Sender: TColorToolBar);
        procedure ctbBorderColorChanged(Sender: TColorToolBar; Color: Cardinal);
        procedure ctbBorderDropdown(Sender: TColorToolBar);
        procedure tbFontChange(Sender: TObject);
        procedure tbBorderChange(Sender: TObject);
        procedure cmbAlignmentChange(Sender: TObject);
        procedure ckbEnterOnceClick(Sender: TObject);
        procedure ckbExitOnceClick(Sender: TObject);
        procedure ctbShadowColorChanged(Sender: TColorToolBar; Color: Cardinal);
        procedure tbShadowChange(Sender: TObject);
        procedure ctbShadowDropdown(Sender: TColorToolBar);
    private
        { Private declarations }
        FSettings: PSubtitleDefault;
        FItem    : PVCItem;
        FSubtitle: PSubtitle;
        FChanging: Boolean;
    public
        { Public declarations }
        procedure InitForm(Item: PVCItem);
    end;

implementation

{$R *.dfm}


uses
    Functions;

procedure TfrmSubtitleSettings.btnFontClick(Sender: TObject);
var
    font: TFont;
begin
    font := FontDialog1.font;
    font.Name := FSettings.m_FontName;
    font.Size := FSettings.m_FontSize;
    font.Style := FontStyleFromMask(FSettings.m_FontStyle);
    font.Color := BGRToTColor(FSettings.m_FontColor);
    if FontDialog1.Execute(self.Handle) = FALSE then Exit;
    lstrcpy(FSettings.m_FontName, PWideChar(WideString(font.Name)));
    FSettings.m_FontSize := font.Size;
    FSettings.m_FontStyle := FontStyleToMask(font.Style);
    TColorToBGR(FSettings.m_FontColor, font.Color);
    lblFont.Caption := GetFontString(FSettings.m_FontName, FSettings.m_FontSize, FSettings.m_FontStyle);
end;

procedure TfrmSubtitleSettings.Button1Click(Sender: TObject);
begin
    //
end;

procedure TfrmSubtitleSettings.Button2Click(Sender: TObject);
begin
    //
end;

procedure TfrmSubtitleSettings.Button3Click(Sender: TObject);
begin
    //
end;

procedure TfrmSubtitleSettings.Button4Click(Sender: TObject);
begin
    //
end;

procedure TfrmSubtitleSettings.ckbEnterOnceClick(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_EntranceOnce := ckbEnterOnce.Checked;
end;

procedure TfrmSubtitleSettings.ckbExitOnceClick(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_ExitOnce := ckbExitOnce.Checked;
end;

procedure TfrmSubtitleSettings.cmbAlignmentChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_Alignment := cmbAlignment.ItemIndex;
end;

procedure TfrmSubtitleSettings.cmbDirectEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_EntranceParam := cmbDirectEnter.ItemIndex;
end;

procedure TfrmSubtitleSettings.cmbDirectExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_ExitParam := cmbDirectExit.ItemIndex;
end;

procedure TfrmSubtitleSettings.cmbEffectEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    case cmbEffectEnter.ItemIndex of
        0:
            begin
                FSettings.m_Entrance := IID_Subtitle_None;
                cmbDirectEnter.Enabled := FALSE;
                cmbSpeedEnter.Enabled := FALSE;
            end;
        1:
            begin
                FSettings.m_Entrance := IID_Subtitle_Fade;
                cmbDirectEnter.Enabled := FALSE;
                cmbSpeedEnter.Enabled := TRUE;
            end;
        2:
            begin
                FSettings.m_Entrance := IID_Subtitle_Slide;
                cmbDirectEnter.Enabled := TRUE;
                cmbSpeedEnter.Enabled := TRUE;
            end;
    end;
end;

procedure TfrmSubtitleSettings.cmbEffectExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    case cmbEffectExit.ItemIndex of
        0:
            begin
                FSettings.m_Exit := IID_Subtitle_None;
                cmbDirectExit.Enabled := FALSE;
                cmbSpeedExit.Enabled := FALSE;
            end;
        1:
            begin
                FSettings.m_Exit := IID_Subtitle_Fade;
                cmbDirectExit.Enabled := FALSE;
                cmbSpeedExit.Enabled := TRUE;
            end;
        2:
            begin
                FSettings.m_Exit := IID_Subtitle_Slide;
                cmbDirectExit.Enabled := TRUE;
                cmbSpeedExit.Enabled := TRUE;
            end;
    end;
end;

procedure TfrmSubtitleSettings.cmbSpeedEnterChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_EntranceTime := SubtitleSpeeds[cmbSpeedEnter.ItemIndex];
end;

procedure TfrmSubtitleSettings.cmbSpeedExitChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_ExitTime := SubtitleSpeeds[cmbSpeedExit.ItemIndex];
end;

procedure TfrmSubtitleSettings.ctbBorderColorChanged(Sender: TColorToolBar;
  Color: Cardinal);
begin
    FSettings.m_BorderColor.Color := (FSettings.m_BorderColor.Color and $FF000000) or Color;
end;

procedure TfrmSubtitleSettings.ctbBorderDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbBorder.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSettings.m_BorderColor.Color);
end;

procedure TfrmSubtitleSettings.ctbFontColorChanged(Sender: TColorToolBar; Color: Cardinal);
begin
    FSettings.m_FontColor.Color := (FSettings.m_FontColor.Color and $FF000000) or Color;
end;

procedure TfrmSubtitleSettings.ctbFontDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbFont.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSettings.m_FontColor.Color);
end;

procedure TfrmSubtitleSettings.ctbShadowColorChanged(Sender: TColorToolBar; Color: Cardinal);
begin
    FSettings.m_ShadowColor.Color := (FSettings.m_ShadowColor.Color and $FF000000) or Color;
end;

procedure TfrmSubtitleSettings.ctbShadowDropdown(Sender: TColorToolBar);
begin
    CreateHSBControl(self.Handle, ctbShadow.Handle, CM_COLOR_TOOLBAR_SETCOLOR, FSettings.m_ShadowColor.Color);
end;

procedure TfrmSubtitleSettings.FormCreate(Sender: TObject);
begin
    FSettings := SubtitleGetDefault();
    PageControl1.ActivePageIndex := 0;
end;

procedure TfrmSubtitleSettings.InitForm(Item: PVCItem);
begin
    FItem := Item;
    FSubtitle := SubtitleGet(FItem);
    FSettings := SubtitleGetDefault();

    FChanging := TRUE;
    cmbHorizontal.ItemIndex := FSettings.m_HPosition;
    cmbVertical.ItemIndex := FSettings.m_VPosition;

    lblFont.Caption := GetFontString(FSettings.m_FontName, FSettings.m_FontSize, FSettings.m_FontStyle);
    ctbFont.SelectedColor := FSettings.m_FontColor.Color;
    ctbBorder.SelectedColor := FSettings.m_BorderColor.Color;
    ctbShadow.SelectedColor := FSettings.m_ShadowColor.Color;
    tbFont.Position := $FF - FSettings.m_FontColor.A;
    tbBorder.Position := $FF - FSettings.m_BorderColor.A;
    tbShadow.Position := $FF - FSettings.m_ShadowColor.A;
    cmbAlignment.ItemIndex := FSettings.m_Alignment;

    cmbEffectEnter.ItemIndex := GetSubtitleEffectIndex(FSettings.m_Entrance);
    cmbDirectEnter.ItemIndex := FSettings.m_EntranceParam;
    cmbSpeedEnter.ItemIndex := GetSubtitleSpeedIndex(FSettings.m_EntranceTime);
    ckbEnterOnce.Checked := FSettings.m_EntranceOnce;

    cmbEffectExit.ItemIndex := GetSubtitleEffectIndex(FSettings.m_Exit);
    cmbDirectExit.ItemIndex := FSettings.m_ExitParam;
    cmbSpeedExit.ItemIndex := GetSubtitleSpeedIndex(FSettings.m_ExitTime);
    ckbExitOnce.Checked := FSettings.m_ExitOnce;

    FChanging := FALSE;
end;

procedure TfrmSubtitleSettings.tbBorderChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_BorderColor.A := $FF - tbBorder.Position;
end;

procedure TfrmSubtitleSettings.tbFontChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_FontColor.A := $FF - tbFont.Position;
end;

procedure TfrmSubtitleSettings.tbShadowChange(Sender: TObject);
begin
    if FChanging then Exit;
    FSettings.m_ShadowColor.A := $FF - tbShadow.Position;
end;

end.
