Unit ShortcutControl;

Interface

Uses
	Winapi.Messages, Winapi.Windows, System.SysUtils, System.Classes, Vcl.Controls, Vcl.Graphics, Vcl.StdCtrls,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Type
	THotkeyEvent = Procedure(Sender: TObject; HotKey: DWORD) Of Object;

	TShortcutInput = Class(TCustomEdit)
	Private
		FHotKey  : DWORD;
		FOnHotkey: THotkeyEvent;
		Procedure SetHotKey(Value: DWORD);

	Public
		Constructor Create(AOwner: TComponent); Override;
		Property HotKey: DWORD Read FHotKey Write SetHotKey;
		Procedure SetDescText(Const Desc: String);

	Published
		Property OnHotkey: THotkeyEvent Read FOnHotkey Write FOnHotkey;

	Published
		Property OnEnter;
		Property OnExit;

	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TShortcutInput]);
End;

{ TShortcutInput }

Constructor TShortcutInput.Create(AOwner: TComponent);
Begin
	Inherited;
	ReadOnly := TRUE;
	AutoSize := TRUE;
	AutoSelect := FALSE;
	ImeMode := imDisable;
End;

Procedure TShortcutInput.SetHotKey(Value: DWORD);
Begin
	FHotKey := Value;
	If Assigned(FOnHotkey) Then FOnHotkey(Self, FHotKey);
End;

Procedure TShortcutInput.SetDescText(Const Desc: String);
Begin
	Self.Text := Desc;
	Self.SelStart := Length(Desc);
End;

End.
