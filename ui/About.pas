Unit About;

Interface

Uses
	Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
	System.Classes, Vcl.Graphics, Winapi.GDIPOBJ, Winapi.ShellAPI,
	Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.StdCtrls,
	Vcl.Imaging.pngimage, Winapi.CommCtrl, Functions, Defines;

Type
	TfrmAbout = Class(TForm)
		btnClose: TButton;
		LinkLabel1: TLinkLabel;
		imgLogo: TImage;
		Procedure FormCreate(Sender: TObject);
		Procedure FormClose(Sender: TObject; Var Action: TCloseAction);
		Procedure lblCopyrightLinkClick(Sender: TObject; Const Link: String; LinkType: TSysLinkType);
	Private
		{ Private declarations }
	Public
		{ Public declarations }
	End;

Implementation

Uses
	ImageResource;

{$R *.dfm}


Const
	ABOUT_HINT =
	  'Open source and free software.'#13#10 +
	  'Home Page:<a href="http://www.vmeisoft.com">http://www.vmeisoft.com</a>'#13#10 +
	  'Donate:<a href="http://www.vmeisoft.com/donate/converter.htm">http://www.vmeisoft.com/donate/converter.htm</a>'#13#10 +
	  'Source:<a href="http://code.google.com/p/vmeisoft-video-convertor-and-editor/">http://code.google.com/p/vmeisoft-video-convertor-and-editor/</a>'#13#10;

Procedure TfrmAbout.FormClose(Sender: TObject; Var Action: TCloseAction);
Begin
	Action := caFree;
End;

Procedure TfrmAbout.FormCreate(Sender: TObject);
Begin
	Self.Caption := Format('About "%s"', [APP_TITLE]);
	LinkLabel1.Caption := ABOUT_HINT + 'Version:' + GetApplicationVersionString();
	imgLogo.Picture.LoadFromFile(ResourcePath + LogoFile);
End;

Procedure TfrmAbout.lblCopyrightLinkClick(Sender: TObject; Const Link: String; LinkType: TSysLinkType);
Var
	Addr: WideString;
Begin
	Addr := Link;
	ShellExecute(Handle, 'open', PWideChar(Addr), '', '', SW_SHOWMAXIMIZED);
End;

End.
