unit GraphicsCommandLink;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Vcl.Buttons,
    Winapi.Windows, Winapi.GDIPAPI, Winapi.GDIPOBJ, GraphicsUtils;

type
    TGraphicsCommandLink = class(TSpeedButton)
    private
        { Private declarations }
        FImage      : TGPBitmap;
        FImageGrey  : TGPBitmap;
        FTitle      : String;
        FDescription: String;

    protected
        { Protected declarations }
        procedure Paint; override;
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        destructor Destroy; override;
        procedure SetImage(Image: TGPBitmap; Title, Description: String);
    published
        { Published declarations }
    end;

procedure Register;

implementation

const
    TITLE_FONT_NAME = 'Arial Black';
    TITLE_FONT_SIZE = 11;
    DESC_FONT_NAME  = 'Tahoma';
    DESC_FONT_SIZE  = 9;

procedure Register;
begin
    RegisterComponents('Samples', [TGraphicsCommandLink]);
end;

constructor TGraphicsCommandLink.Create(AOwner: TComponent);
begin
    inherited;
    Self.Flat := not(csDesigning in ComponentState);
end;

destructor TGraphicsCommandLink.Destroy;
begin
    if FImageGrey <> nil then FImageGrey.Free();
    inherited;
end;

procedure TGraphicsCommandLink.Paint();
var
    x, y    : Integer;
    Image   : TGPBitmap;
    Graphics: TGPGraphics;

    font  : TGPFont;
    origin: TGPPointF;
    brush : TGPSolidBrush;

begin
    inherited;

    Graphics := TGPGraphics.Create(Self.Canvas.Handle);

    if Self.Enabled then
    begin
        Image := FImage;
    end else begin
        if FImageGrey = nil then FImageGrey := CreateGreyImage(FImage);
        Image := FImageGrey;
    end;
    if Image <> Nil then
    begin
        x := 5;
        y := (Height - Image.GetHeight()) div 2;
        Graphics.DrawImage(Image, x, y, Image.GetWidth(), Image.GetHeight());
    end;

    origin.y := 10;
    if FImage <> nil then
    begin
        origin.x := 8 + FImage.GetWidth();
    end else begin
        origin.x := 8 + 17;
    end;

    if FTitle <> '' then
    begin
        if Self.Enabled then
        begin
            brush := TGPSolidBrush.Create(GetButtonTextBGR() + $FF000000);
        end else begin
            brush := TGPSolidBrush.Create(GetButtonTextBGR() + $7F000000);
        end;
        font := TGPFont.Create(TITLE_FONT_NAME, TITLE_FONT_SIZE);
        Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        Graphics.DrawString(FTitle, -1, font, origin, brush);
        font.Free();
        brush.Free();
    end;

    if FDescription <> '' then
    begin
        if Self.Enabled then
        begin
            brush := TGPSolidBrush.Create($FF151C71);
        end else begin
            brush := TGPSolidBrush.Create($7F000000);
        end;
        font := TGPFont.Create(DESC_FONT_NAME, DESC_FONT_SIZE);
        origin.y := origin.y + 20;
        Graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        Graphics.DrawString(FDescription, -1, font, origin, brush);
        font.Free();
        brush.Free();
    end;

    Graphics.Free();

end;

function IntMax(a, b: Integer): Integer;
begin
    if a > b then
    begin
        result := a;
    end else begin
        result := b;
    end;
end;

function MeasureString(str: String; FontName: String; FontSize: Integer): Integer;
var
    dc       : HDC;
    Graphics : TGPGraphics;
    font     : TGPFont;
    origin   : TGPPointF;
    boundRect: TGPRectF;
begin
    dc := GetDC(0);
    Graphics := TGPGraphics.Create(dc);
    ReleaseDC(0, dc);
    Graphics.SetPageUnit(UnitPixel);
    font := TGPFont.Create(FontName, FontSize);
    origin := MakePoint(0.0, 0.0);
    boundRect := MakeRect(0.0, 0.0, 0.0, 0.0);
    Graphics.MeasureString(str, length(str), font, origin, boundRect);
    result := Trunc(boundRect.Width + 0.9);
    Graphics.Free();
end;

procedure TGraphicsCommandLink.SetImage(Image: TGPBitmap; Title, Description: String);
var
    w     : Integer;
    w1, w2: Integer;
begin
    FImage := Image;
    FTitle := Title;
    FDescription := Description;
    if FImageGrey <> nil then
    begin
        FImageGrey.Free();
        FImageGrey := nil;
    end;
    w := 8;
    if FImage <> nil then
    begin
        w := w + FImage.GetWidth();
    end;
    if FTitle <> '' then
    begin
        w1 := MeasureString(FTitle, TITLE_FONT_NAME, TITLE_FONT_SIZE);
    end else begin
        w1 := 0;
    end;
    if FDescription <> '' then
    begin
        w2 := MeasureString(FDescription, DESC_FONT_NAME, DESC_FONT_SIZE);
    end else begin
        w2 := 0;
    end;
    w := w + IntMax(w1, w2) + 4;
    Self.Width := w;
    Self.Invalidate();
end;

end.
