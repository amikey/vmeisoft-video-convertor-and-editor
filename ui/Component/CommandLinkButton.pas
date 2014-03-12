Unit CommandLinkButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.StdCtrls,
	Winapi.Windows, Winapi.Messages, Winapi.CommCtrl, Winapi.GDIPAPI, Winapi.GDIPOBJ,
	GraphicsUtils;

Type
	TCommandLinkButton = Class(TButton)
	Private
		{ Private declarations }
		FImage    : TGPBitmap;
		FImageGrey: TGPBitmap;
	Protected
		{ Protected declarations }
		Procedure CNNotify(Var Message: TWMNotify); Message CN_NOTIFY;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Destructor Destroy; Override;
		Procedure SetImage(Image: TGPBitmap);
	Published
		{ Published declarations }
		Property Caption;
		Property CommandLinkHint;
		Property Images;
		Property Enabled;
		Property OnClick;
		Property TabOrder;
		Property TabStop Default True;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TCommandLinkButton]);
End;

Constructor TCommandLinkButton.Create(AOwner: TComponent);
Begin
	Inherited Create(AOwner);
	If Win32MajorVersion > 5 Then
	Begin
		Self.Style := bsCommandLink;
	End;
	ImageIndex := 0;
	TabStop := True;
End;

Destructor TCommandLinkButton.Destroy;
Begin
	If FImageGrey <> Nil Then FImageGrey.Free();
	Inherited;
End;

Procedure TCommandLinkButton.SetImage(Image: TGPBitmap);
Begin
	FImage := Image;
	If FImageGrey <> Nil Then
	Begin
		FImageGrey.Free();
		FImageGrey := Nil;
	End;
	If Self.HandleAllocated Then
	Begin
		Self.Invalidate();
	End;
End;

Procedure TCommandLinkButton.CNNotify(Var Message: TWMNotify);
Var
	CustomDraw: PNMCustomDraw;
	x, y      : Integer;
	Graphics  : TGPGraphics;
	font      : TGPFont;
	origin    : TGPPointF;
	brush     : TGPSolidBrush;
	Image     : TGPBitmap;
Begin
	Inherited;
	If Message.NMHdr.code = NM_CUSTOMDRAW Then
	Begin
		CustomDraw := PNMCustomDraw(Message.NMHdr);
		If CustomDraw.dwDrawStage = CDDS_PREPAINT Then
		Begin
			Message.Result := CDRF_NOTIFYPOSTPAINT;
		End Else If CustomDraw.dwDrawStage = CDDS_POSTPAINT Then
		Begin

			Graphics := TGPGraphics.Create(CustomDraw.hdc);
			Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
			If FImage <> Nil Then
			Begin
				x := 6;
				y := (Height - FImage.GetHeight()) Div 2;
				If Self.Enabled Then
				Begin
					Image := FImage;
				End Else Begin
					If FImageGrey = Nil Then FImageGrey := CreateGreyImage(FImage);
					Image := FImageGrey;
				End;
				Graphics.DrawImage(Image, x, y, Image.GetWidth(), Image.GetHeight());
			End Else If Win32MajorVersion < 6 Then
			Begin
				If Self.Enabled Then
				Begin
					Image := GetGoImage();
				End Else Begin
					Image := GetGoGreyImage();
				End;
				x := 6;
				y := 10;
				Graphics.DrawImage(Image, x, y, Image.GetWidth(), Image.GetHeight());
			End;

			If Win32MajorVersion < 6 Then
			Begin
				Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
				font := TGPFont.Create(Self.font.Name, 11);
				origin.y := 10;
				If Self.Images <> Nil Then
				Begin
					origin.x := 8 + Self.Images.Width;
				End Else Begin
					origin.x := 8 + 17;
				End;
				If Self.Enabled Then
				Begin
					brush := TGPSolidBrush.Create($FF151C71);
				End Else Begin
					brush := TGPSolidBrush.Create($7F000000);
				End;
				Graphics.DrawString(Self.Caption, -1, font, origin, brush);
				font.Free();
				font := TGPFont.Create(Self.font.Name, 9);
				origin.y := origin.y + 20;
				Graphics.DrawString(Self.CommandLinkHint, -1, font, origin, brush);
				Message.Result := CDRF_SKIPDEFAULT;
				font.Free();
				brush.Free();
			End Else Begin
				Message.Result := CDRF_DODEFAULT;
			End;

			Graphics.Free();
		End;
	End;
End;

End.
