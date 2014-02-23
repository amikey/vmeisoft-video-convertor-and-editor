unit SelectMode;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
    TfrmSelectMode = class(TForm)
        btnOK: TButton;
        btnCancel: TButton;
        btnEach: TRadioButton;
        btnMerge: TRadioButton;
        procedure RadioButtonClick(Sender: TObject);
    private
        { Private declarations }
    public
        { Public declarations }
    end;

var
    g_merge_mode  : Boolean;

implementation

{$R *.dfm}


uses VideoConverterInt, CommonData;

procedure TfrmSelectMode.RadioButtonClick(Sender: TObject);
begin
    g_merge_mode := btnMerge.Checked;
end;

end.
