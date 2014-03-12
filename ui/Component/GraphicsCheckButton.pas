unit GraphicsCheckButton;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

type
    TGraphicsCheckButton = class(TSpeedButton)
    private
        { Private declarations }
        FDownImage: TGPBitmap;
        FUpImage  : TGPBitmap;
    protected
        { Protected declarations }
        procedure Paint; override;
    public
        { Public declarations }
        procedure SetImages(DownImage, UpImage: TGPBitmap);
    published
        { Published declarations }
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsCheckButton]);
end;

procedure TGraphicsCheckButton.Paint();
var
    x, y : Integer;
    image: TGPBitmap;
    Graphics: TGPGraphics;
begin
    inherited;
    if (Self.Down) then
    begin
        image := FDownImage;
    end else begin
        image := FUpImage;
    end;
    if image <> Nil then
    begin
        x := (Width - image.GetWidth()) div 2;
        y := (Height - image.GetHeight()) div 2;
        Graphics := TGPGraphics.Create(Self.Canvas.Handle);
        Graphics.DrawImage(image, x, y, image.GetWidth(), image.GetHeight());
        Graphics.Free();
    end;
end;

procedure TGraphicsCheckButton.SetImages(DownImage, UpImage: TGPBitmap);
begin
    FDownImage := DownImage;
    FUpImage := UpImage;
end;

end.
