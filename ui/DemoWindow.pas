unit DemoWindow;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Winapi.GDIPAPI, Winapi.GDIPOBJ;

type
    TfrmDemo = class(TForm)
    private
        { Private declarations }
        procedure CreateParams(var Params: TCreateParams); override;
        procedure DrawBackground(Graphics: TGPGraphics);
        procedure DrawImage(Graphics: TGPGraphics);
    protected
        { Protected declarations }
        procedure WMEraseBkgnd(var Message: TWmEraseBkgnd); message WM_ERASEBKGND;
        procedure WMPaint(var Message: TWMPaint); message WM_PAINT;
    public
        { Public declarations }
        FImage: TGPBitmap;
        procedure Redraw();
    end;

implementation

{$R *.dfm}

{ TfrmDemo }

procedure TfrmDemo.CreateParams(var Params: TCreateParams);
begin
    inherited;
    Params.Style := WS_SIZEBOX;
    Params.ExStyle := WS_EX_NOACTIVATE;
    Params.WindowClass.Style := CS_VREDRAW or CS_HREDRAW;
end;

procedure TfrmDemo.WMEraseBkgnd(var Message: TWmEraseBkgnd);
begin
    Message.Result := 1;
end;

procedure TfrmDemo.DrawBackground(Graphics: TGPGraphics);
var
    brush: TGPSolidBrush;
    rc   : TGPRect;
begin
    rc.x := 0;
    rc.y := 0;
    rc.Width := ClientWidth;
    rc.Height := ClientHeight;
    brush := TGPSolidBrush.Create($FFFFFFFF);
    Graphics.FillRectangle(brush, rc);
    brush.Free();
end;

procedure TfrmDemo.DrawImage(Graphics: TGPGraphics);
var
    pen               : TGPPen;
    x, y, w, h, w1, h1: Integer;
    rc                : TGPRect;
begin
    if FImage = nil then exit;
    x := 0;
    y := 0;
    w := FImage.GetWidth();
    h := FImage.GetHeight();
    w1 := ClientWidth - 2;
    h1 := ClientHeight - 2;
    if (w <= w1) and (h <= h1) then
    begin
        x := (w1 - w) div 2;
        y := (h1 - h) div 2;
    end else if w * Height > Width * h then
    begin
        h := w1 * h div w;
        y := (h1 - h) div 2;
        w := w1;
    end else begin
        w := h1 * w div h;
        x := (w1 - w) div 2;
        h := h1;
    end;

    rc.x := x;
    rc.y := y;
    rc.Width := w + 1;
    rc.Height := h + 1;
    pen := TGPPen.Create($FFB8D6FB, 1);
    Graphics.DrawRectangle(pen, rc);
    pen.Free();

    Graphics.DrawImage(FImage, x + 1, y + 1, w, h);

end;

procedure TfrmDemo.WMPaint(var Message: TWMPaint);
var
    DC      : HDC;
    PS      : TPaintStruct;
    Graphics: TGPGraphics;
begin
    DC := BeginPaint(Handle, PS);
    Graphics := TGPGraphics.Create(DC);
    DrawBackground(Graphics);
    DrawImage(Graphics);
    Graphics.Free();
    EndPaint(Handle, PS);
end;

procedure TfrmDemo.Redraw();
var
    DC      : HDC;
    Graphics: TGPGraphics;
begin
    DC := GetDC(self.Handle);
    Graphics := TGPGraphics.Create(DC);
    DrawImage(Graphics);
    Graphics.Free();
    ReleaseDC(self.Handle, DC);
end;

end.
