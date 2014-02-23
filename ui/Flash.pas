unit Flash;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.StdCtrls, Functions, Vcl.Imaging.pngimage;

type
    TfrmFlash = class(TForm)
        lblVersion: TLabel;
        lblName: TLabel;
        imgLogo: TImage;
        procedure FormCreate(Sender: TObject);
    private
        { Private declarations }
    public
        { Public declarations }
    end;

var
    frmFlash: TfrmFlash;

implementation

uses
    Defines, ImageResource;

{$R *.dfm}

procedure TfrmFlash.FormCreate(Sender: TObject);
begin
    lblName.Caption := APP_TITLE;
    lblVersion.Caption := 'Version:' + GetApplicationVersionString();
    imgLogo.Picture.LoadFromFile(ResourcePath + LogoFile);
end;

end.
