Unit Main;

Interface

Uses
	Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
	Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.ToolWin,
	Vcl.ImgList, Vcl.ExtCtrls, Vcl.Buttons, Vcl.StdCtrls, GraphicsButton,
	GraphicsGroupRadio, GraphicsSpeedButton, Vcl.OleCtrls, SHDocVw,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsCommandLink;

Type
	TfrmMain = Class(TForm)
		ScrollBox1: TScrollBox;
		Panel1: TPanel;
		btnAdd: TGraphicsSpeedButton;
		OpenDialog1: TOpenDialog;
		WebBrowser1: TWebBrowser;
		btnStart: TGraphicsSpeedButton;
		btnFormat: TGraphicsCommandLink;
		btnOptions: TGraphicsSpeedButton;
		btnAbout: TGraphicsSpeedButton;
		Procedure FormCreate(Sender: TObject);
		Procedure btnAddClick(Sender: TObject);
		Procedure btnFormatClick(Sender: TObject);
		Procedure btnStartClick(Sender: TObject);
		Procedure btnOptionsClick(Sender: TObject);
		Procedure btnAboutClick(Sender: TObject);
	Private
		{ Private declarations }
		Function DoMouseWheel(Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean; Override;
	Private
		FBitmapFormat: TGPBitmap;
		Procedure SetOutputFormat();
	Public
		{ Public declarations }
		Procedure OnItemRemove(index: Integer);
		Procedure ConvertNextItem();
	End;

Var
	frmMain: TfrmMain;

Implementation

{$R *.dfm}


Uses
	Defines, Profile, CommonData, ItemView, Functions, ImageResource, CodecSettings,
	VideoConverterInt, SelectMode, Merge, Options, About;

Procedure TfrmMain.btnAboutClick(Sender: TObject);
Var
	frmAbout: TfrmAbout;
Begin
	frmAbout := TfrmAbout.Create(self);
    frmAbout.ShowModal();
    frmAbout.Free();
End;

Procedure TfrmMain.btnAddClick(Sender: TObject);
Var
	i          : Integer;
	frmItem    : TfrmItem;
	ErrorString: WideString;
Begin
	If OpenDialog1.Execute(frmMain.Handle) = false Then exit;
	If OpenDialog1.Files.Count = 0 Then exit;
	WebBrowser1.Hide();
	ScrollBox1.Show();
	For i := 0 To OpenDialog1.Files.Count - 1 Do
	Begin
		frmItem := TfrmItem.Create(ScrollBox1);
		frmItem.Parent := ScrollBox1;
		frmItem.Align := alBottom;
		If frmItem.ImportFile(OpenDialog1.Files[i]) Then
		Begin
			frmItem.Show();
			frmItem.Align := alTop;
			g_itemviews[g_itemcount] := frmItem;
			g_itemviews[g_itemcount].m_Index := g_itemcount;
			g_itemcount := g_itemcount + 1;
		End Else Begin
			frmItem.Free();
			ErrorString := 'This file is not a video file.' + #13#10 + OpenDialog1.Files[i];
			Application.MessageBox(PWideChar(ErrorString), PWideChar(APP_TITLE), MB_ICONERROR);
		End;
	End;
	btnStart.Enabled := TRUE;
End;

Procedure TfrmMain.OnItemRemove(index: Integer);
Var
	i: Integer;
Begin
	For i := Index To g_itemcount - 2 Do
	Begin
		g_itemviews[i] := g_itemviews[i + 1];
	End;
	g_itemcount := g_itemcount - 1;
	btnStart.Enabled := g_itemcount > 0;
	For i := 0 To g_itemcount - 1 Do
	Begin
		g_itemviews[i].m_Index := i;
	End;
End;

Procedure TfrmMain.btnFormatClick(Sender: TObject);
Var
	frmCodecSettings: TfrmCodecSettings;
Begin
	frmCodecSettings := TfrmCodecSettings.Create(self);
	frmCodecSettings.ShowModal();
	frmCodecSettings.Free();
	SetOutputFormat();
	CopyProfileSettings();
End;

Procedure TfrmMain.btnOptionsClick(Sender: TObject);
Var
	frmOptions: TfrmOptions;
Begin
	frmOptions := TfrmOptions.Create(self);
	frmOptions.InitForm();
	frmOptions.ShowModal();
	frmOptions.Free();
End;

Procedure TfrmMain.ConvertNextItem();
Var
	frmItem: TfrmItem;
	i      : Integer;
Begin
	For i := 0 To g_itemcount - 1 Do
	Begin
		frmItem := g_itemviews[i];
		If frmItem.StartConvert() Then exit;
	End;
	btnStart.Enabled := TRUE;
End;

Procedure TfrmMain.btnStartClick(Sender: TObject);
Var
	frmSelectMode: TfrmSelectMode;
	frmMerge     : TfrmMerge;
	Count        : Integer;
	i            : Integer;
	frmItem      : TfrmItem;
	ResultMode   : Integer;
Begin
	If g_itemcount > 1 Then
	Begin
		frmSelectMode := TfrmSelectMode.Create(self);
		ResultMode := frmSelectMode.ShowModal();
		frmSelectMode.Free();
		If ResultMode <> mrOK Then exit;
	End Else
	Begin
		g_merge_mode := false;
	End;

	If g_merge_mode Then
	Begin
		frmMerge := TfrmMerge.Create(self);
		For i := 0 To g_itemcount - 1 Do
		Begin
			frmItem := g_itemviews[i];
			frmMerge.FItems[i] := frmItem.GetItem();
		End;
		frmMerge.FCount := g_itemcount;
		frmMerge.ShowModal();
		frmMerge.Free();
	End Else
	Begin
		btnStart.Enabled := false;
		For i := 0 To g_itemcount - 1 Do
		Begin
			frmItem := g_itemviews[i];
			frmItem.WaitForConvert();
		End;
		Count := g_itemcount;
		If Count > _Global.m_Parallel Then Count := _Global.m_Parallel;
		vcReadyForConvert();
		For i := 0 To Count - 1 Do
		Begin
			frmItem := g_itemviews[i];
			frmItem.StartConvert();
		End;
	End;
End;

Function TfrmMain.DoMouseWheel(Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean;
Begin
	ScrollBox1.VertScrollBar.Position := ScrollBox1.VertScrollBar.Position - WheelDelta;
End;

Procedure TfrmMain.FormCreate(Sender: TObject);
Begin
	OpenDialog1.Filter := MOVIE_FILE_FILTER;
	WebBrowser1.Align := alClient;
	ScrollBox1.Align := alClient;
	WebBrowser1.Show();
	ScrollBox1.Hide();
	WebBrowser1.Navigate(ResourcePath + 'Hint.htm');
	btnAdd.SetImage(imageAddMovie);
	btnStart.SetImage(imageStart);
	btnOptions.SetImage(imageOptions);
    btnAbout.SetImage(LoadThumbailImage(ResourcePath + 'Logo.png', 48, 48));
	SetOutputFormat();
End;

Procedure TfrmMain.SetOutputFormat();
Begin
	If FBitmapFormat <> Nil Then FBitmapFormat.Free;
	FBitmapFormat := LoadImageToBitmap(GetFormatImageFile(_ProfleSettings));
	btnFormat.SetImage(FBitmapFormat, GetFormatName(_ProfleSettings), GetFormatDescription(_ProfleSettings));
	btnOptions.Left := btnFormat.Left + btnFormat.Width + 8;
End;

End.
