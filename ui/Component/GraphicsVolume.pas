unit GraphicsVolume;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Winapi.Windows,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

const
    GV_WIDTH  = 16;
    GV_HEIGHT = 32;

type
    TGraphicsVolume = class(TCustomControl)
    private
        { Private declarations }
        FBitmap: TGPBitmap;
        FBuffer: Array [0 .. GV_WIDTH * GV_HEIGHT] of Integer;
        procedure DrawVolumeBar(Value: Integer);
    protected
        { Protected declarations }
        procedure Paint; override;
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        destructor Destroy; override;

        procedure SetVolume(Value: Integer);
    published
        { Published declarations }
        property Visible;
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsVolume]);
end;

const
    GRAY_COLOR: DWORD = $FFC0DCE2;
    LO_COLOR: DWORD   = $FF16C900;
    HI_COLOR: DWORD   = $FFFF0000;

var
    BarColors   : array [0 .. 9] of DWORD;
    BarColorInit: Boolean;

procedure InitBarColors();
var
    r, g, b   : DWORD;
    r1, g1, b1: DWORD;
    r2, g2, b2: DWORD;
    i         : DWORD;
begin
    if BarColorInit then Exit;
    BarColorInit := TRUE;
    r1 := (LO_COLOR shr 16) and $FF;
    g1 := (LO_COLOR shr 8) and $FF;
    b1 := (LO_COLOR shr 0) and $FF;
    r2 := (HI_COLOR shr 16) and $FF;
    g2 := (HI_COLOR shr 8) and $FF;
    b2 := (HI_COLOR shr 0) and $FF;
    for i := 0 to 9 do
    begin
        r := (r1 * (9 - i) + r2 * i) div 9;
        g := (g1 * (9 - i) + g2 * i) div 9;
        b := (b1 * (9 - i) + b2 * i) div 9;
        BarColors[i] := $FF000000 + (r shl 16) + (g shl 8) + (b);
    end;
end;

constructor TGraphicsVolume.Create(AOwner: TComponent);
var
    i: Integer;
begin
    inherited Create(AOwner);
    InitBarColors();
    Width := GV_WIDTH;
    Height := GV_HEIGHT;
    FBitmap := TGPBitmap.Create(GV_WIDTH, GV_HEIGHT, GV_WIDTH * 4, PixelFormat32bppRGB, @FBuffer[0]);
    for i := 0 to GV_WIDTH * GV_HEIGHT - 1 do
    begin
        FBuffer[i] := -1;
    end;
    DrawVolumeBar(0);
end;

destructor TGraphicsVolume.Destroy;
begin
    FBitmap.Free();
    inherited Destroy;
end;

procedure TGraphicsVolume.Paint();
var
    Graphics: TGPGraphics;
begin
    Graphics := TGPGraphics.Create(Self.Canvas.Handle);
    Graphics.DrawImage(FBitmap, 0, 0);
    Graphics.Free();
end;

procedure TGraphicsVolume.DrawVolumeBar(Value: Integer);
var
    i, j: Integer;
    c   : Integer;
    p   : Integer;
begin
    Value := (Value + 9) div 10;
    for i := 0 to 9 do
    begin
        if Value > i then
        begin
            c := BarColors[i];
        end else begin
            c := GRAY_COLOR;
        end;

        p := ((9 - i) * 3 + 1) * GV_WIDTH;
        for j := 0 to GV_WIDTH * 2 - 1 do
        begin
            FBuffer[p + j] := c;
        end;
    end;
end;

procedure TGraphicsVolume.SetVolume(Value: Integer);
begin
    DrawVolumeBar(Value);
    Self.Paint();
end;

end.
