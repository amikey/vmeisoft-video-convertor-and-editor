Unit GraphicsButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.StdCtrls, Winapi.Messages, Winapi.CommCtrl, Winapi.Windows,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

Type
	TGraphicsButton = Class(TButton)
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
		Property Visible;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TGraphicsButton]);
End;

Constructor TGraphicsButton.Create(AOwner: TComponent);
Begin
	Inherited Create(AOwner);
    ImageIndex := 0;
End;

Destructor TGraphicsButton.Destroy;
Begin
	If FImageGrey <> Nil Then FImageGrey.Free();
	Inherited;
End;

Procedure TGraphicsButton.SetImage(Image: TGPBitmap);
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

Procedure TGraphicsButton.CNNotify(Var Message: TWMNotify);
Var
	CustomDraw: PNMCustomDraw;
	x, y      : Integer;
	Graphics  : TGPGraphics;
	Image     : TGPBitmap;
Begin
	Inherited;
	If FImage = Nil Then exit;
	If Message.NMHdr.code = NM_CUSTOMDRAW Then
	Begin
		CustomDraw := PNMCustomDraw(Message.NMHdr);
		If CustomDraw.dwDrawStage = CDDS_PREPAINT Then
		Begin
			Message.Result := CDRF_NOTIFYPOSTPAINT;
		End Else If CustomDraw.dwDrawStage = CDDS_POSTPAINT Then
		Begin
			If Caption <> '' Then
			Begin
				x := 6;
			End Else Begin
				x := (Width - FImage.GetWidth()) Div 2;
			End;
			y := (Height - FImage.GetHeight()) Div 2;
			If Self.Enabled Then
			Begin
				Image := FImage;
			End Else Begin
				If FImageGrey = Nil Then FImageGrey := CreateGreyImage(FImage);
				Image := FImageGrey;
			End;
			Graphics := TGPGraphics.Create(CustomDraw.hdc);
			Graphics.DrawImage(Image, x, y, Image.GetWidth(), Image.GetHeight());
			Graphics.Free();
		End;
	End;
End;

End.
