Program videoconverter;

Uses
	Vcl.Forms,
	Winapi.Windows,
	Winapi.ShellAPI,
	System.SysUtils,
	About In 'About.pas' {frmAbout} ,
	CodecSettings In 'CodecSettings.pas' {frmCodecSettings} ,
	FileInformation In 'FileInformation.pas' {frmMediaInfo} ,
	Flash In 'Flash.pas' {frmFlash} ,
	ItemView In 'ItemView.pas' {frmItem} ,
	Main In 'Main.pas' {frmMain} ,
	Profile In 'Profile.pas',
	VideoConverterInt In 'VideoConverterInt.pas',
	Functions In 'Functions.pas',
	Defines In 'Defines.pas',
	ImageResource In 'ImageResource.pas',
	LogFile In 'LogFile.pas',
	CommonData In 'CommonData.pas',
	VideoEffectInt In 'VideoEffectInt.pas',
	VideoPlayer In 'VideoPlayer.pas' {frmVideoPlayer} ,
	EffectEditor In 'EffectEditor.pas' {frmEffect} ,
	CutMedia In 'CutMedia.pas' {frmCut} ,
	SelectMode In 'SelectMode.pas' {frmSelectMode} ,
	Options In 'Options.pas' {frmOptions} ,
	Merge In 'Merge.pas' {frmMerge} ,
	Processing In 'Processing.pas' {frmProcessing} ,
	Subtitle In 'Subtitle.pas' {frmSubtitle} ,
	SubtitleEdit In 'SubtitleEdit.pas' {frmSubtitleEdit} ,
	SubtitleInt In 'SubtitleInt.pas',
	SubtitleSettings In 'SubtitleSettings.pas' {frmSubtitleSettings} ,
	SubtitleText In 'SubtitleText.pas' {frmSubtitleText} ,
	DemoWindow In 'DemoWindow.pas' {frmDemo} ,
	AudioSettings In 'AudioSettings.pas' {frmAudioSettings} ,
	Recording In 'Recording.pas' {frmRecording} ,
	SubtitleSelection In 'SubtitleSelection.pas' {frmSubtitleSelection};

{$R *.res}


Var
	MessageString: WideString;

Begin
	If GetMultipleInatance() > 1 Then
	Begin
		SetForegroundWindow(GetMainWindowHandle());
		Exit;
	End;

	If
	  (VCGlobal_Size() <> sizeof(VCGlobal)) Or
	  (MediaInfo_Size() <> sizeof(MediaInfo)) Or
	  (VCItem_Size() <> sizeof(VCItem)) Or
	  (CodecOptions_Size() <> sizeof(CodecOptions)) Or
	  (MergeInfo_Size() <> sizeof(MergeInfo))
	Then
	Begin
		MessageString := Format('Error Version !'#13#10 +
		  'VCGlobal:%d<->%d'#13#10 +
		  'MediaInfo:%d<->%d'#13#10 +
		  'VCItem:%d<->%d'#13#10 +
		  'CodecOptions:%d<->%d'#13#10 +
		  'MergeInfo:%d<->%d'#13#10
		  ,
		  [VCGlobal_Size(), sizeof(VCGlobal),
		  MediaInfo_Size(), sizeof(MediaInfo),
		  VCItem_Size(), sizeof(VCItem),
		  CodecOptions_Size(), sizeof(CodecOptions),
		  MergeInfo_Size(), sizeof(MergeInfo)
		  ]);
		Application.MessageBox(PWideChar(MessageString), PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;

	If (ProfileVideo_Size() <> sizeof(ProfileVideo)) Or
	  (ProfileAudio_Size() <> sizeof(ProfileAudio)) Or
	  (ProfileTarget_Size() <> sizeof(ProfileTarget)) Or
	  (ProfileTag_Size() <> sizeof(ProfileTag)) Or
	  (ProfileSettings_Size() <> sizeof(ProfileSettings))
	Then
	Begin
		MessageString := Format('Error Version !'#13#10 +
		  'ProfileVideo:%d<->%d'#13#10 +
		  'ProfileAudio:%d<->%d'#13#10 +
		  'ProfileTarget:%d<->%d'#13#10 +
		  'ProfileTag:%d<->%d'#13#10 +
		  'ProfileSettings:%d<->%d'#13#10
		  ,
		  [ProfileVideo_Size(), sizeof(ProfileVideo),
		  ProfileAudio_Size(), sizeof(ProfileAudio),
		  ProfileTarget_Size(), sizeof(ProfileTarget),
		  ProfileTag_Size(), sizeof(ProfileTag),
		  ProfileSettings_Size(), sizeof(ProfileSettings)
		  ]);
		Application.MessageBox(PWideChar(MessageString), PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;

	If (CSubtitleLine_Size() <> sizeof(CSubtitleLine)) Or
	  (CSubtitleItem_Size() <> sizeof(CSubtitleItem)) Or
	  (CSubtitle_Size() <> sizeof(CSubtitle))
	Then
	Begin
		MessageString := Format('Error Version !'#13#10 +
		  'CSubtitleDefault:%d<->%d'#13#10 +
		  'CSubtitleLine:%d<->%d'#13#10 +
		  'CSubtitleItem:%d<->%d'#13#10 +
		  'CSubtitle:%d<->%d'#13#10
		  ,
		  [SubtitleDefault_Size(), sizeof(SubtitleDefault),
		  CSubtitleLine_Size(), sizeof(CSubtitleLine),
		  CSubtitleItem_Size(), sizeof(CSubtitleItem),
		  CSubtitle_Size(), sizeof(CSubtitle)
		  ]);
		Application.MessageBox(PWideChar(MessageString), PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;

	Application.Initialize;
	Application.MainFormOnTaskbar := TRUE;
	Application.Title := APP_TITLE;

	InitApplication();

	frmFlash := TfrmFlash.Create(Nil);
	frmFlash.Show();
	frmFlash.Update();

	If vcInitialize() <> S_OK Then
	Begin
		frmFlash.Free();
		Application.MessageBox(FAILD_INITIALIZE, PWideChar(APP_TITLE), MB_ICONERROR);
		Exit;
	End;

	LoadImageResources();
	InitCommonData();

	Application.CreateForm(TfrmMain, frmMain);
	SetMainWindowHandle(frmMain.Handle);
	frmFlash.Free();
	Application.Run;

End.
