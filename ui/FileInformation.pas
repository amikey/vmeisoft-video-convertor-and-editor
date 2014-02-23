unit FileInformation;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.StdCtrls,
    VideoConverterInt;

type
    TfrmMediaInfo = class(TForm)
        btnClose: TButton;
        lblFile: TLabel;
        edFile: TEdit;
        PageControl1: TPageControl;
        Video: TTabSheet;
        Audio: TTabSheet;
        lblResolution: TLabel;
        edResolution: TEdit;
        lblFrameRate: TLabel;
        edFrameRate: TEdit;
        lblVideoBitRate: TLabel;
        edVideoBitRate: TEdit;
        lbledSamples: TLabel;
        edSamples: TEdit;
        lblChannels: TLabel;
        edChannels: TEdit;
        lblAudioBitRate: TLabel;
        edAudioBitRate: TEdit;
        procedure FormCreate(Sender: TObject);
    private
        { Private declarations }
    public
        { Public declarations }
        procedure InitForm(p: PMediaInfo);
    end;


implementation

{$R *.dfm}


procedure TfrmMediaInfo.FormCreate(Sender: TObject);
begin
    PageControl1.ActivePageIndex := 0;
end;

procedure TfrmMediaInfo.InitForm(p: PMediaInfo);
begin
    edFile.Text := p.m_szFileName;
    if p.m_bVideoStream <> 0 then
    begin
        edResolution.Text := Format('%d x %d', [p.m_Width, p.m_Height]);

        if p.m_FrameRateDen = 1 then
        begin
            edFrameRate.Text := Format('%d fps', [p.m_FrameRateNum]);
        end else begin
            edFrameRate.Text := Format('%.2f fps', [p.m_FrameRateNum / p.m_FrameRateDen]);
        end;

        if p.m_VideoBitRate < 1000000 then
        begin
            if (p.m_VideoBitRate mod 1000) = 0 then
            begin
                edVideoBitRate.Text := Format('%d Kbps', [p.m_VideoBitRate div 1000]);
            end else begin
                edVideoBitRate.Text := Format('%.1f Kbps', [p.m_VideoBitRate / 1000.0]);
            end;
        end else begin
            if (p.m_VideoBitRate mod 1000000) = 0 then
            begin
                edVideoBitRate.Text := Format('%d Mbps', [p.m_VideoBitRate div 1000000]);
            end else begin
                edVideoBitRate.Text := Format('%.1f Mbps', [p.m_VideoBitRate / 1000000.0]);
            end;
        end;

    end else begin
        Video.TabVisible := FALSE;
    end;

    if p.m_bVideoStream <> 0 then
    begin
        case p.m_SampleRate of
            8000: edSamples.Text := 'Phone Quality (8000)';
            22050: edSamples.Text := 'Low Quality (22050)';
            44100: edSamples.Text := 'CD Quality (44100)';
            48000: edSamples.Text := 'DVD Quality (48000)';
        end;

        case p.m_nChannel of
            1: edChannels.Text := 'Mono';
            2: edChannels.Text := 'Stereo';
        end;

        if p.m_AudioBitRate < 1000000 then
        begin
            if (p.m_AudioBitRate mod 1000) = 0 then
            begin
                edAudioBitRate.Text := Format('%d Kbps', [p.m_AudioBitRate div 1000]);
            end else begin
                edAudioBitRate.Text := Format('%.1f Kbps', [p.m_AudioBitRate / 1000.0]);
            end;
        end else begin
            if (p.m_AudioBitRate mod 1000000) = 0 then
            begin
                edAudioBitRate.Text := Format('%d Mbps', [p.m_AudioBitRate div 1000000]);
            end else begin
                edAudioBitRate.Text := Format('%.1f Mbps', [p.m_AudioBitRate / 1000000.0]);
            end;
        end;

    end else begin
        Audio.TabVisible := FALSE;
    end;

end;

end.
