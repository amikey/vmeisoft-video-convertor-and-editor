Unit GraphicsSpeedButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

Type
	TGraphicsSpeedButton = Class(TSpeedButton)
	Private
		{ Private declarations }
		FImage                   : TGPBitmap;
		FImageFire               : TGPBitmap;
		FImageGrey               : TGPBitmap;
		FImageTemp               : TGPBitmap;
		FImageWidth, FImageHeight: Integer;
	Protected
		{ Protected declarations }
		Procedure Paint; Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Destructor Destroy; Override;
		Procedure SetImage(Image: TGPBitmap);
		Procedure SetDisableImage(Image: TGPBitmap);
		Procedure SetFireImage(ImageFire: TGPBitmap);
	Published
		{ Published declarations }
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TGraphicsSpeedButton]);
End;

Constructor TGraphicsSpeedButton.Create(AOwner: TComponent);
Begin
	Inherited;
	Self.Flat := Not(csDesigning In ComponentState);
End;

Destructor TGraphicsSpeedButton.Destroy;
Begin
	If FImageTemp <> Nil Then FImageTemp.Free();
	Inherited;
End;

Procedure TGraphicsSpeedButton.Paint();
Var
	x, y    : Integer;
	Image   : TGPBitmap;
	Graphics: TGPGraphics;
Begin
	Inherited;
	If Self.Enabled Then
	Begin
		Image := FImage;
		If MouseInControl And (FImageFire <> Nil) Then Image := FImageFire;
	End Else Begin
		If (FImage <> Nil) And (FImageGrey = Nil) Then
		Begin
			FImageGrey := CreateGreyImage(FImage);
            FImageTemp := FImageGrey;
		End;
		Image := FImageGrey;
	End;
	If Image <> Nil Then
	Begin
		Graphics := TGPGraphics.Create(Self.Canvas.Handle);
		x := (Width - FImageWidth) Div 2;
		y := (Height - FImageHeight) Div 2;
		Graphics.DrawImage(Image, x, y, FImageWidth, FImageHeight);
		Graphics.Free();
	End;
End;

Procedure TGraphicsSpeedButton.SetDisableImage(Image: TGPBitmap);
Begin
	FImageGrey := Image;
End;

Procedure TGraphicsSpeedButton.SetFireImage(ImageFire: TGPBitmap);
Begin
	FImageFire := ImageFire;
End;

Procedure TGraphicsSpeedButton.SetImage(Image: TGPBitmap);
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

End.
