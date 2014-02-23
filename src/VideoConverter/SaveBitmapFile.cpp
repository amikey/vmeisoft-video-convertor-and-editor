#include "stdafx.h"
#include "VideoConverter.h"
#include "SaveBitmapFile.h"

int32_t SaveTempBitmapFile(const wchar_t *file, void *bits, int32_t width, int32_t height)
{

	int32_t size = width * height * 4;

	// 写入文件头
	BITMAPFILEHEADER hdr = {};
	hdr.bfType = 0x4D42;
	hdr.bfSize = (uint32_t)(sizeof(hdr) + sizeof(BITMAPINFOHEADER) + size);
	hdr.bfOffBits = sizeof(hdr) + sizeof(BITMAPINFOHEADER);

	// 写BMP文件头
	BITMAPINFOHEADER info = {};
	info.biSize = sizeof(info);
	info.biWidth = width;
	info.biHeight = - height;
	info.biPlanes = 1;
	info.biBitCount = 32;
	info.biCompression = BI_RGB;
	info.biSizeImage = size;

	DeleteFile(file);
	HANDLE hFile = CreateFile(file, 
						GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	DWORD nBytesWrite;
	if(WriteFile(hFile, &hdr, sizeof(hdr), &nBytesWrite, NULL) == 0)
	{
		ErrorLog("Faild to write\n");
	}

	if(WriteFile(hFile, &info, sizeof(info), &nBytesWrite, NULL) == 0)
	{
		ErrorLog("Faild to write\n");
	}

	// 写数据
	if(WriteFile(hFile, bits, size, &nBytesWrite, NULL) == 0)
	{
		ErrorLog("Faild to write\n");
	}

	CloseHandle(hFile);

	return 0;
}

static HRESULT GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0) return E_FAIL_RESULT();  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)MemoryAlloc(size * num);
	if(pImageCodecInfo == NULL)	return E_FAIL_RESULT();  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( _wcsicmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			SAFE_FREE(pImageCodecInfo);
			return S_OK;  // Success
		}    
	}

	SAFE_FREE(pImageCodecInfo);
	return E_FAIL_RESULT();  // Failure
}

int32_t SaveToImageFile(const wchar_t *ImageFile, void *bits, int32_t width, int32_t height)
{
	static const CLSID GUID_NULL;
	static CLSID pngClsid;
	static CLSID jpgClsid;
	static CLSID bmpClsid;

	CLSID *clsid;
	switch(g_opt.m_GrabFileType)
	{
	case GrabFileType_PNG:
		if (pngClsid == GUID_NULL) GetEncoderClsid(L"image/png", &pngClsid);
		clsid = &pngClsid;
		break;

	case GrabFileType_JPG:
		if (jpgClsid == GUID_NULL) GetEncoderClsid(L"image/jpeg", &jpgClsid);
		clsid = &jpgClsid;
		break;

	default:
		if (bmpClsid == GUID_NULL) GetEncoderClsid(L"image/bmp", &bmpClsid);
		clsid = &bmpClsid;
		break;
	}
	
	Bitmap bitmap(width, height, width * 4, PixelFormat32bppRGB, (BYTE*)bits);
	return bitmap.Save(ImageFile, clsid);
}
