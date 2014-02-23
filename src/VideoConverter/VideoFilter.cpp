#include "stdafx.h"
#include "VideoFilter.h"

/**************************************************
Base
*************************************************/
CVideoFilterBase::CVideoFilterBase(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	m_FilterType = VIDEO_FILTER_TYPE_NONE;
	m_Id = pTemplate->m_ClsID;
	m_data = pData;
	m_template = pTemplate;
	if (m_data) m_data->m_Instance = this;
}

CVideoFilterBase::~CVideoFilterBase(void)
{
	if (m_data) m_data->m_Instance = NULL;
}

BOOL CVideoFilterBase::IsValid()
{
	assert(m_template && m_data);
	return memcmp(m_template->m_pParameter, m_data->m_Parameter, m_template->m_cbParameter) == 0 ? FALSE : TRUE;
}

HRESULT CVideoFilterBase::SetParameters(void *Param, int32_t length)
{
	assert(m_template && m_data);
	if (length < m_template->m_cbParameter) return E_FAIL;
	CopyMemory(m_data->m_Parameter, Param, m_template->m_cbParameter);
	return S_OK;
}

HRESULT CVideoFilterBase::GetParameters(void *Param, int32_t length)
{
	assert(m_template && m_data);
	if (length < m_template->m_cbParameter) return E_FAIL;
	CopyMemory(Param, m_data->m_Parameter, m_template->m_cbParameter);
	return S_OK;
}

uint8_t* CVideoFilterBase::GetBuffer()
{
	return m_next->GetBuffer();
}

void CVideoFilterBase::PutSourceSize(int32_t width, int32_t height)
{
	m_next->PutSourceSize(width, height);
}

void CVideoFilterBase::NewSegment(int64_t tStart, int64_t tStop)
{
	m_tStart = tStart;
	m_tStop = tStop;
	m_next->NewSegment(tStart, tStop);
}

void CVideoFilterBase::EndOfStream()
{
	m_next->EndOfStream();
}

void CVideoFilterBase::FilterRelease()
{
	delete this;
}

/**************************************************
Manager
*************************************************/
CVideoFilterManager::CVideoFilterManager(void)
{
	m_ppFilters = NULL;
	m_cFilters = 0;
}

CVideoFilterManager::~CVideoFilterManager(void)
{
	for(int i = 0; i < m_cFilters; i++)
	{
		m_ppFilters[i]->FilterRelease();
	}
	SAFE_FREE(m_ppFilters);
}

void CVideoFilterManager::AddFiler(CVideoFilterBase* pFilter)
{
	m_ppFilters = (CVideoFilterBase **)MemoryRealloc(m_ppFilters, (m_cFilters + 1) * sizeof(CVideoFilterBase*));
	int32_t index = 0;
	for(int i = m_cFilters - 1; i >= 0; i--)
	{
		if (m_ppFilters[i]->m_FilterType <= pFilter->m_FilterType)
		{
			index = i + 1;
			break;
		}
	}
	for(int i = m_cFilters; i > index; i--)
	{
		m_ppFilters[i] = m_ppFilters[i - 1];
	}
	m_ppFilters[index] = pFilter;
	for(int i = 0; i < m_cFilters; i++)
	{
		m_ppFilters[i]->m_next = m_ppFilters[i + 1];
	}
	m_cFilters ++;
}

void CVideoFilterManager::PutSourceSize(int32_t width, int32_t height)
{
	m_cbBuffer = width * height * 4;
	m_ppFilters[0]->PutSourceSize(width, height);
}

void CVideoFilterManager::DoFilter(uint8_t *frame, int64_t Pts)
{
	uint8_t *source = m_ppFilters[0]->GetBuffer();
	if (source != frame) CopyMemory(source, frame, m_cbBuffer);
	m_ppFilters[0]->Process(source, Pts);
}

void CVideoFilterManager::NewSegment(int64_t tStart, int64_t tStop)
{
	m_ppFilters[0]->NewSegment(tStart, tStop);
}

void CVideoFilterManager::EndOfStream()
{
	m_ppFilters[0]->EndOfStream();
}