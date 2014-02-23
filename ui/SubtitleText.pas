unit SubtitleText;

interface

uses Winapi.Windows, System.SysUtils, System.Classes, Vcl.Graphics, Vcl.Forms,
    Vcl.Controls, Vcl.StdCtrls, Vcl.Buttons;

type
    TfrmSubtitleText = class(TForm)
        lblText: TLabel;
        EditText: TEdit;
        btnOK: TButton;
        btnCancel: TButton;
    private
        { Private declarations }
    public
        { Public declarations }
    end;

implementation

{$R *.dfm}

end.
