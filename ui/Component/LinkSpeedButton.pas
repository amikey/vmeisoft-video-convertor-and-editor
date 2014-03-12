Unit LinkSpeedButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons, Winapi.Windows,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

Type
	TLinkSpeedButton = Class(TSpeedButton) // TGraphicControl
	Private
		{ Private declarations }
		FTitle: String;
	Protected
		{ Protected declarations }
		Procedure Paint; Override;
		Procedure SetTitle(value: String);
		Procedure AdjustBounds;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
	Published
		{ Published declarations }
		Property Title: String Read FTitle Write SetTitle;
	End;

Procedure Register;

Implementation

Const
	FONT_SIZE = 9;

Procedure Register;
Begin
	RegisterComponents('Samples', [TLinkSpeedButton]);
End;

Procedure TLinkSpeedButton.AdjustBounds;
Var
	DC         : HDC;
	Graphics   : TGPGraphics;
	font       : TGPFont;
	origin     : TGPPointF;
	boundingBox: TGPRectF;
Begin
	If Not((csReading In ComponentState) Or (csDesigning In ComponentState)) Then
	Begin
		DC := GetDC(0);
		Graphics := TGPGraphics.Create(Self.Canvas.Handle);
		font := TGPFont.Create(Self.font.Name, FONT_SIZE, FontStyleRegular);
		origin.x := 0;
		origin.y := 0;
		Graphics.MeasureString(FTitle, -1, font, origin, boundingBox);
		Self.Width := Round(boundingBox.Width + 7.0);
		Self.Height := Round(boundingBox.Height + 7.0);
		ReleaseDC(0, DC);
	End;
End;

Constructor TLinkSpeedButton.Create(AOwner: TComponent);
Begin
	Inherited;
	Self.Flat := Not(csDesigning In ComponentState);
	Self.Cursor := crHandPoint;
End;

Procedure TLinkSpeedButton.Paint();
Var
	brush   : TGPSolidBrush;
	font    : TGPFont;
	Graphics: TGPGraphics;
	origin  : TGPPointF;

	PaintRect: TRect;
Begin

	PaintRect := Rect(0, 0, Width, Height);
	If FState In [bsDown, bsExclusive] Then
	Begin
		DrawEdge(Canvas.Handle, PaintRect, BDR_SUNKEN, BF_RECT);
	End Else If MouseInControl Then
	Begin
		DrawEdge(Canvas.Handle, PaintRect, BDR_RAISED, BF_RECT);
	End Else Begin
		DrawEdge(Canvas.Handle, PaintRect, BDR_RAISEDOUTER, BF_RECT Or BF_FLAT);
	End;

	If FTitle = '' Then Exit;
	If MouseInControl Then
	Begin
		font := TGPFont.Create(Self.font.Name, FONT_SIZE, FontStyleUnderline);
	End Else Begin
		font := TGPFont.Create(Self.font.Name, FONT_SIZE, FontStyleRegular);
	End;
	brush := TGPSolidBrush.Create($FF0000FF);
	Graphics := TGPGraphics.Create(Self.Canvas.Handle);
	origin.x := 3;
	origin.y := 3;
	Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	Graphics.DrawString(FTitle, -1, font, origin, brush);
	font.Free();
	brush.Free();
	Graphics.Free();
End;

Procedure TLinkSpeedButton.SetTitle(value: String);
Begin
	If FTitle <> value Then
	Begin
		FTitle := value;
		AdjustBounds();
		Invalidate();
	End;
End;

End.
