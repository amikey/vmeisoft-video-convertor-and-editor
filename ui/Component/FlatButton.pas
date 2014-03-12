Unit FlatButton;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Vcl.StdCtrls, Winapi.Messages, Winapi.CommCtrl, Winapi.Windows, Vcl.Themes,
	Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

Type
	TFlatButton = Class(TWinControl)
	Private
		{ Private declarations }
		FChecked     : Boolean;
		FImage       : TGPBitmap;
		FImageChecked: TGPBitmap;
		Procedure SetChecked(Value: Boolean);
	Protected
		{ Protected declarations }
        Procedure WMEraseBkgnd(Var Message: TWmEraseBkgnd); Message WM_ERASEBKGND;
		Procedure CNNotify(Var Message: TWMNotify); Message CN_NOTIFY;
		Procedure CNCommand(Var Message: TWMCommand); Message CN_COMMAND;
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Procedure SetImage(Image, ImageChecked: TGPBitmap);
	Published
		{ Published declarations }
		Property Visible;
		Property Checked: Boolean Read FChecked Write SetChecked;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TFlatButton]);
End;

Constructor TFlatButton.Create(AOwner: TComponent);
Begin
	Inherited Create(AOwner);
    ControlStyle := [csReflector, csPannable, csOpaque, csOverrideStylePaint];
End;

Procedure TFlatButton.CreateParams(Var Params: TCreateParams);
Begin
	Inherited CreateParams(Params);
	CreateSubClass(Params, 'BUTTON');
	Params.Style := WS_CHILD Or BS_AUTOCHECKBOX Or BS_PUSHLIKE Or BS_FLAT;
   	Params.ExStyle := WS_EX_TRANSPARENT;
    Params.WindowClass.hbrBackground := 0;
End;

Procedure TFlatButton.CreateWnd;
Begin
	Inherited;
	SendMessage(Handle, BM_SETCHECK, WPARAM(FChecked), 0);
End;

Procedure TFlatButton.SetChecked(Value: Boolean);
Begin
	If FChecked <> Value Then
	Begin
		FChecked := Value;
		If HandleAllocated Then
		Begin
			SendMessage(Handle, BM_SETCHECK, WPARAM(Checked), 0);
		End;
	End;
End;

Procedure TFlatButton.SetImage(Image, ImageChecked: TGPBitmap);
Begin
	FImage := Image;
	FImageChecked := ImageChecked;
	If Self.HandleAllocated Then
	Begin
		Self.Invalidate();
	End;
End;

Procedure TFlatButton.CNCommand(Var Message: TWMCommand);
Begin
	If Message.NotifyCode = BN_CLICKED Then
	Begin
		FChecked := SendMessage(Handle, BM_GETCHECK, 0, 0) = BST_CHECKED;
	End;
End;

Procedure TFlatButton.WMEraseBkgnd(Var Message: TWmEraseBkgnd);
Begin
	Message.Result := 1;
End;

Procedure TFlatButton.CNNotify(Var Message: TWMNotify);
Var
	CustomDraw    : PNMCustomDraw;
	x, y          : Integer;
	Graphics      : TGPGraphics;
	Image         : TGPBitmap;
	MouseInControl: Boolean;
	PaintRect     : TRect;
	Pen           : TGPPen;
Begin
	If Message.NMHdr.code = NM_CUSTOMDRAW Then
	Begin
		CustomDraw := PNMCustomDraw(Message.NMHdr);
		If CustomDraw.dwDrawStage = CDDS_PREERASE Then
		Begin
			PaintRect := ClientRect;
			MouseInControl := PtInRect(ClientRect, ScreenToClient(Mouse.CursorPos));

			If SendMessage(Handle, BM_GETSTATE, 0, 0) And BST_PUSHED = BST_PUSHED Then
			Begin
				//DrawEdge(CustomDraw.hdc, PaintRect, EDGE_SUNKEN, BF_RECT)
                DrawFrameControl(CustomDraw.hdc, PaintRect, DFC_BUTTON, DFCS_PUSHED Or DFCS_BUTTONPUSH Or DFCS_ADJUSTRECT);
			End Else If MouseInControl Then
			Begin
				//DrawEdge(CustomDraw.hdc, PaintRect, EDGE_RAISED, BF_RECT)
                DrawFrameControl(CustomDraw.hdc, PaintRect, DFC_BUTTON, DFCS_BUTTONPUSH Or DFCS_ADJUSTRECT);
			End;
			(*
			  If MouseInControl Then
			  Begin
			  Graphics := TGPGraphics.Create(CustomDraw.hdc);
			  Pen := TGPPen.Create($60000000, 1);
			  Graphics.DrawRectangle(Pen, 0, 0, Width - 1, Height - 1);
			  Pen.Free();
			  Graphics.Free();
			  End;
			*)
			// DrawFrameControl(CustomDraw.hdc, ClientRect, DFC_BUTTON, DFCS_BUTTONPUSH Or DFCS_ADJUSTRECT);
		End Else If CustomDraw.dwDrawStage = CDDS_PREPAINT Then
		Begin
			If FChecked Then
			Begin
				Image := FImage;
			End Else Begin
				Image := FImageChecked;
			End;
			If FImage = Nil Then
			Begin
				x := (Width - FImage.GetWidth()) Div 2;
				y := (Height - FImage.GetHeight()) Div 2;
				Graphics := TGPGraphics.Create(CustomDraw.hdc);
				Graphics.DrawImage(Image, x, y, Image.GetWidth(), Image.GetHeight());
				Graphics.Free();
			End;
		End;
		Message.Result := CDRF_SKIPDEFAULT;
		Exit;
	End;
	Inherited;
End;

End.
