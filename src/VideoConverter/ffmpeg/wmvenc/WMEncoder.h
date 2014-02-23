#pragma once

interface IWMWriter;

struct WVPrivate
{
	IWMWriter*	m_pWriter;
	int32_t			m_nVideoInputIndex;
	int32_t			m_nAudioInputIndex;
	LONGLONG	m_llFrameCount;
	LONGLONG	m_llFrameRateA, m_llFrameRateB;
	LONGLONG	m_llAudioWriten;
	LONGLONG	m_llAudioDiv;
	int32_t			m_VideoBufferSize;
	int32_t			m_AudioBufferSize;

	AVFormatContext* m_afc;
};

extern int32_t wm_write_header(AVFormatContext *s);
extern int32_t wm_write_packet(AVFormatContext *s, AVPacket *pkt);
extern int32_t wm_interleave_packet(struct AVFormatContext *, AVPacket *out, AVPacket *in, int32_t flush);
extern int32_t wm_write_trailer(AVFormatContext *s);

extern int32_t wmv_encode_init(AVCodecContext *avctx);
extern int32_t wmv_encode_frame(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int32_t *got_packet_ptr);
extern int32_t wmv_encode_close(AVCodecContext *avctx);

extern int32_t wma_encode_init(AVCodecContext *avctx);
extern int32_t wma_encode_frame(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int32_t *got_packet_ptr);
extern int32_t wma_encode_close(AVCodecContext *avctx);
