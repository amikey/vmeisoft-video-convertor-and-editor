unit GraphicsGroupRadio;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

type
    TGraphicsGroupRadio = class(TSpeedButton)
    private
        { Private declarations }
        FImage    : TGPBitmap;
        FBtnText  : String;
        FImageLeft: Integer;
    protected
        { Protected declarations }
        procedure Paint; override;
    public
        { Public declarations }
        procedure SetImage(Image: TGPBitmap; BtnText: String);

    published
        { Published declarations }
        property ImageLeft: Integer read FImageLeft write FImageLeft default 0;
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsGroupRadio]);
end;

procedure TGraphicsGroupRadio.Paint();
var
    x, y    : Integer;
    Graphics: TGPGraphics;
begin
    inherited;

    if (FImage <> nil) then
    begin
        if FImageLeft <> 0 then
        begin
            x := FImageLeft;
        end else
        begin
            x := (Width - FImage.GetWidth()) div 2;
        end;
        y := (Height - FImage.GetHeight()) div 2;
        Graphics := TGPGraphics.Create(Self.Canvas.Handle);
        Graphics.DrawImage(FImage, x, y, FImage.GetWidth(), FImage.GetHeight());
        Graphics.Free();
    end else begin
        x := 0;
    end;

    if FBtnText <> '' then
    begin
        x := x + FImage.GetWidth() + 16;
        y := (Height - Canvas.TextHeight(FBtnText)) div 2;
        Canvas.TextOut(x, y, FBtnText);
    end;
end;

procedure TGraphicsGroupRadio.SetImage(Image: TGPBitmap; BtnText: String);
begin
    FImage := Image;
    FBtnText := BtnText;
end;

end.
