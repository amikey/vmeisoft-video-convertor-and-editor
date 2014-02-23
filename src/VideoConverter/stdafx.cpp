#include "stdafx.h"

#if defined(NDEBUG) && defined(_DEBUG)
#error ?
#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../Libs/Win64/Debug/ProfileManager.lib")
#else
#pragma comment(lib, "../Libs/Win64/Release/ProfileManager.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../Libs/Win32/Debug/ProfileManager.lib")
#else
#pragma comment(lib, "../Libs/Win32/Release/ProfileManager.lib")
#endif
#endif

#ifdef _WIN64
#pragma comment(lib, "../Libs/Win64/ffmpeg/libavcodec.lib")
#pragma comment(lib, "../Libs/Win64/ffmpeg/libavformat.lib")
#pragma comment(lib, "../Libs/Win64/ffmpeg/libavresample.lib")
#pragma comment(lib, "../Libs/Win64/ffmpeg/libswresample.lib")
#pragma comment(lib, "../Libs/Win64/ffmpeg/libswscale.lib")
#pragma comment(lib, "../Libs/Win64/ffmpeg/libavutil.lib")

#pragma comment(lib, "../Libs/Win64/ffmpeg/libopencore-amrnb.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libopencore-amrwb.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libgsm.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libspeex.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libcelt0.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libmp3lame.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libogg.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libvorbis.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libvorbisenc.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libx264.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libtheora.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libtheoraenc.lib" )

#pragma comment(lib, "../Libs/Win64/ffmpeg/libgcc.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/libmingw.lib" )

#else
#pragma comment(lib, "../Libs/Win32/ffmpeg/libavcodec.lib")
#pragma comment(lib, "../Libs/Win32/ffmpeg/libavformat.lib")
#pragma comment(lib, "../Libs/Win32/ffmpeg/libavresample.lib")
#pragma comment(lib, "../Libs/Win32/ffmpeg/libswresample.lib")
#pragma comment(lib, "../Libs/Win32/ffmpeg/libswscale.lib")
#pragma comment(lib, "../Libs/Win32/ffmpeg/libavutil.lib")

#pragma comment(lib, "../Libs/Win32/ffmpeg/libopencore-amrnb.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libopencore-amrwb.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libgsm.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libspeex.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libcelt0.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libmp3lame.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libogg.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libopencore-amrnb.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libopencore-amrwb.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libvorbis.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libvorbisenc.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libx264.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libtheora.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libtheoraenc.lib" )

#pragma comment(lib, "../Libs/Win32/ffmpeg/libgcc.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/libmingw.lib" )

#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../Libs/Win64/ffmpeg/Debug/libvpx.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Debug/faac.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Debug/libgif.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Debug/zlib.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Debug/bz2.lib" )
#else
#pragma comment(lib, "../Libs/Win64/ffmpeg/Release/libvpx.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Release/faac.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Release/libgif.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Release/zlib.lib" )
#pragma comment(lib, "../Libs/Win64/ffmpeg/Release/bz2.lib" )
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../Libs/Win32/ffmpeg/Debug/libvpx.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Debug/faac.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Debug/libgif.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Debug/zlib.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Debug/bz2.lib" )
#else
#pragma comment(lib, "../Libs/Win32/ffmpeg/Release/libvpx.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Release/faac.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Release/libgif.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Release/zlib.lib" )
#pragma comment(lib, "../Libs/Win32/ffmpeg/Release/bz2.lib" )
#endif
#endif


#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "D3d9.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "wmvcore.lib")
