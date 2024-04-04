#include "stdafx.h"
#include "CommandParse.h"


CCommandParse::CCommandParse()
{
}


CCommandParse::~CCommandParse()
{
}


//Exe Arguments Parse: Implement the Parse Param
void CCommandParse::ParseParam(
	const TCHAR *pszParam,
	BOOL bFlag, BOOL bLast)
{
	//Exe Arguments Parse: Collect the parameters 
	//	and switches in a separate Array
	CString param_or_switch(pszParam);
	if (!bFlag)
		m_params.Add(param_or_switch);
}

//Exe Arguments Parse: Get Functions. 
void CCommandParse::GetParams(CStringArray& params)
{
	int size = m_params.GetCount();
	for (int i = 0; i < size; i++)
		params.Add(m_params.GetAt(i));
}
