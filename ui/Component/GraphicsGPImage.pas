Unit GraphicsGPImage;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.ExtCtrls, Vcl.Graphics,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Type
	TGraphicsGPImage = Class(TGraphicControl)
	Private
		{ Private declarations }
		FNoBorder: Boolean;
		FImage   : TGPBitmap;
	Protected
		{ Protected declarations }
		Procedure Paint; Override;
	Public
		{ Public declarations }
		Procedure SetImage(Image: TGPBitmap);

	Published
		{ Published declarations }
        Property NoBorder: Boolean read FNoBorder write FNoBorder default false;
		Property Align;
		Property Enabled;
		Property OnClick;
		Property Visible;
		Property OnMouseDown;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TGraphicsGPImage]);
End;

Procedure TGraphicsGPImage.SetImage(Image: TGPBitmap);
Begin
	FImage := Image;
	self.Invalidate();
End;

Procedure TGraphicsGPImage.Paint();
Var
	Graphics          : TGPGraphics;
	pen               : TGPPen;
	rc                : TGPRect;
	x, y, w, h, w1, h1: Single;
Begin
	Graphics := TGPGraphics.Create(self.Canvas.Handle);

    if FNoBorder = false then
	Begin
		rc.x := 0;
		rc.y := 0;
		rc.Width := Width - 1;
		rc.Height := Height - 1;
		pen := TGPPen.Create($FFB8D6FB, 1);
		Graphics.DrawRectangle(pen, rc);
		pen.Free();
	End;

	If FImage <> Nil Then
	Begin
		x := 0;
		y := 0;
		w := FImage.GetWidth();
		h := FImage.GetHeight();
		w1 := Width - 2;
		h1 := Height - 2;
		If (w <= w1) And (h <= h1) Then
		Begin
			x := (w1 - w) / 2;
			y := (h1 - h) / 2;
		End Else If w * Height > Width * h Then
		Begin
			h := w1 * h / w;
			y := (h1 - h) / 2;
			w := w1;
		End Else Begin
			w := h1 * w / h;
			x := (w1 - w) / 2;
			h := h1;
		End;

		Graphics.DrawImage(FImage, x + 1, y + 1, w, h);
	End;

	Graphics.Free();
End;

End.
