#pragma once
#include "afxwin.h"
class CCommandParse :
	public CCommandLineInfo
{
public:
	CCommandParse(void);
	virtual ~CCommandParse(void);
	//Exe Arguments Parse: Get functions for 
	//params and switches
	void GetParams(CStringArray& params);
private:
	//Exe Arguments Parse: Private Members
	CStringArray m_params;
	CStringArray m_switches;
	//Exe Arguments Parse: Override for Base class virtual
	void ParseParam(const TCHAR *pszParam,
		BOOL bFlag, BOOL bLast);
};
