unit TimeEdit;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls,
    Vcl.ComCtrls, Winapi.CommCtrl;

type
    TTimeEditEvent = procedure(Sender: TObject; time: Int64) of object;

    TTimeEdit = class(TCustomPanel)
    private
        { Private declarations }
        FHour       : Integer;
        FMinute     : Integer;
        FSecond     : Integer;
        FMSecond    : Integer;
        FHourEdit   : TEdit;
        FMinuteEdit : TEdit;
        FSecondEdit : TEdit;
        FMSecondEdit: TEdit;
        FLabel1     : TLabel;
        FLabel2     : TLabel;
        FLabel3     : TLabel;
        FUpDown     : TUpDown;
        FFocus      : Integer;
        FTimeValue  : Int64;
        FOnChange   : TTimeEditEvent;
        FMinTime    : Int64;
        FMaxTime    : Int64;
    private
        procedure Increment();
        procedure Decrement();
    protected
        { Protected declarations }
        procedure OnEditEnter(Sender: TObject);
        procedure OnUpDownClick(Sender: TObject; Button: TUDBtnType);
        procedure OnEditKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
        procedure OnEditChange(Sender: TObject);
        Function GetTimeValue(): Int64;
        Procedure SetTimeValue(Value: Int64);
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
    published
        { Published declarations }
        property OnChange : TTimeEditEvent read FOnChange write FOnChange;
        property MaxTime  : Int64 read FMaxTime write FMaxTime;
        property MinTime  : Int64 read FMinTime write FMinTime;
        property TimeValue: Int64 read GetTimeValue write SetTimeValue;
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TTimeEdit]);
end;

{ TTimeEdit }

const
    AV_TIME_BASE: Int64  = 1000000;
    MAX_EDIT_TIME: Int64 = Int64(24) * Int64(3600) * Int64(1000000);

function IntToStr2(Value: Integer): String;
begin
    Result := IntToStr(Value);
    if Value < 10 then Result := '0' + Result;
end;

constructor TTimeEdit.Create(AOwner: TComponent);
begin
    inherited;
    FMinTime := 0;
    FMaxTime := MAX_EDIT_TIME;
    self.ParentBackground := FALSE;
    self.ParentColor := FALSE;
    self.Color := clWindow;
    self.BevelOuter := bvNone;
    self.Width := 80;
    self.Height := 16;
    self.Caption := '';
    self.BevelKind := bkFlat;
    self.AutoSize := TRUE;

    FHourEdit := TEdit.Create(self);
    FMinuteEdit := TEdit.Create(self);
    FSecondEdit := TEdit.Create(self);
    FMSecondEdit := TEdit.Create(self);
    FLabel1 := TLabel.Create(self);
    FLabel2 := TLabel.Create(self);
    FLabel3 := TLabel.Create(self);
    FUpDown := TUpDown.Create(self);

    FHourEdit.Parent := self;
    FMinuteEdit.Parent := self;
    FSecondEdit.Parent := self;
    FMSecondEdit.Parent := self;
    FLabel1.Parent := self;
    FLabel2.Parent := self;
    FLabel3.Parent := self;
    FUpDown.Parent := self;

    FHourEdit.Top := 4;
    FMinuteEdit.Top := 4;
    FSecondEdit.Top := 4;
    FMSecondEdit.Top := 4;
    FLabel1.Top := 4;
    FLabel2.Top := 4;
    FLabel3.Top := 4;
    FUpDown.Top := 0;

    FHourEdit.Width := 16;
    FMinuteEdit.Width := 16;
    FSecondEdit.Width := 16;
    FMSecondEdit.Width := 8;
    FLabel1.Width := 7;
    FLabel2.Width := 7;
    FLabel3.Width := 7;
    FUpDown.Width := 16;

    FHourEdit.Height := 16;
    FMinuteEdit.Height := 16;
    FSecondEdit.Height := 16;
    FMSecondEdit.Height := 16;
    FLabel1.Height := 16;
    FLabel2.Height := 16;
    FLabel3.Height := 16;
    FUpDown.Height := 24;

    FHourEdit.Left := 2;
    FLabel1.Left := 2 + 2 + 16;
    FMinuteEdit.Left := 2 + 16 + 8;
    FLabel2.Left := 2 + 2 + 16 + 8 + 16;
    FSecondEdit.Left := 2 + 16 + 8 + 16 + 8;
    FLabel3.Left := 2 + 2 + 16 + 8 + 16 + 8 + 16;
    FMSecondEdit.Left := 2 + 16 + 8 + 16 + 8 + 16 + 8;
    FUpDown.Left := 2 + 16 + 8 + 16 + 8 + 16 + 8 + 8 + 4;

    FHourEdit.Tag := 0;
    FMinuteEdit.Tag := 1;
    FSecondEdit.Tag := 2;
    FMSecondEdit.Tag := 3;
    FHourEdit.BorderStyle := bsNone;
    FMinuteEdit.BorderStyle := bsNone;
    FSecondEdit.BorderStyle := bsNone;
    FMSecondEdit.BorderStyle := bsNone;
    FHourEdit.Alignment := taRightJustify;
    FMinuteEdit.Alignment := taRightJustify;
    FSecondEdit.Alignment := taRightJustify;
    FMSecondEdit.Alignment := taRightJustify;
    FMinuteEdit.NumbersOnly := TRUE;
    FMinuteEdit.NumbersOnly := TRUE;
    FSecondEdit.NumbersOnly := TRUE;
    FMSecondEdit.NumbersOnly := TRUE;
    FMinuteEdit.MaxLength := 2;
    FMinuteEdit.MaxLength := 2;
    FSecondEdit.MaxLength := 2;
    FMSecondEdit.MaxLength := 1;

    FLabel1.AutoSize := TRUE;
    FLabel2.AutoSize := TRUE;
    FLabel3.AutoSize := TRUE;

    FHourEdit.Text := '00';
    FMinuteEdit.Text := '00';
    FSecondEdit.Text := '00';
    FMSecondEdit.Text := '0';
    FLabel1.Caption := ':';
    FLabel2.Caption := ':';
    FLabel3.Caption := '.';
    FHourEdit.Visible := TRUE;
    FMinuteEdit.Visible := TRUE;
    FSecondEdit.Visible := TRUE;
    FUpDown.Visible := TRUE;

    FHourEdit.OnEnter := OnEditEnter;
    FMinuteEdit.OnEnter := OnEditEnter;
    FSecondEdit.OnEnter := OnEditEnter;
    FMSecondEdit.OnEnter := OnEditEnter;
    FHourEdit.OnKeyDown := OnEditKeyDown;
    FMinuteEdit.OnKeyDown := OnEditKeyDown;
    FSecondEdit.OnKeyDown := OnEditKeyDown;
    FMSecondEdit.OnKeyDown := OnEditKeyDown;
    FHourEdit.OnChange := OnEditChange;
    FMinuteEdit.OnChange := OnEditChange;
    FSecondEdit.OnChange := OnEditChange;
    FMSecondEdit.OnChange := OnEditChange;

    FUpDown.OnClick := OnUpDownClick;

end;

function TTimeEdit.GetTimeValue: Int64;
begin
    Result := FTimeValue;
end;

procedure TTimeEdit.SetTimeValue(Value: Int64);
var
    t: Int64;
begin
    if FTimeValue = Value then Exit;
    FTimeValue := Value;
    t := Value;
    t := t div (100000);
    FMSecond := t mod 10;
    t := t div 10;
    FSecond := t mod 60;
    t := t div 60;
    FMinute := t mod 60;
    t := t div 60;
    FHour := t;
    FHourEdit.Text := IntToStr2(FHour);
    FMinuteEdit.Text := IntToStr2(FMinute);
    FSecondEdit.Text := IntToStr2(FSecond);
    FMSecondEdit.Text := IntToStr(FMSecond);
end;

procedure TTimeEdit.Decrement();
begin
    case FFocus of
        0:
            begin
                if FHour = 0 then Exit;
                FHour := FHour - 1;
                FHourEdit.Text := IntToStr2(FHour);
            end;
        1:
            begin
                if FMinute = 0 then Exit;
                FMinute := FMinute - 1;
                FMinuteEdit.Text := IntToStr2(FMinute);
            end;
        2:
            begin
                if FSecond = 0 then Exit;
                FSecond := FSecond - 1;
                FSecondEdit.Text := IntToStr2(FSecond);
            end;
        3:
            begin
                if FMSecond = 0 then Exit;
                FMSecond := FMSecond - 1;
                FMSecondEdit.Text := IntToStr(FMSecond);
            end;
    end;
    FTimeValue := (FHour * 3600 + FMinute * 60 + FSecond) * AV_TIME_BASE + FMSecond * AV_TIME_BASE div 10;
    if FTimeValue < FMinTime then
    begin
        FTimeValue := -1;
        SetTimeValue(FMinTime);
    end;
    if Assigned(FOnChange) then FOnChange(self, FTimeValue);
end;

procedure TTimeEdit.Increment();
begin
    case FFocus of
        0:
            begin
                FHour := FHour + 1;
                FHourEdit.Text := IntToStr2(FHour);
            end;
        1:
            begin
                if FMinute = 59 then Exit;
                FMinute := FMinute + 1;
                FMinuteEdit.Text := IntToStr2(FMinute);
            end;
        2:
            begin
                if FSecond = 59 then Exit;
                FSecond := FSecond + 1;
                FSecondEdit.Text := IntToStr2(FSecond);
            end;
        3:
            begin
                if FMSecond = 9 then Exit;
                FMSecond := FMSecond + 1;
                FMSecondEdit.Text := IntToStr(FMSecond);
            end;
    end;
    FTimeValue := (FHour * 3600 + FMinute * 60 + FSecond) * AV_TIME_BASE + FMSecond * AV_TIME_BASE div 10;
    if FTimeValue > FMaxTime then
    begin
        FTimeValue := -1;
        SetTimeValue(FMaxTime);
    end;
    if Assigned(FOnChange) then FOnChange(self, FTimeValue);
end;

procedure TTimeEdit.OnEditEnter(Sender: TObject);
var
    Edit: TEdit;
begin
    Edit := Sender as TEdit;
    FFocus := Edit.Tag;
end;

procedure TTimeEdit.OnEditKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
var
    StartPos, EndPos: Integer;
begin
    case Key of
        VK_UP: Increment();
        VK_DOWN: Decrement();
        VK_LEFT:
            begin
                if FFocus = 0 then Exit;
                SendMessage((Sender as TEdit).Handle, EM_GETSEL, WPARAM(@StartPos), LPARAM(@EndPos));
                if (StartPos = 0) and (EndPos = 0) then
                begin
                    case FFocus of
                        1:
                            begin
                                FFocus := 0;
                                FHourEdit.SetFocus();
                            end;
                        2:
                            begin
                                FFocus := 1;
                                FMinuteEdit.SetFocus();
                            end;
                        3:
                            begin
                                FFocus := 2;
                                FSecondEdit.SetFocus();
                            end;
                    end;
                end;
            end;
        VK_RIGHT:
            begin
                if FFocus = 3 then Exit;
                SendMessage((Sender as TEdit).Handle, EM_GETSEL, WPARAM(@StartPos), LPARAM(@EndPos));
                if (StartPos = 2) and (EndPos = 2) then
                begin
                    case FFocus of
                        0:
                            begin
                                FFocus := 1;
                                FMinuteEdit.SetFocus();
                            end;
                        1:
                            begin
                                FFocus := 2;
                                FSecondEdit.SetFocus();
                            end;
                        2:
                            begin
                                FFocus := 3;
                                FMSecondEdit.SetFocus();
                            end;
                    end;
                end;
            end;
    end;
end;

procedure TTimeEdit.OnUpDownClick(Sender: TObject; Button: TUDBtnType);
begin
    if Button = btNext then
    begin
        Increment();
    end else
    begin
        Decrement();
    end;
end;

procedure TTimeEdit.OnEditChange(Sender: TObject);
var
    Edit : TEdit;
    Value: Integer;
begin
    Edit := Sender as TEdit;
    FFocus := Edit.Tag;
    if Edit.Text = '' then Exit;
    Value := StrToInt(Edit.Text);
    if Value < 0 then Exit;

    case FFocus of
        0:
            begin
                FHour := Value;
            end;
        1:
            begin
                if Value > 59 then
                begin
                    Value := 59;
                    Edit.Text := IntToStr2(Value);
                end;
                FMinute := Value;
            end;
        2:
            begin
                if Value > 59 then
                begin
                    Value := 59;
                    Edit.Text := IntToStr2(Value);
                end;
                FSecond := Value;
            end;
        3:
            begin
                if Value > 9 then
                begin
                    Value := 9;
                    Edit.Text := IntToStr2(Value);
                end;
                FMSecond := Value;
            end;
    end;
    if FTimeValue < FMinTime then
    begin
        FTimeValue := -1;
        SetTimeValue(FMinTime);
    end;
    if FTimeValue > FMaxTime then
    begin
        FTimeValue := -1;
        SetTimeValue(FMaxTime);
    end;
    if Assigned(FOnChange) then FOnChange(self, FTimeValue);
end;

end.
