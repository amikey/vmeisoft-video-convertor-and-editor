unit ImageResource;

interface

uses
    System.SysUtils, System.Classes, Winapi.Windows, Vcl.Controls, Vcl.Forms,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

const
    LogoFile = 'Logo.png';

var
    imageAddMovie: TGPBitmap;
    imageAddFile : TGPBitmap;
    imageDelete  : TGPBitmap;
    imageOptions : TGPBitmap;

    imagePlay    : TGPBitmap;
    imagePause   : TGPBitmap;
    imageClip    : TGPBitmap;
    imageCut     : TGPBitmap;
    imageEffect  : TGPBitmap;
    imageSubtitle: TGPBitmap;
    imageAudio   : TGPBitmap;
    imageRedo    : TGPBitmap;

    imageStart  : TGPBitmap;
    imageInfo   : TGPBitmap;
    imageCapture: TGPBitmap;
    imageReset  : TGPBitmap;

    imageGraphicsAlignBottom  : TGPBitmap;
    imageGraphicsAlignCenter  : TGPBitmap;
    imageGraphicsAlignCentered: TGPBitmap;
    imageGraphicsAlignLeft    : TGPBitmap;
    imageGraphicsAlignRight   : TGPBitmap;
    imageGraphicsAlignTop     : TGPBitmap;

    imageRotateLeft    : TGPBitmap;
    imageRotateRight   : TGPBitmap;
    imageFlipHorizontal: TGPBitmap;
    imageFlipVertical  : TGPBitmap;
    imageAspect        : TGPBitmap;

    imageZoom: TGPBitmap;

    imageHD    : TGPBitmap;
    imageVideo : TGPBitmap;
    imageDevice: TGPBitmap;
    imageWeb   : TGPBitmap;
    imageApp   : TGPBitmap;
    imageMusic : TGPBitmap;

    imageEditSubtitle: TGPBitmap;
    imageAddSubtitle : TGPBitmap;
    imageImport      : TGPBitmap;
    imageSettings    : TGPBitmap;

    imagePrevious: TGPBitmap;
    imageNext    : TGPBitmap;

    imageRecord: TGPBitmap;
    imageStop  : TGPBitmap;

var
    imageProcess: TGPBitmap;

function LoadImageToBitmap(FileName: String): TGPBitmap;
function LoadThumbailImage(FileName: String; Width, Height: Integer): TGPBitmap;
procedure LoadImageResources();

implementation

uses
    Defines, Functions;

function LoadImageToBitmap(FileName: String): TGPBitmap;
var
    image   : TGPImage;
    graphics: TGPGraphics;
begin
    image := TGPImage.Create(FileName);
    if image.GetLastStatus() <> Ok then
    begin
        Application.MessageBox(PWideChar(WideString(FileName)), 'Faild to load file.', MB_ICONERROR);
    end;
    Result := TGPBitmap.Create(image.GetWidth(), image.GetHeight(), PixelFormat32bppARGB);
    graphics := TGPGraphics.Create(Result);
    graphics.DrawImage(image, 0, 0, image.GetWidth(), image.GetHeight());

    graphics.Free();
    image.Free();
end;

function LoadThumbailImage(FileName: String; Width, Height: Integer): TGPBitmap;
var
    image   : TGPImage;
    graphics: TGPGraphics;
begin
    image := TGPImage.Create(FileName);
    if image.GetLastStatus() <> Ok then
    begin
        Application.MessageBox(PWideChar(WideString(FileName)), 'Faild to load file.', MB_ICONERROR);
    end;
    if image.GetWidth() * Height > image.GetHeight() * Width then
    begin
        Height := Width * image.GetHeight() div image.GetWidth(); // x / 96 = h / w
    end else begin
        Width := Height * image.GetWidth() div image.GetHeight();
    end;

    Result := TGPBitmap.Create(Width, Height, PixelFormat32bppARGB);
    graphics := TGPGraphics.Create(Result);
    graphics.SetInterpolationMode(InterpolationModeHighQuality);
    graphics.DrawImage(image, 0, 0, Width, Height);
    graphics.Free;
    image.Free;
end;

procedure LoadImageResources();
begin
    imageAddMovie := LoadImageToBitmap(ResourcePath + 'AddMovie.png');
    imageOptions := LoadImageToBitmap(ResourcePath + 'Options.png');
    imageAddFile := LoadImageToBitmap(ResourcePath + 'AddFile.png');
    imageDelete := LoadImageToBitmap(ResourcePath + 'Delete.png');

    imagePlay := LoadImageToBitmap(ResourcePath + 'Play.png');
    imagePause := LoadImageToBitmap(ResourcePath + 'Pause.png');
    imageClip := LoadImageToBitmap(ResourcePath + 'Clip.png');
    imageStart := LoadImageToBitmap(ResourcePath + 'Start.png');

    imageInfo := LoadImageToBitmap(ResourcePath + 'Info.png');
    imageCut := LoadImageToBitmap(ResourcePath + 'Cut.png');
    imageEffect := LoadImageToBitmap(ResourcePath + 'Effect.png');
    imageSubtitle := LoadImageToBitmap(ResourcePath + 'Subtitle.png');
    imageAudio := LoadImageToBitmap(ResourcePath + 'Audio.png');
    imageRedo := LoadImageToBitmap(ResourcePath + 'Redo.png');

    imageCapture := LoadImageToBitmap(ResourcePath + 'Capture.png');
    imageReset := LoadImageToBitmap(ResourcePath + 'Reset.png');

    imageGraphicsAlignBottom := LoadImageToBitmap(ResourcePath + 'graphics-align-bottom.png');
    imageGraphicsAlignCenter := LoadImageToBitmap(ResourcePath + 'graphics-align-center.png');
    imageGraphicsAlignCentered := LoadImageToBitmap(ResourcePath + 'graphics-align-centered.png');
    imageGraphicsAlignLeft := LoadImageToBitmap(ResourcePath + 'graphics-align-left.png');
    imageGraphicsAlignRight := LoadImageToBitmap(ResourcePath + 'graphics-align-right.png');
    imageGraphicsAlignTop := LoadImageToBitmap(ResourcePath + 'graphics-align-top.png');

    imageRotateLeft := LoadImageToBitmap(ResourcePath + 'RotateLeft.png');
    imageRotateRight := LoadImageToBitmap(ResourcePath + 'RotateRight.png');
    imageFlipHorizontal := LoadImageToBitmap(ResourcePath + 'FlipHorizontal.png');
    imageFlipVertical := LoadImageToBitmap(ResourcePath + 'FlipVertical.png');

    imageAspect := LoadImageToBitmap(ResourcePath + 'Aspect.png');

    imageZoom := LoadImageToBitmap(ResourcePath + 'Zoom.png');

    imageHD := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\HD.png');
    imageVideo := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\Video.png');
    imageDevice := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\Device.png');
    imageWeb := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\Web.png');
    imageApp := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\Application.png');
    imageMusic := LoadImageToBitmap(ResourcePath + '\Profile\Thumbnails\Groups\Music.png');

    imageProcess := LoadImageToBitmap(ResourcePath + 'Process.png');

    imageEditSubtitle := LoadImageToBitmap(ResourcePath + 'EditSubtitle.png');
    imageAddSubtitle := LoadImageToBitmap(ResourcePath + 'AddSubtitle.png');
    imageImport := LoadImageToBitmap(ResourcePath + 'Import.png');
    imageSettings := LoadImageToBitmap(ResourcePath + 'Settings.png');

    imagePrevious := LoadImageToBitmap(ResourcePath + 'Previous.png');
    imageNext := LoadImageToBitmap(ResourcePath + 'Next.png');

    imageRecord := LoadImageToBitmap(ResourcePath + 'Record.png');
    imageStop := LoadImageToBitmap(ResourcePath + 'Stop.png');

end;

end.
