unit GraphicsTrackBar;

interface

uses
    System.SysUtils, System.Classes, System.Types, Vcl.Controls, Vcl.ComCtrls, Vcl.Themes,
    Winapi.Messages, Winapi.Windows, Winapi.CommCtrl,
    Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

type
    TGraphicsTrackBar = class(TWinControl)
    private
        { Private declarations }
        FOnMouseDragBegin: TNotifyEvent;
        FOnMouseDragEnd  : TNotifyEvent;
        FMouseDrag       : Boolean;
        FPosition        : Integer;
        FMin             : Integer;
        FMax             : Integer;
        FOnChange        : TNotifyEvent;

        procedure SetMin(Value: Integer);
        procedure SetMax(Value: Integer);
        procedure SetPosition(Value: Integer);

    protected
        procedure CreateParams(var Params: TCreateParams); override;
        procedure CreateWnd; override;

    protected
        { Protected declarations }
        procedure WMLButtonDown(var Message: TWMLButtonDown); message WM_LBUTTONDOWN;
        procedure WMLButtonUp(var Message: TWMLButtonUp); message WM_LBUTTONUP;
        procedure WMCaptureChanged(var Message: TMessage); message WM_CAPTURECHANGED;
        procedure CNHScroll(var Message: TWMHScroll); message CN_HSCROLL;
        procedure CNVScroll(var Message: TWMVScroll); message CN_VSCROLL;

    public
        constructor Create(AOwner: TComponent); override;

    public
        { Public declarations }
        property Min: Integer read FMax write SetMin;
        property Max: Integer read FMax write SetMax;

    published
        { Published declarations }
        property OnMouseDragBegin: TNotifyEvent read FOnMouseDragBegin write FOnMouseDragBegin;
        property OnMouseDragEnd  : TNotifyEvent read FOnMouseDragEnd write FOnMouseDragEnd;
        property OnChange        : TNotifyEvent read FOnChange write FOnChange;
        property Position        : Integer read FPosition write SetPosition default 0;

    published
        property Align;
        property Anchors;
        property BorderWidth;
        property Ctl3D;
        property DoubleBuffered;
        property DragCursor;
        property DragKind;
        property DragMode;
        property Enabled;
        property Constraints;
        property ParentCtl3D;
        property ParentDoubleBuffered;
        property ParentShowHint;
        property Hint;
        property ShowHint;
        property TabOrder;
        property TabStop default True;
        property Visible;
    end;

procedure Register;

const
    GraphicsTrackBarHint =
      '-->: Forward one second;'#13#10 +
      '<--: Backward one second;'#13#10 +
      'Page Down: Forward 10 seconds;'#13#10 +
      'Page Up: Backward 10 seconds';

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsTrackBar]);
end;

constructor TGraphicsTrackBar.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    Width := 150;
    Height := 24;
    TabStop := True;
    FMin := 0;
    FMax := 100;
    FPosition := 0;
    ControlStyle := ControlStyle - [csDoubleClicks, csGestures];
end;

procedure TGraphicsTrackBar.CreateParams(var Params: TCreateParams);
begin
    InitCommonControl(ICC_BAR_CLASSES);
    inherited CreateParams(Params);
    CreateSubClass(Params, TRACKBAR_CLASS);
    Params.Style := Params.Style or TBS_BOTH or TBS_NOTICKS or TBS_HORZ or TBS_FIXEDLENGTH;
    Params.WindowClass.Style := Params.WindowClass.Style and not(CS_HREDRAW or CS_VREDRAW or CS_DBLCLKS);
end;

procedure TGraphicsTrackBar.CreateWnd;
begin
    inherited CreateWnd;
    if HandleAllocated then
    begin
        SendMessage(Handle, TBM_SETTHUMBLENGTH, 20, 0);
        SendMessage(Handle, TBM_SETRANGEMIN, 1, FMin);
        SendMessage(Handle, TBM_SETRANGEMAX, 1, FMax);
        SendMessage(Handle, TBM_SETPOS, 1, FPosition);
        SendMessage(Handle, TBM_SETPAGESIZE, 0, 10);
    end;
end;

procedure TGraphicsTrackBar.WMLButtonDown(var Message: TWMLButtonDown);
begin
    inherited;
    if (FMouseDrag = FALSE) and (Assigned(FOnMouseDragBegin)) then
    begin
        FMouseDrag := True;
        FOnMouseDragBegin(Self);
    end;
end;

procedure TGraphicsTrackBar.SetPosition(Value: Integer);
Begin
    if Value <> FPosition then
    begin
        FPosition := Value;
        if HandleAllocated then SendMessage(Handle, TBM_SETPOS, 1, Value);
    end;
End;

procedure TGraphicsTrackBar.SetMin(Value: Integer);
begin
    if Value <> FMin then
    begin
        FMin := Value;
        SendMessage(Handle, TBM_SETRANGEMIN, 1, Value);
    end;
end;

procedure TGraphicsTrackBar.SetMax(Value: Integer);
begin
    if Value <> FMax then
    begin
        FMax := Value;
        SendMessage(Handle, TBM_SETRANGEMAX, 1, Value);
    end;
END;

procedure TGraphicsTrackBar.WMLButtonUp(var Message: TWMLButtonUp);
begin
    inherited;
    if (FMouseDrag) and (Assigned(FOnMouseDragEnd)) then
    begin
        FMouseDrag := FALSE;
        FOnMouseDragEnd(Self);
    end;
end;

procedure TGraphicsTrackBar.WMCaptureChanged(var Message: TMessage);
begin
    inherited;
    if (FMouseDrag) and (Assigned(FOnMouseDragEnd)) then
    begin
        FMouseDrag := FALSE;
        FOnMouseDragEnd(Self);
    end;
end;

procedure TGraphicsTrackBar.CNHScroll(var Message: TWMHScroll);
var
    Position: Integer;
begin
    Message.Result := 0;
    Position := SendMessage(Handle, TBM_GETPOS, 0, 0);
    if FPosition = Position then Exit;
    FPosition := Position;
    if FMouseDrag then
    begin
        if Assigned(FOnChange) then
        begin
            FOnChange(Self);
        end;
    end else begin
        if Assigned(FOnMouseDragBegin) then
        begin
            FOnMouseDragBegin(Self);
        end;
        if Assigned(FOnChange) then
        begin
            FOnChange(Self);
        end;
        if Assigned(FOnMouseDragEnd) then
        begin
            FOnMouseDragEnd(Self);
        end;
    end;
end;

procedure TGraphicsTrackBar.CNVScroll(var Message: TWMVScroll);
begin
    Message.Result := 0;
end;


end.
