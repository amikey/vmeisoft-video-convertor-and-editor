#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

//audio_enc->lpc_type = AV_LPC_TYPE_FIXED;
//audio_enc->lpc_coeff_precision = 15;
//audio_enc->prediction_order_method = 3;

void CEncoder::audio_flac(void)
{
	set_audio_codec_option("lpc_type", "1"); // FF_LPC_TYPE_FIXED
	set_audio_codec_option("lpc_coeff_precision", "15");
	set_audio_codec_option("prediction_order_method", "3");
}
