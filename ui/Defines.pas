Unit Defines;

Interface

Uses
	Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
	Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons;

Const
	FAILD_INITIALIZE   = 'Failed to initialize.';
	FAILD_OPEN         = 'Failed to open the file.';
	FAILED_FOUND_FLASH = 'Error occurs, please contact us.';
	UPDATE_NOW         = 'New version detected, do you want to update?';
	NO_IMAGE_FILE      = 'Does not support the file or the file is not a valid image file.';
	ORI_SIZE           = 'Use the original video size:<a id="original">%d x %d</a>'#13'Click <a id="crop">here to crop video.</a>';
	ABORT_EXPORT       = 'Are you sure to abort the recording?';

	FOLDER_HINT = 'Output Folder(Click link to open folder):'#13#10'<a>%s</a>';

Const
	DevelopAddress  = 'http://www.vmeisoft.com/develop/';
	SuggestAddress  = 'http://www.vmeisoft.com/suggest/';
	RegisterAddress = 'http://www.vmeisoft.com/register/thanks.php';

Var
	APP_TITLE: String = 'VMeisoft Video Converter and Video Editor';

Const
	WATR_HINT = 'Video output size:%d*%d, Watermark image size:%d*%d';
	AD_HINT   = 'Video output size:%d*%d, Advertisement image size:%d*%d';

Var
	g_IsRegistered: Integer; // 0-Trail; 1-Registered; 2-Free

Const
	SELECT_WATERMARK = 'Select Watermark Picture';
	SELECT_AD        = 'Select Advertisement Picture';

Const
	PICTURE_FILTERS = 'All|*.gif;*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff;*.ico;*.emf;*.wmf|' +
	  'JPEG Image File|*.jpg;*.jpeg|' +
	  'Bitmaps|*.bmp|' +
	  'Portable Network Graphics|*.png|' +
	  'GIF Image|*.gif|' +
	  'TIFF Images|*.tif;*.tiff|' +
	  'Icons|*.ico|' +
	  'Enhanced Metafiles|*.emf|' +
	  'Metafiles|*.wmf' +
	  'All Files|*.*';

	PICTURE_ENCODER_FILTERS =
	  'Bitmap|*.bmp|' +
	  'JPEG Image File |*.jpg|' +
	  'Portable Network Graphics|*.png|' +
	  'TIFF Images|*.tif|' +
	  'GIF Image|*.gif';

	AUDIO_FILTERS =
	  '*.3ga;*.669;' +
	  '*.a52;*.aac;*.ac3;*.adt;*.adts;*.aif;*.aifc;*.aiff;*.amr;*.aob;*.ape;*.asf;*.au;*.awb;' +
	  '*.caf;*.cda;' +
	  '*.dts;' +
	  '*.eac3;' +
	  '*.flac;' +
	  '*.it;' +
	  '*.m2a;*.m3u;*.m4a;*.m4p;*.mid;*.midi;*.mka;*.mlp;*.mod;*.mp1;*.mp2;*.mp3;*.mpc;' +
	  '*.oga;*.ogg;*.oma;' +
	  '*.qcp;' +
	  '*.rmi;' +
	  '*.s3m;*.snd;*.spx;' +
	  '*.thd;*.tta;' +
	  '*.voc;*.vqf;' +
	  '*.w64;*.wav;*.wax;*.wma;*.wma;*.wv;' +
	  '*.xa;*.xm';

	VIDEO_FILE_FILTER =
	  '*.3g2;*.3gp;*.3gp2;*.3gpp;' +
	  '*.amv;*.asf;*.asx;*.avi;' +
	  '*.bin;' +
	  '*.dat;*.divx;*.drc;*.dv;*.dvr-ms;' +
	  '*.f4v;*.f4v;*.flv;*.flv;' +
	  '*.gxf;' +
	  '*.h264;*.hdmov;' +
	  '*.iso;*.ivf;' +
	  '*.m1v;*.m2t;*.m2ts;*.m2v;*.m4v;*.mjp;*.mjpg;*.mkv;*.mod;*.mov;*.movie;*.mp2;*.mp2v;*.mp4;*.mp4v;*.mpa;*.mpe;*.mpeg;*.mpeg1;*.mpeg2;*.mpeg4;*.mpg;*.mpv2;*.mpv4;*.mts;*.mtv;*.mxf;*.mxg;' +
	  '*.nsv;*.nuv;' +
	  '*.ogg;*.ogm;*.ogv;*.ogx;' +
	  '*.ps;' +
	  '*.ram;*.rec;*.rm;*.rmm;*.rmvb;*.rpm;' +
	  '*.swf;' +
	  '*.tod;*.ts;*.tts;' +
	  '*.vob;*.vro;' +
	  '*.webm;*.wm;*.wmd;*.wmp;*.wmv;*.wmv;*.wmx;*.wmz;*.wpl;*.wv;' +
	  '*.yuv';

	MOVIE_FILE_FILTER =
	  'Video Files|' + VIDEO_FILE_FILTER + '|' +
	  'AVI Video|*.avi|' +
	  'DVD file|*.vob|' +
	  'Flash Video File|*.flv;*.f4v|' +
	  'MPEG Video|*.mpg;*.mpe;*.mpeg;*.m1v;*.m2v;*.dat;*.vob;*.mod;*.h264|' +
	  'MPEG-4 Video|*.mp4;*.m4v|' +
	  'MPEG-TS Files|*.m2ts;*.m2t;*.mts;*.ts;*.tod|' +
	  'Windows Media Video|*.wmv;*.asf|' +
	  'Real Media Files|*.rm;*.rmvb|' +
	  'QuickTime Movie|*.mov;*.qt;*.mp4;*.m4v;*.dv;*.3gp;*.3g2|' +
	  '3GP Video|*.3gp;*.3g2;*.3gpp;*.3gp2|' +
	  'Digital Video|*.dv|' +
	  'All Files|*.*'
	  ;

	SUBTITLE_FILE_FILTER =
	  'Subtitle Files|' +
	  '*.utf;*.utf8;*.utf-8;*.sub;*.srt;*.smi;*.rt;*.txt;*.ssa;*.aqt;*.jss;*.js;*.ass;' +
	  '*.vtt;' +
	  '|' +
	  'All Files|*.*'
	  ;

Const
	FRAME_START_FMT   = 'From Frame:%d';
	FRAME_STOP_FMT    = 'End Frame:%d';
	TOTAL_FRAME_FRAME = 'Total Frame:%d';

Const
{$IFNDEF DEBUG}
	DLL_NAME = 'VideoConverter.dll';
{$ELSE}
{$IFDEF WIN64}
	DLL_NAME = 'E:\VideoConverter\src\x64\Release\VideoConverter.dll';
{$ELSE}
	DLL_NAME = 'E:\VideoConverter\src\Debug\VideoConverter.dll';
{$ENDIF}
{$ENDIF}

Implementation

End.
