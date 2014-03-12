Unit GraphicsDownButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.StdCtrls, Winapi.Windows, Winapi.Messages, Winapi.CommCtrl,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Type
	TGraphicsDownButton = Class(TCheckBox)
	Private
		{ Private declarations }
		FImage: TGPBitmap;
	Protected
		{ Protected declarations }
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CNNotify(Var Message: TWMNotify); Message CN_NOTIFY;
	Public
		{ Public declarations }
		Procedure SetImage(Image: TGPBitmap);
	Published
		{ Published declarations }
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TGraphicsDownButton]);
End;

Procedure TGraphicsDownButton.CreateParams(Var Params: TCreateParams);
Begin
	Inherited;
	Params.Style := Params.Style Or BS_PUSHLIKE Or BS_FLAT;
End;

Procedure TGraphicsDownButton.SetImage(Image: TGPBitmap);
Begin
	FImage := Image;
	If Self.HandleAllocated Then
	Begin
		Self.Invalidate();
	End;
End;

Procedure TGraphicsDownButton.CNNotify(Var Message: TWMNotify);
Var
	CustomDraw: PNMCustomDraw;
	x, y      : Integer;
	Graphics  : TGPGraphics;
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
			y := y + 2;
			Graphics := TGPGraphics.Create(CustomDraw.hdc);
			Graphics.DrawImage(FImage, x, y, FImage.GetWidth(), FImage.GetHeight());
			Graphics.Free();
		End;
	End;
End;

End.
