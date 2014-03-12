Unit GraphicsUtils;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.ExtCtrls, Vcl.Graphics, Vcl.Forms,
	Vcl.Consts, Vcl.Themes, Vcl.Buttons,
	Winapi.Windows, Winapi.GDIPAPI, Winapi.GDIPOBJ;

Const
	R_COEF: DWORD = 19595; // 65536 * 0.299 = 19595
	G_COEF: DWORD = 38469; // 65536 * 0.587 = 38469
	B_COEF: DWORD = 7471;  // 65536 * 0.114 = 7471

Function CreateGreyImage(Image: TGPBitmap): TGPBitmap;
Function GetGoImage(): TGPBitmap;
Function GetGoGreyImage(): TGPBitmap;
Function CreateDorpdownButton(): TGPBitmap;
Function BGRtoRGB(Color: DWORD): DWORD; Inline;
Function GetButtonTextBGR(): DWORD;

Implementation

Var
	imageGo    : TGPBitmap;
	imageGoGrey: TGPBitmap;

Function CreateGreyImage(Image: TGPBitmap): TGPBitmap;
Var
	Graphics        : TGPGraphics;
	rect            : TGPRect;
	lockedBitmapData: TBitmapData;
	p               : PBYTE;
	i               : Integer;
	v               : DWORD;
Begin
	Result := TGPBitmap.Create(Image.GetWidth(), Image.GetHeight(), PixelFormat32bppARGB);
	Graphics := TGPGraphics.Create(Result);
	Graphics.DrawImage(Image, 0, 0, Image.GetWidth(), Image.GetHeight());
	Graphics.Free();

	rect.X := 0;
	rect.Y := 0;
	rect.Width := Image.GetWidth();
	rect.Height := Image.GetHeight();
	Result.LockBits(rect, ImageLockModeWrite, PixelFormat32bppARGB, lockedBitmapData);
	p := lockedBitmapData.Scan0;
	For i := 0 To rect.Width * rect.Height - 1 Do
	Begin
		v := (p[0] * B_COEF + p[1] * G_COEF + p[2] * R_COEF) Shr 17;
		v := v + 120;
		// if v > 255 then v := 255;
		p[0] := v;
		p[1] := v;
		p[2] := v;
		p := p + 4;
	End;
	Result.UnlockBits(lockedBitmapData);
End;

Function GetResourcePath(): String;
Begin
	Result := ExtractFilePath(Application.ExeName);
	Result := Result + 'Resource\';
End;

Function LoadImageToBitmap(FileName: String): TGPBitmap;
Var
	Image   : TGPImage;
	Graphics: TGPGraphics;
Begin
	Image := TGPImage.Create(FileName);
	If Image.GetLastStatus() <> Ok Then
	Begin
		Application.MessageBox(PWideChar(WideString(FileName)), 'Faild to load file.', MB_ICONERROR);
	End;
	Result := TGPBitmap.Create(Image.GetWidth(), Image.GetHeight(), PixelFormat32bppARGB);
	Graphics := TGPGraphics.Create(Result);
	Graphics.DrawImage(Image, 0, 0, Image.GetWidth(), Image.GetHeight());

	Graphics.Free();
	Image.Free();
End;

Function GetGoImage(): TGPBitmap;
Begin
	If imageGo = Nil Then
	Begin
		imageGo := LoadImageToBitmap(GetResourcePath() + 'Go.png');
	End;
	Result := imageGo;
End;

Function GetGoGreyImage(): TGPBitmap;
Begin
	If imageGoGrey <> Nil Then
	Begin
		Result := imageGoGrey;
		Exit;
	End;
	If imageGo = Nil Then
	Begin
		GetGoImage();
	End;
	imageGoGrey := CreateGreyImage(imageGo);
	Result := imageGoGrey;
End;

Const
	B                                         = $FF000000;
	TickData: Array [0 .. 9 * 5 - 1] Of DWORD =
	  (
	  B, B, B, B, B, B, B, B, B,
	  0, B, B, B, B, B, B, B, 0,
	  0, 0, B, B, B, B, B, 0, 0,
	  0, 0, 0, B, B, B, 0, 0, 0,
	  0, 0, 0, 0, B, 0, 0, 0, 0
	  );

Function CreateDorpdownButton(): TGPBitmap;
Begin
	Result := TGPBitmap.Create(9, 5, 9 * 4, PixelFormat32bppARGB, @TickData);
End;

Function BGRtoRGB(Color: DWORD): DWORD;
Begin
	Result :=
	  ((Color And $FF0000) Shr 16) Or
	  (Color And $00FF00) Or
	  ((Color And $FF) Shl 16);
End;

Function CheckButtonTextColor(Color: DWORD):DWORD;
Var
	Color1: DWORD;
Begin
    Color1 :=
	  ((Color And $FF0000) Shr 16) +
	  ((Color And $00FF00) Shr 8) +
	  ((Color And $FF));
    if Color1 < 3 * $80 then
    begin
        Result := 0;
    end else begin
		Result := $FFFFFF;
    end;
End;

Function GetButtonTextBGR(): DWORD;
Begin
	If (StyleServices.Enabled) And (StyleServices.IsSystemStyle = false) Then
	Begin
		Result := BGRtoRGB(ColorToRGB(StyleServices.GetSystemColor(clBtnText)));
        Result := CheckButtonTextColor(Result);
	End Else Begin
		Result := BGRtoRGB(Vcl.Graphics.ColorToRGB(clBtnText));
	End;
End;

End.
