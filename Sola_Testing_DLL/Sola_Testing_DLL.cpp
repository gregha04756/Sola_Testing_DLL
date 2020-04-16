// Sola_Testing_DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Sola_Testing_DLL.h"


// This is the constructor of a class that has been exported.
// see Sola_Testing_DLL.h for the class definition

CSola_Testing_DLL::CSola_Testing_DLL()
{
	this->m_szDlgQuitEvent = NULL;
	this->m_szDlgRdyEvent = NULL;
	this->m_hDlgQuitEvent = NULL;
	this->m_hwndOwner = NULL;
	this->m_hwndDlg = NULL;
	this->m_hDlgThread = NULL;
	this->m_hdupDlgThread = NULL;
	this->m_hInst = (HINSTANCE) NULL;
	this->m_szTitle = NULL;
	this->m_dwDlgThreadID = NULL;
	this->m_hDlgRdyEvent = NULL;
	this->m_Parent_Callback_Fn = NULL;
	m_h_Dlg_Glbl = NULL;
	m_ui16_fs = 0;
	m_ui16_os = 0;
	m_ui16_fr = 0;
	m_ui16_fns = 0;
}

CSola_Testing_DLL::CSola_Testing_DLL(HWND hOwner,callback_fn_ptr fp,HINSTANCE hInst,TCHAR* szTitle) : m_hwndOwner(hOwner),
	m_Parent_Callback_Fn(fp),m_hInst(hInst),m_szTitle(szTitle)
{
	BOOL bSuccess = true;
	this->m_szDlgQuitEvent = _T("DlgQuitEvent");
	this->m_szDlgRdyEvent = _T("DlgRdyEvent");
	this->m_hDlgQuitEvent = NULL;
	this->m_hDlgRdyEvent = NULL;
	this->m_hwndDlg = NULL;
	this->m_hDlgThread = NULL;
	this->m_hdupDlgThread = NULL;
	this->m_dwDlgThreadID = 0L;
	m_h_Dlg_Glbl = NULL;
	m_ui16_fs = 0;
	m_ui16_os = 0;
	m_ui16_fr = 0;
	m_ui16_fns = 0;
}

CSola_Testing_DLL::~CSola_Testing_DLL()
{
	BOOL bResult;
	if (NULL != this->m_hDlgThread)
	{
		Stop();
	}
	if (NULL != this->m_hDlgQuitEvent)
	{
		bResult = ::CloseHandle(m_hDlgQuitEvent);
		m_hDlgQuitEvent = NULL;
	}
#if 0
/*	if (NULL != m_h_Dlg_Glbl)
	{
		GlobalFree(m_h_Dlg_Glbl);
	}*/
#endif
}

void CSola_Testing_DLL::Set_Parent_Callback(callback_fn_ptr fp)
{
	this->m_Parent_Callback_Fn = fp;
}

void CSola_Testing_DLL::Set_Values(uint16_t ui16_fs,uint16_t ui16_os,uint16_t ui16_fr,uint16_t ui16_fns)
{
	BOOL b_r;
	m_ui16_fs = ui16_fs;
	m_ui16_os = ui16_os;
	m_ui16_fr = ui16_fr;
	m_ui16_fns = ui16_fns;
	if ((NULL != m_hDlgThread) && IsWindow(m_hwndDlg))
	{
		b_r = PostMessage(m_hwndDlg,WM_APPUPDVALUES,(WPARAM)0,(LPARAM)0);
	}
}

BOOL CSola_Testing_DLL::Run()
{
	BOOL bResult;
	DWORD dwResult;
	int nResult;
	if (NULL == m_hDlgThread)
	{
		m_hDlgQuitEvent = ::CreateEvent(NULL,true,false,m_szDlgQuitEvent);
		bResult = ::ResetEvent(m_hDlgQuitEvent);
		m_hDlgThread = ::CreateThread(NULL,0,DlgThreadProc,(LPVOID)this,CREATE_SUSPENDED,&m_dwDlgThreadID);
		dwResult = ::GetLastError();
		if ( m_hDlgThread == NULL )
		{
			nResult = ::MessageBox(this->m_hwndOwner,_T("Error starting Dlg thread"),m_szTitle,MB_OK);
			ReportError(dwResult);
		}
		else
		{
			bResult = ::DuplicateHandle(::GetCurrentProcess(),m_hDlgThread,::GetCurrentProcess(),&m_hdupDlgThread,DUPLICATE_SAME_ACCESS,true,0);
			dwResult = ::ResumeThread(m_hDlgThread);
		}
	}
	return (ERROR_SUCCESS == GetLastError());
}

BOOL CSola_Testing_DLL::Stop()
{
	BOOL bResult;
	DWORD dw_wr;
	DWORD dw_rc;
	int nResult;

	if (NULL != this->m_hDlgThread)
	{
		bResult = ::SetEvent(m_hDlgQuitEvent);
		if (IsWindow(m_hwndDlg))
		{
			bResult = ::PostMessage(m_hwndDlg,WM_APPENDDLGTHREAD,(WPARAM)0,(LPARAM)0);
			dw_wr = WaitForSingleObject(m_hDlgThread,5000);
			dw_rc = GetLastError();
			if (WAIT_TIMEOUT == dw_wr)
			{
				nResult = MessageBox(this->m_hwndOwner,_T("Timed out waiting dlg thread"),this->m_szTitle,MB_OK);
			}
			if (WAIT_FAILED == dw_wr)
			{
				ReportError(dw_rc);
			}
		}
		bResult = ::CloseHandle(m_hDlgThread);
		bResult = ::CloseHandle(m_hdupDlgThread);
		m_hDlgThread = NULL;
		m_hdupDlgThread = NULL;
	}
	if (NULL != m_hDlgQuitEvent)
	{
		bResult = ::CloseHandle(m_hDlgQuitEvent);
		m_hDlgQuitEvent = NULL;
	}
	return true;
}

DWORD WINAPI CSola_Testing_DLL::DlgThreadProc(LPVOID lParam)
{
	MSG message;
	BOOL bResult;
	HGLOBAL hgbl_hd;

	CSola_Testing_DLL* pnd = (CSola_Testing_DLL*)lParam;
	pnd->m_hwndDlg = NULL;
	
	if (!IsWindow(pnd->m_hwndDlg) && IsWindow(pnd->m_hwndOwner))
	{
#if 0
		if (NULL != (pnd->m_h_Dlg_Glbl = pnd->Make_Dlg_Template("Click Scan to start")))
		{
			pnd->m_hwndDlg = CreateDialogIndirectParam(pnd->m_hInst,(LPDLGTEMPLATE)pnd->m_h_Dlg_Glbl,pnd->m_hwndOwner,DlgProc,(LPARAM)pnd);
		}
#endif
		pnd->m_hwndDlg = CreateDialogIndirectParam(pnd->m_hInst,(LPDLGTEMPLATE)dlg_103,pnd->m_hwndOwner,DlgProc,(LPARAM)pnd);
		if (NULL == pnd->m_hwndDlg)
		{
			ReportError(GetLastError());
			return 1;
		}
		bResult = ShowWindow(pnd->m_hwndDlg,SW_SHOW); 

		while ((bResult = GetMessage(&message, NULL, 0, 0)) != 0) 
		{ 
			if (bResult == -1)
			{
	     // Handle the error and possibly exit
				break;
			}
			else if (!IsWindow(pnd->m_hwndDlg) || !IsDialogMessage(pnd->m_hwndDlg,&message)) 
			{ 
				TranslateMessage(&message); 
				DispatchMessage(&message); 
			} 
		}
	}
#if 0
	if (NULL != pnd->m_h_Dlg_Glbl)
	{
		hgbl_hd = GlobalFree(pnd->m_h_Dlg_Glbl);
	}
#endif
	if (NULL != pnd->m_Parent_Callback_Fn)
	{
		pnd->m_Parent_Callback_Fn(pnd->m_hwndOwner);
	}
	return 0;
}

INT_PTR CALLBACK CSola_Testing_DLL::DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	BOOL b_r;
	static CSola_Testing_DLL* pnd;
	BOOL bResult;
	int i;
	int i_r;
	int ndx;
	UINT uiResult;
	LRESULT lRes;
	HRESULT hRes;
	LARGE_INTEGER lipt;
	DWORD hh;
	int mm;
	int ss;
	double dErrRate;
	static HWND hwnd_mblo;
	static HWND hwnd_mbhi;
	static INITCOMMONCONTROLSEX iccex_icc;
	static HFONT h_font;
	std::wstring *p_scan_upd;
	std::wstring ws_value;
	TCHAR sz_value[32];
	uint8_t ui8_mblo;
	uint8_t ui8_mbhi;
	wstring wstr_mblo;
	wstring wstr_mbhi;
	TCHAR sz_mb_addr[4];
	PVOID p_v;
	std::list<wstring>::iterator ipl_it;

	switch (message)
	{
	case WM_INITDIALOG:
		ui8_mblo = 1;
		ui8_mbhi = 8;
		iccex_icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex_icc.dwICC = ICC_STANDARD_CLASSES|ICC_INTERNET_CLASSES;
		InitCommonControlsEx(&iccex_icc);
		pnd = (CSola_Testing_DLL*)lParam;
		h_font = CreateFont(
			-30, /* Height */
			0, /* Width */
			0, /* Escapement */
			0, /* Orientation */
			FW_SEMIBOLD, /* Weight */
			false, /* Italic */
			false, /* Underline */
			false, /* Strikeout */
			DEFAULT_CHARSET,  /* Character set */
			OUT_TT_PRECIS, /* Output precision */
			CLIP_DEFAULT_PRECIS,/* Clip precision */
			DEFAULT_QUALITY, /* Quality */
			FF_DONTCARE, /* Pitch and family */
			L"Calibri"); /* Type face */
		if (NULL != h_font)
		{
			lRes = SendDlgItemMessage(hDlg,IDCANCEL,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_STATICFS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_TEXTFS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_STATICOS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_TEXTOS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_STATICFR,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_TEXTFR,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_STATICFNS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
			lRes = SendDlgItemMessage(hDlg,ID_TEXTFNS,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));
/*			SendDlgItemMessage(hDlg,IDCANCEL,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));*/
/*			SendDlgItemMessage(hDlg,ID_TEXT,WM_SETFONT,(WPARAM)h_font,MAKELPARAM(true,true));*/
		}
#if 0
		hwnd_mblo = CreateWindow(
			WC_EDIT, 
			L"", 
			WS_CHILD|WS_OVERLAPPED|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_RIGHT,
			10,
			85,
			70, 
			20,  
			hDlg, 
			NULL, 
			pnd->m_hInst, 
			NULL);
		hwnd_mbhi = CreateWindow(
			WC_EDIT, 
			L"", 
			WS_CHILD|WS_OVERLAPPED|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_RIGHT,
			10,
			115,
			70, 
			20,  
			hDlg, 
			NULL, 
			pnd->m_hInst, 
			NULL);
		wstr_mblo.assign(to_wstring((_ULonglong)ui8_mblo));
		wstr_mbhi.assign(to_wstring((_ULonglong)ui8_mbhi));
		b_r = SetWindowText(hwnd_mblo,wstr_mblo.c_str());
		b_r = SetWindowText(hwnd_mbhi,wstr_mbhi.c_str());
#endif
		if (NULL != pnd->m_szTitle)
		{
			b_r = SetWindowText(hDlg,pnd->m_szTitle);
		}
		return (INT_PTR)TRUE;
	case WM_APPUPDVALUES:
		hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("%-6.2f"),pnd->m_ui16_fs/100.0);
		b_r = SetDlgItemText(hDlg,ID_TEXTFS,sz_value);
		hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("%-6.2f°F %-6.2f°C"),((9.0*pnd->m_ui16_os)/50.0)+32.0,pnd->m_ui16_os/10.0);
		b_r = SetDlgItemText(hDlg,ID_TEXTOS,sz_value);
		hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("%d"),pnd->m_ui16_fr);
		b_r = SetDlgItemText(hDlg,ID_TEXTFR,sz_value);
		hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("%d"),pnd->m_ui16_fns);
		if (0x8300 == pnd->m_ui16_fns)
		{
			hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("HIGH"));
		}
		if (0x8400 == pnd->m_ui16_fns)
		{
			hRes = ::StringCchPrintf(sz_value,sizeof(sz_value)/sizeof(TCHAR),_T("LOW"));
		}
		b_r = SetDlgItemText(hDlg,ID_TEXTFNS,sz_value);
		return (INT_PTR)true;
	case WM_APPENDDLGTHREAD:
		bResult = DestroyWindow(hDlg);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
#if 0
	if (NULL != pnd->m_Parent_Callback_Fn)
	{
		pnd->m_Parent_Callback_Fn(pnd->m_hwndOwner);
	}
#endif
			if (IsWindow(hDlg))
			{
				bResult = PostMessage(hDlg,WM_APPENDDLGTHREAD,(WPARAM)0,(LPARAM)0);
			}
			else
			{
				bResult = DestroyWindow(hDlg);
			}
			return (INT_PTR)TRUE;
		}
#if 0
		if (LOWORD(wParam) == IDOK)
		{
			return (INT_PTR)TRUE;
		}
#endif
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

#if 0
HGLOBAL CSola_Testing_DLL::Make_Dlg_Template(LPSTR lpszMessage)
{
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    LRESULT ret;
    int nchar;
	LPSTR p_msg;

	ULONG lp_wt;
	size_t st;


    hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
    if (!hgbl)
        return NULL;
 
    lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);
 
    // Define a dialog box.
 
    lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
    lpdt->cdit = 2;         // Number of controls
    lpdt->x  = 10;
	lpdt->y  = 10;
    lpdt->cx = 320;
	lpdt->cy = 180;

    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0;             // No menu
    *lpw++ = 0;             // Predefined dialog box class (by default)

    lpwsz = (LPWSTR)lpw;
/*    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"My Dialog",-1,lpwsz,50);*/
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Op Test",-1,lpwsz,50);
    lpw += nchar;

    //-----------------------
    // Define a CANCEL button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 150;
    lpdit->cx = 80;
	lpdit->cy = 20;
    lpdit->id = IDCANCEL;       // Cancel button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Cancel",-1,lpwsz,50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data

#if 0
	//-----------------------
    // Define Init button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 150;
	lpdit->y  = 10;
    lpdit->cx = 40;
	lpdit->cy = 20;
    lpdit->id = ID_INIT;    // Init button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class atom

    lpwsz = (LPWSTR)lpw;
/*    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Help",-1,lpwsz,50);*/
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Scan",-1,lpwsz,50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data
#endif

    //-----------------------
    // Define static text control for Flame Signal label
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 10;
    lpdit->cx = 120;
	lpdit->cy = 30;
    lpdit->id = ID_STATICFS;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

	p_msg = szfs;
    for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*p_msg++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data

#if 0
    //-----------------------
    // Define static text control for Outlet Sensor label
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 50;
    lpdit->cx = 120;
	lpdit->cy = 30;
    lpdit->id = ID_STATICOS;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

    p_msg = szos;
	for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*p_msg++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define static text control for Firing rate label
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 90;
    lpdit->cx = 120;
	lpdit->cy = 30;
    lpdit->id = ID_STATICOS;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

    p_msg = szfr;
	for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*p_msg++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define static text control for Fan speed label
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 130;
    lpdit->cx = 120;
	lpdit->cy = 30;
    lpdit->id = ID_STATICFNS;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

    p_msg = szfns;
	for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*p_msg++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data
#endif

    GlobalUnlock(hgbl); 
/*    ret = DialogBoxIndirect(hinst, 
                           (LPDLGTEMPLATE)hgbl, 
                           hwndOwner, 
                           (DLGPROC)DialogProc); 
    GlobalFree(hgbl); */
    return hgbl; 
}

LPWORD CSola_Testing_DLL::lpwAlign(LPWORD lpIn)
{
    ULONG ul;

    ul = (ULONG)lpIn;
    ul ++;
    ul >>=1;
    ul <<=1;
    return (LPWORD)ul;
}
#endif