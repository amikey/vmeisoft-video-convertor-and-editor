Unit TimerPaintBox;

Interface

Uses
	System.SysUtils, System.Classes, Winapi.Windows, Vcl.Controls, Vcl.ExtCtrls;

Type
	TTimerPaintBox = Class(TPaintBox)
	Private
		{ Private declarations }
	Protected
		{ Protected declarations }
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
	Published
		{ Published declarations }
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TTimerPaintBox]);
End;

{ TTimerPaintBox }
Constructor TTimerPaintBox.Create(AOwner: TComponent);
Begin
	Inherited;
	ControlStyle := [csReplicatable, csCaptureMouse, csOpaque, csGestures];
End;

End.
