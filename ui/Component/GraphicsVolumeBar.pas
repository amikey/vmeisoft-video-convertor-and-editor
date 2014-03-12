unit GraphicsVolumeBar;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Winapi.Windows,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

type
    TGraphicsVolumeBar = class(TCustomControl)
    private
        { Private declarations }
        FBak: TGPBitmap;
        FBar: TGPBitmap;
        FVol: Integer;
    protected
        { Protected declarations }
        procedure Paint; override;
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;

        procedure LoadImages(Bk, Bar: TGPBitmap);
        procedure SetVolume(Value: Integer);
    published
        { Published declarations }
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsVolumeBar]);
end;

constructor TGraphicsVolumeBar.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    Self.Width := 286;
    Self.Height := 22;
end;

procedure TGraphicsVolumeBar.Paint();
var
    Len     : Integer;
    Graphics: TGPGraphics;
begin
    if (FBak = nil) then Exit;
    if (FBar = nil) then Exit;

    Len := (280 * FVol) div 100;

    Graphics := TGPGraphics.Create(Self.Canvas.Handle);
    Graphics.DrawImage(FBak, 0, 0, 286, 22);
    Graphics.DrawImage(FBar, 3, 3, 0, 0, Len, 16, UnitPixel);
    Graphics.Free();
end;

procedure TGraphicsVolumeBar.LoadImages(Bk, Bar: TGPBitmap);
begin
    FBak := Bk;
    FBar := Bar;
end;

procedure TGraphicsVolumeBar.SetVolume(Value: Integer);
begin
    FVol := Value;
    Self.Invalidate();
end;

end.
