Unit KeyTestControl;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Winapi.Messages, Winapi.Windows,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Type

	TOnTestMessage = Procedure(Var Msg: TMessage) Of Object;

	TKeyTest = Class(TWinControl)
	Private
		{ Private declarations }
		FOnTestMessage: TOnTestMessage;
	Protected
		{ Protected declarations }
		Procedure WndProc(Var Msg: TMessage); Override;
		Procedure WMEraseBkgnd(Var Message: TWmEraseBkgnd); Message WM_ERASEBKGND;
		Procedure WMPaint(Var Message: TWMPaint); Message WM_PAINT;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
	Published
		{ Published declarations }
		Property OnTestMessage: TOnTestMessage Read FOnTestMessage Write FOnTestMessage;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TKeyTest]);
End;

{ TKeyTest }

Constructor TKeyTest.Create(AOwner: TComponent);
Begin
	Inherited;
	TabStop := TRUE;
End;

Procedure TKeyTest.WMEraseBkgnd(Var Message: TWmEraseBkgnd);
Begin
	Message.Result := 1;
End;

Procedure TKeyTest.WMPaint(Var Message: TWMPaint);
Var
	DC      : HDC;
	PS      : TPaintStruct;
	Graphics: TGPGraphics;
	pen     : TGPPen;
	brush   : TGPSolidBrush;
	rc      : TGPRect;
Begin
	DC := BeginPaint(Handle, PS);
	Graphics := TGPGraphics.Create(DC);

	rc.x := 0;
	rc.y := 0;
	rc.Width := Width;
	rc.Height := Height;
	brush := TGPSolidBrush.Create($FFFFFFFF);
	Graphics.FillRectangle(brush, rc);
	brush.Free();

	Begin
		rc.x := 0;
		rc.y := 0;
		rc.Width := Width - 1;
		rc.Height := Height - 1;
		pen := TGPPen.Create($FFB8D6FB, 1);
		Graphics.DrawRectangle(pen, rc);
		pen.Free();
	End;

	Graphics.Free();

	EndPaint(Handle, PS);
End;

Procedure TKeyTest.WndProc(Var Msg: TMessage);
Begin
	If Assigned(FOnTestMessage) Then FOnTestMessage(Msg);
	Inherited;
End;

End.
