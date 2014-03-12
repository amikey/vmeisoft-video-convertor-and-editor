Unit GraphicsSpeedButtonEx;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons, Vcl.Graphics, Vcl.Themes,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

Type
	TGraphicsSpeedButtonEx = Class(TSpeedButton)
	Private
		{ Private declarations }
		FImage                   : TGPBitmap;
		FImageGrey               : TGPBitmap;
		FTitle                   : String;
		FImageWidth, FImageHeight: Integer;
	Protected
		{ Protected declarations }
		Procedure Paint; Override;
	Private
		FGPFont: TGPFont;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Destructor Destroy; Override;
		Procedure SetImage(Image: TGPBitmap);
		Procedure SetTitle(Title: String);
	Published
		{ Published declarations }
	End;

Procedure Register;

Implementation

Const
	TITLE_FONT_NAME = 'Tahoma';
	TITLE_FONT_SIZE = 9;

Procedure Register;
Begin
	RegisterComponents('Samples', [TGraphicsSpeedButtonEx]);
End;

Constructor TGraphicsSpeedButtonEx.Create(AOwner: TComponent);
Begin
	Inherited;
	Self.Flat := Not(csDesigning In ComponentState);
End;

Destructor TGraphicsSpeedButtonEx.Destroy;
Begin
	If FImageGrey <> Nil Then FImageGrey.Free();
	If FGPFont <> Nil Then FGPFont.Free();
	Inherited;
End;

Procedure TGraphicsSpeedButtonEx.Paint();
Var
	x, y        : Integer;
	Image       : TGPBitmap;
	Graphics    : TGPGraphics;
	layoutRect  : TGPRectF;
	stringFormat: TGPStringFormat;
	brush       : TGPBrush;
Begin
	Inherited;
	If Self.Enabled Then
	Begin
		Image := FImage;
	End Else Begin
		If (FImage <> Nil) And (FImageGrey = Nil) Then
		Begin
			FImageGrey := CreateGreyImage(FImage);
		End;
		Image := FImageGrey;
	End;
	Graphics := TGPGraphics.Create(Self.Canvas.Handle);
	If Image <> Nil Then
	Begin
		x := 5;
		y := (Height - Image.GetHeight()) Div 2;
		Graphics.DrawImage(Image, x, y, FImageWidth, FImageHeight);
	End;
	If FTitle <> '' Then
	Begin
		If Self.Enabled Then
		Begin
			brush := TGPSolidBrush.Create(GetButtonTextBGR() Or $FF000000);
		End Else Begin
			brush := TGPSolidBrush.Create(GetButtonTextBGR() Or $7F000000);
		End;

		layoutRect := MakeRect(5 + FImageWidth, 0.0, Width - 5 - FImageWidth, Height - 1);
		stringFormat := TGPStringFormat.Create();
		stringFormat.SetLineAlignment(StringAlignmentCenter);
		Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
		Graphics.DrawString(FTitle, -1, FGPFont, layoutRect, stringFormat, brush);
		stringFormat.Free();
		brush.Free();
	End;
	Graphics.Free();
End;

Procedure TGraphicsSpeedButtonEx.SetImage(Image: TGPBitmap);
Begin
	FImage := Image;
	If FImageGrey <> Nil Then
	Begin
		FImageGrey.Free();
		FImageGrey := Nil;
	End;
	If FImage <> Nil Then
	Begin
		FImageWidth := FImage.GetWidth();
		FImageHeight := FImage.GetHeight();
	End;
	Self.Invalidate();
End;

Procedure TGraphicsSpeedButtonEx.SetTitle(Title: String);
Begin
	FTitle := Title;
	If (FTitle <> '') And (FGPFont = Nil) Then
	Begin
		FGPFont := TGPFont.Create(Self.font.Name, Self.font.Size, FontStyleRegular);
	End;
	Self.Invalidate();
End;

End.
