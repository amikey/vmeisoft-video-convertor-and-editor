Unit WinPaint;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Winapi.Messages, Winapi.Windows,
	System.UITypes, Vcl.Graphics,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Type
	TWinPaintEvent = Procedure(DC: HDC) Of Object;

	TWinPaint = Class(TWinControl)
	Private
		{ Private declarations }
		FOnPaint: TWinPaintEvent;
	Protected
		{ Protected declarations }
		//Procedure WMEraseBkgnd(Var Message: TWmEraseBkgnd); Message WM_ERASEBKGND;
		Procedure WMPaint(Var Message: TWMPaint); Message WM_PAINT;
	Protected
		Procedure CreateParams(Var Params: TCreateParams); Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
	Published
		{ Published declarations }
		Property OnPaint: TWinPaintEvent Read FOnPaint Write FOnPaint;
		Property OnMouseDown;
		Property OnMouseMove;
		Property OnMouseUp;
		Property OnMouseWheel;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TWinPaint]);
End;

{ TWinPaint }

Constructor TWinPaint.Create(AOwner: TComponent);
Begin
	Inherited;
	//ControlStyle := [csAcceptsControls, csReflector, csPannable, csOpaque, csOverrideStylePaint];
	TabStop := TRUE;
	//ParentDoubleBuffered := FALSE;
	//DoubleBuffered := FALSE;
End;

Procedure TWinPaint.CreateParams(Var Params: TCreateParams);
Begin
	Inherited;
    (*
	Params.WindowClass.hbrBackground := 0;
	Params.WindowClass.style := CS_OWNDC;
	Params.style := WS_CHILD Or WS_TABSTOP Or WS_CLIPSIBLINGS Or WS_CLIPCHILDREN;
	Params.ExStyle := WS_EX_TRANSPARENT;
    *)
End;
      (*
Procedure TWinPaint.WMEraseBkgnd(Var Message: TWmEraseBkgnd);
Begin
	If Assigned(FOnPaint) Then FOnPaint(Message.DC);
	Message.Result := 1;
End;
        *)
Procedure TWinPaint.WMPaint(Var Message: TWMPaint);
Var
	DC: HDC;
	PS: TPaintStruct;
Begin
	DC := BeginPaint(Handle, PS);
	If csDesigning In ComponentState Then
	Begin
		Winapi.Windows.Rectangle(DC, 0, 0, Width, Height);
	End;
	If Assigned(FOnPaint) Then FOnPaint(DC);
	EndPaint(Handle, PS);
End;

End.
