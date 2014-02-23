unit Buy;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Winapi.ShellAPI,
  CommandLinkButton;

type
  TfrmBuy = class(TForm)
    Button1: TCommandLinkButton;
    Button2: TCommandLinkButton;
    procedure Button2Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmBuy: TfrmBuy;

implementation

uses
    Defines, ProductRegister;

{$R *.dfm}

procedure TfrmBuy.Button1Click(Sender: TObject);
begin
    ShellExecute(Handle, 'open', g_ProductInfo.m_Buy, '', '', SW_SHOWMAXIMIZED);
    Close();
end;

procedure TfrmBuy.Button2Click(Sender: TObject);
begin
    Close();
end;

procedure TfrmBuy.FormCreate(Sender: TObject);
begin
    Self.Caption := APP_TITLE;
    Button1.CommandLinkHint := Format('You can still try this software for %d days.', [GetExpireDays()]);
end;

end.
