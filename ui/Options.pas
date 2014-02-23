Unit Options;

Interface

{$WARN UNIT_PLATFORM OFF}


Uses
	Winapi.Windows, Winapi.Messages, System.Types, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
	Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.StdCtrls, Vcl.FileCtrl,
	Vcl.ExtCtrls, Vcl.Samples.Spin, Vcl.ExtDlgs, Vcl.ImgList,
	Winapi.GDIPAPI, Winapi.GDIPOBJ,
	VideoConverterInt, GraphicsButton, GraphicsGPImage, ListViewEx, Vcl.Menus,
	ColorToolBar, Vcl.Buttons, GraphicsSpeedButton, WinPaint, DemoWindow;

Const
	DEMO_TYPE_WATER  = 1;
	DEMO_TYPE_RESIZE = 2;
	DEMO_TYPE_BK     = 3;

Type
	TfrmOptions = Class(TForm)
		edFolder: TEdit;
		Label1: TLabel;
		btnBrowse: TButton;
		Label8: TLabel;
		edWatermark: TEdit;
		btnWaterAdd: TGraphicsButton;
		btnWaterDelete: TGraphicsButton;
		cmbWaterHPosition: TComboBox;
		ImageList1: TImageList;
		OpenDialog1: TOpenDialog;
		tbTransparency: TTrackBar;
		lblTransparency: TLabel;
		Label5: TLabel;
		cmbWaterVPosition: TComboBox;
		PageControl1: TPageControl;
		TabSheet1: TTabSheet;
		TabSheet2: TTabSheet;
		Label6: TLabel;
		GroupBox2: TGroupBox;
		TabSheet3: TTabSheet;
		TabSheet4: TTabSheet;
		rgResizeModth: TRadioGroup;
		pcBackground: TPageControl;
		TabSheet5: TTabSheet;
		TabSheet6: TTabSheet;
		TabSheet7: TTabSheet;
		cmbFillDirect: TComboBox;
		btnBrowser: TButton;
		TabSheet8: TTabSheet;
		lblParallel: TLabel;
		cmbParallel: TComboBox;
		ctbColor: TColorToolBar;
		ctbColor1: TColorToolBar;
		ctbColor2: TColorToolBar;
		btnPreResize: TGraphicsSpeedButton;
		btnNextResize: TGraphicsSpeedButton;
		lalWaterWidth: TLabel;
		Label3: TLabel;
		edWaterWidth: TEdit;
		edWaterHeight: TEdit;
		udWaterWidth: TUpDown;
		udWaterHeight: TUpDown;
		gbType: TGroupBox;
		btnSolid: TRadioButton;
		btnGradient: TRadioButton;
		btnPicture: TRadioButton;
		Procedure FormCreate(Sender: TObject);
		Procedure btnWaterAddClick(Sender: TObject);
		Procedure Image1Click(Sender: TObject);
		Procedure btnWaterDeleteClick(Sender: TObject);
		Procedure tbTransparencyChange(Sender: TObject);
		Procedure btnBrowseClick(Sender: TObject);
		Procedure FormDestroy(Sender: TObject);
		Procedure cmbWaterHPositionChange(Sender: TObject);
		Procedure cmbWaterVPositionChange(Sender: TObject);
		Procedure btnBrowserClick(Sender: TObject);
		Procedure cmbParallelChange(Sender: TObject);
		Procedure rgResizeModthClick(Sender: TObject);
		Procedure btnPreResizeClick(Sender: TObject);
		Procedure btnNextResizeClick(Sender: TObject);
		Procedure udWaterWidthClick(Sender: TObject; Button: TUDBtnType);
		Procedure udWaterHeightClick(Sender: TObject; Button: TUDBtnType);
		Procedure ctbColorColorChanged(Sender: TColorToolBar; Color: Cardinal);
		Procedure ctbColorDropdown(Sender: TColorToolBar);
		Procedure ctbColor1ColorChanged(Sender: TColorToolBar; Color: Cardinal);
		Procedure ctbColor1Dropdown(Sender: TColorToolBar);
		Procedure ctbColor2ColorChanged(Sender: TColorToolBar; Color: Cardinal);
		Procedure ctbColor2Dropdown(Sender: TColorToolBar);
		Procedure cmbFillDirectChange(Sender: TObject);
		Procedure OnFillTypeClick(Sender: TObject);
		Procedure FormClose(Sender: TObject; Var Action: TCloseAction);
		Procedure PageControl1Change(Sender: TObject);
	Private
		{ Private declarations }
		FChanging    : Boolean;
		FCodecOptions: PCodecOptions;
		frmDemo      : TfrmDemo;
		FImageBuffer : PBYTE;
		FDemoImage   : TGPBitmap;
		Procedure ShowDemo(DemoType: Integer);
		Procedure HideDemo();
		Procedure WMMove(Var Message: TWMMOVE); Message WM_MOVE;
	Private
		{ Background }
		Function LoadBackground(): Boolean;
		Procedure RecreateBackImage();
		Procedure SetFillType();
	Private
		{ Logo }
		Procedure RecreateWaterImage();
		Procedure SetWaterEnable(bEnabled: Boolean);
	Private
		{ Resize }
		FResizeIndex: Integer;
		Procedure RecreateResizeImage();
	Public
		{ Public declarations }
		Procedure InitForm();
	End;

Implementation

Uses
	Functions, ImageResource, Defines, Main, CommonData, ItemView;

{$R *.dfm}


Const
	WatermarkPos: Array [0 .. 2] Of Integer = (1, 2, 4);

Function WatermarkPosIndex(Pos: Integer): Integer;
Var
	i: Integer;
Begin
	For i := 0 To 2 Do
	Begin
		If WatermarkPos[i] = Pos Then
		Begin
			Result := i;
			Exit;
		End;
	End;
	Result := -1;
End;

Procedure TfrmOptions.btnBrowseClick(Sender: TObject);
Var
	Directory: String;
	Options  : TSelectDirExtOpts;
Begin
	Directory := edFolder.Text;
	Options := [sdNewFolder, sdNewUI, sdValidateDir];

	If SelectDirectory('Select Export Folder:', '', Directory, Options, Self) Then
	Begin
		edFolder.Text := Directory;
		lstrcpy(_Global.m_OutputPath, PWideChar(WideString(Directory)));
	End;
End;

Procedure TfrmOptions.btnBrowserClick(Sender: TObject);
Begin
	If LoadBackground() Then
	Begin
		RecreateBackImage();
	End;
End;

Procedure TfrmOptions.btnWaterDeleteClick(Sender: TObject);
Begin
	edWatermark.Text := '';
	_Global.m_LogoFile[0] := #0;
	SetWaterEnable(FALSE);
	HideDemo();
End;

Procedure TfrmOptions.btnWaterAddClick(Sender: TObject);
Var
	FileName               : String;
	ImageWidth, ImageHeight: Integer;
Begin
	OpenDialog1.Title := SELECT_WATERMARK;
	OpenDialog1.Filter := PICTURE_FILTERS;
	If Not OpenDialog1.Execute(Self.Handle) Then Exit;
	FileName := OpenDialog1.FileName;
	If IsSupportImage(FileName, ImageWidth, ImageHeight) = FALSE Then
	Begin
		Application.MessageBox(NO_IMAGE_FILE, PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;
	edWatermark.Text := OpenDialog1.FileName;
	lstrcpyW(_Global.m_LogoFile, PWideChar(WideString(edWatermark.Text)));
	_Global.m_LogoFileSize.Width := ImageWidth;
	_Global.m_LogoFileSize.Height := ImageHeight;
	If (ImageWidth > FCodecOptions.m_VideoWidth) Or (ImageHeight > FCodecOptions.m_VideoHeight) Then
	Begin
		If ImageWidth * FCodecOptions.m_VideoHeight > FCodecOptions.m_VideoWidth * ImageHeight Then
		Begin
			_Global.m_LogoRect.Width := FCodecOptions.m_VideoWidth;
			_Global.m_LogoRect.Height := FCodecOptions.m_VideoWidth * ImageHeight Div ImageWidth;
		End Else Begin
			_Global.m_LogoRect.Width := FCodecOptions.m_VideoHeight * ImageWidth Div ImageHeight;
			_Global.m_LogoRect.Height := FCodecOptions.m_VideoHeight;
		End;
	End Else Begin
		_Global.m_LogoRect.Width := ImageWidth;
		_Global.m_LogoRect.Height := ImageHeight;
	End;
	If (_Global.m_LogoRect.X + _Global.m_LogoRect.Width) > FCodecOptions.m_VideoWidth Then
	Begin
		_Global.m_LogoRect.X := FCodecOptions.m_VideoWidth - _Global.m_LogoRect.Width;
	End;
	If (_Global.m_LogoRect.Y + _Global.m_LogoRect.Height) > FCodecOptions.m_VideoHeight Then
	Begin
		_Global.m_LogoRect.Y := FCodecOptions.m_VideoHeight - _Global.m_LogoRect.Height;
	End;
	udWaterWidth.Position := _Global.m_LogoRect.Width;
	udWaterHeight.Position := _Global.m_LogoRect.Height;
	SetWaterEnable(TRUE);
	ShowDemo(DEMO_TYPE_WATER);
End;

Procedure TfrmOptions.cmbWaterHPositionChange(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_LogoHPosition := cmbWaterHPosition.ItemIndex;
	RecreateWaterImage();
End;

Procedure TfrmOptions.cmbFillDirectChange(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_FillDirect := cmbFillDirect.ItemIndex;
	RecreateBackImage();
End;

Procedure TfrmOptions.cmbParallelChange(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_Parallel := cmbParallel.ItemIndex;
End;

Procedure TfrmOptions.cmbWaterVPositionChange(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_LogoVPosition := cmbWaterVPosition.ItemIndex;
	RecreateWaterImage();
End;

Procedure TfrmOptions.ctbColor1ColorChanged(Sender: TColorToolBar; Color: Cardinal);
Begin
	_Global.m_FillColor1 := Color;
	ctbColor.SelectedColor := Color;
	RecreateBackImage();
End;

Procedure TfrmOptions.ctbColor1Dropdown(Sender: TColorToolBar);
Begin
	CreateHSBControl(Self.Handle, ctbColor1.Handle, CM_COLOR_TOOLBAR_SETCOLOR, _Global.m_FillColor1);
End;

Procedure TfrmOptions.ctbColor2ColorChanged(Sender: TColorToolBar; Color: Cardinal);
Begin
	_Global.m_FillColor2 := Color;
	RecreateBackImage();
End;

Procedure TfrmOptions.ctbColor2Dropdown(Sender: TColorToolBar);
Begin
	CreateHSBControl(Self.Handle, ctbColor2.Handle, CM_COLOR_TOOLBAR_SETCOLOR, _Global.m_FillColor2);
End;

Procedure TfrmOptions.ctbColorColorChanged(Sender: TColorToolBar; Color: Cardinal);
Begin
	_Global.m_FillColor1 := Color;
	ctbColor1.SelectedColor := Color;
	RecreateBackImage();
End;

Procedure TfrmOptions.ctbColorDropdown(Sender: TColorToolBar);
Begin
	CreateHSBControl(Self.Handle, ctbColor.Handle, CM_COLOR_TOOLBAR_SETCOLOR, _Global.m_FillColor1);
End;

Procedure TfrmOptions.FormClose(Sender: TObject; Var Action: TCloseAction);
Begin
	frmDemo.Close();
End;

Procedure TfrmOptions.FormCreate(Sender: TObject);
Var
	i: Integer;
Begin
	PageControl1.ActivePageIndex := 0;
	btnWaterAdd.SetImage(imageAddFile);
	btnWaterDelete.SetImage(imageDelete);
	btnPreResize.SetImage(imagePrevious);
	btnNextResize.SetImage(imageNext);

	For i := 1 To GetCpuCoreCount() Do
	Begin
		cmbParallel.Items.Add(Format('%d', [i]));
	End;
	FCodecOptions := vcGetCodecOptions();;

	FImageBuffer := GetMemory(FCodecOptions.m_VideoWidth * FCodecOptions.m_VideoHeight * 4);
	FDemoImage := TGPBitmap.Create(FCodecOptions.m_VideoWidth, FCodecOptions.m_VideoHeight, FCodecOptions.m_VideoWidth * 4, PixelFormat32bppRGB, FImageBuffer);

	frmDemo := TfrmDemo.Create(Self);
	frmDemo.HandleNeeded;
	frmDemo.FImage := FDemoImage;
End;

Procedure TfrmOptions.FormDestroy(Sender: TObject);
Begin
	frmDemo.Free();
	FDemoImage.Free();
	FreeMemory(FImageBuffer);
End;

Procedure TfrmOptions.btnPreResizeClick(Sender: TObject);
Begin
	FResizeIndex := FResizeIndex - 1;
	btnPreResize.Enabled := FResizeIndex > 0;
	btnNextResize.Enabled := TRUE;
	RecreateResizeImage();
End;

Procedure TfrmOptions.OnFillTypeClick(Sender: TObject);
Var
	FillType: Integer;
Begin
	If FChanging Then Exit;
	FillType := (Sender As TRadioButton).Tag;
	If (FillType = FillType_Picture) Then
	Begin
		If _Global.m_FillPicture[0] = #0 Then
		Begin
			If LoadBackground() = FALSE Then
			Begin
				SetFillType();
				Exit;
			End;
		End;
	End;
	_Global.m_FillType := FillType;
	pcBackground.ActivePageIndex := _Global.m_FillType;
	RecreateBackImage();
End;

Procedure TfrmOptions.PageControl1Change(Sender: TObject);
Begin
	HideDemo();
	Case PageControl1.ActivePageIndex Of
		1:
			Begin
				If _Global.m_LogoFile[0] <> #0 Then
				Begin
					ShowDemo(DEMO_TYPE_WATER);
				End;
			End;
		2:
			Begin
				If g_itemcount > 0 Then
				Begin
					ShowDemo(DEMO_TYPE_RESIZE);
				End;
			End;
		3:
			Begin
				ShowDemo(DEMO_TYPE_BK);
			End;
	End;
End;

Procedure TfrmOptions.btnNextResizeClick(Sender: TObject);
Begin
	FResizeIndex := FResizeIndex + 1;
	btnPreResize.Enabled := TRUE;
	btnNextResize.Enabled := FResizeIndex < (g_itemcount - 1);
	RecreateResizeImage();
End;

Procedure TfrmOptions.Image1Click(Sender: TObject);
Begin
	Application.ActivateHint(Mouse.CursorPos);
End;

Procedure TfrmOptions.InitForm;
Begin
	FChanging := TRUE;
	// Page 1
	edFolder.Text := _Global.m_OutputPath;

	// Page 2
	edWatermark.Text := _Global.m_LogoFile;
	tbTransparency.Position := _Global.m_LogoAlpha;
	cmbWaterHPosition.ItemIndex := _Global.m_LogoHPosition;
	cmbWaterVPosition.ItemIndex := _Global.m_LogoVPosition;
	udWaterWidth.Min := 1;
	udWaterWidth.Max := FCodecOptions.m_VideoWidth;
	udWaterHeight.Min := 1;
	udWaterHeight.Max := FCodecOptions.m_VideoHeight;
	SetWaterEnable(_Global.m_LogoFile[0] <> #0);

	// Page 3
	rgResizeModth.ItemIndex := _Global.m_ResizeMethod;
	btnPreResize.Enabled := FALSE;
	btnNextResize.Enabled := g_itemcount > 1;

	// Page 4
	SetFillType();
	ctbColor.SelectedColor := _Global.m_FillColor1;
	ctbColor1.SelectedColor := _Global.m_FillColor1;
	ctbColor2.SelectedColor := _Global.m_FillColor2;
	pcBackground.ActivePageIndex := _Global.m_FillType;
	cmbFillDirect.ItemIndex := _Global.m_FillDirect;

	// Page 5
	cmbParallel.ItemIndex := _Global.m_Parallel - 1;

	FChanging := FALSE;
End;

Function TfrmOptions.LoadBackground(): Boolean;
Var
	FileName               : String;
	ImageWidth, ImageHeight: Integer;
Begin
	Result := FALSE;
	OpenDialog1.Title := SELECT_WATERMARK;
	OpenDialog1.Filter := PICTURE_FILTERS;
	If Not OpenDialog1.Execute(Self.Handle) Then Exit;
	FileName := OpenDialog1.FileName;
	If IsSupportImage(FileName, ImageWidth, ImageHeight) = FALSE Then
	Begin
		Application.MessageBox(NO_IMAGE_FILE, PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;
	lstrcpyW(_Global.m_FillPicture, PWideChar(WideString(FileName)));
	Result := TRUE;
End;

Procedure TfrmOptions.RecreateBackImage();
Begin
	DraweBackgroundSample(FImageBuffer);
	frmDemo.Redraw();
End;

Procedure TfrmOptions.RecreateResizeImage();
Begin
	If g_itemcount = 0 Then Exit;
	DrawResizeSample(FImageBuffer, g_itemviews[FResizeIndex].GetItem());
	frmDemo.Redraw();
End;

Procedure TfrmOptions.SetFillType();
Var
	b: Boolean;
Begin
	b := FChanging;
	FChanging := TRUE;
	btnSolid.Checked := FALSE;
	btnGradient.Checked := FALSE;
	btnPicture.Checked := FALSE;
	Case _Global.m_FillType Of
		FillType_Solid: btnSolid.Checked := TRUE;
		FillType_Gradient: btnGradient.Checked := TRUE;
		FillType_Picture: btnPicture.Checked := TRUE;
	End;
	FChanging := b;
End;

Procedure TfrmOptions.SetWaterEnable(bEnabled: Boolean);
Begin
	btnWaterDelete.Enabled := bEnabled;
	edWaterWidth.Enabled := bEnabled;
	udWaterWidth.Enabled := bEnabled;
	edWaterHeight.Enabled := bEnabled;
	udWaterHeight.Enabled := bEnabled;
	tbTransparency.Enabled := bEnabled;
	cmbWaterHPosition.Enabled := bEnabled;
	cmbWaterVPosition.Enabled := bEnabled;
End;

Procedure TfrmOptions.ShowDemo(DemoType: Integer);
Begin
	Case DemoType Of
		DEMO_TYPE_WATER: DrawLogoSample(FImageBuffer);
		DEMO_TYPE_RESIZE: RecreateResizeImage();
		DEMO_TYPE_BK: DraweBackgroundSample(FImageBuffer);
	End;
	ShowWindow(frmDemo.Handle, SW_SHOWNOACTIVATE);
	InvalidateRect(frmDemo.Handle, Nil, FALSE);
End;

Procedure TfrmOptions.HideDemo();
Begin
	ShowWindow(frmDemo.Handle, SW_HIDE);
End;

Procedure TfrmOptions.RecreateWaterImage();
Begin
	DrawLogoSample(FImageBuffer);
	frmDemo.Redraw();
End;

Procedure TfrmOptions.rgResizeModthClick(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_ResizeMethod := rgResizeModth.ItemIndex;
	RecreateResizeImage();
End;

Procedure TfrmOptions.tbTransparencyChange(Sender: TObject);
Begin
	If FChanging Then Exit;
	_Global.m_LogoAlpha := tbTransparency.Position;
	RecreateWaterImage();
End;

Procedure TfrmOptions.udWaterHeightClick(Sender: TObject; Button: TUDBtnType);
Begin
	If FChanging Then Exit;
	_Global.m_LogoRect.Height := udWaterWidth.Position;
	If (_Global.m_LogoRect.Y + _Global.m_LogoRect.Height) > FCodecOptions.m_VideoHeight Then
	Begin
		_Global.m_LogoRect.Y := FCodecOptions.m_VideoHeight - _Global.m_LogoRect.Height;
	End;
	RecreateWaterImage();
End;

Procedure TfrmOptions.udWaterWidthClick(Sender: TObject; Button: TUDBtnType);
Begin
	If FChanging Then Exit;
	_Global.m_LogoRect.Width := udWaterWidth.Position;
	If (_Global.m_LogoRect.X + _Global.m_LogoRect.Width) > FCodecOptions.m_VideoWidth Then
	Begin
		_Global.m_LogoRect.X := FCodecOptions.m_VideoWidth - _Global.m_LogoRect.Width;
	End;
	RecreateWaterImage();
End;

Procedure TfrmOptions.WMMove(Var Message: TWMMOVE);
Var
	rc: TRECT;
Begin
	Inherited;
	If frmDemo = Nil Then Exit;
	GetWindowRect(Self.Handle, rc);
	SetWindowPos(frmDemo.Handle, 0, rc.Left + rc.Width + 2, rc.Top - 2, 0, 0, SWP_NOZORDER Or SWP_NOSIZE Or SWP_NOACTIVATE);
End;

End.
