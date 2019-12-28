#include "stdafx.h"

#include "GraphViewer.h"
#include "GraphViewerDoc.h"
#include "GraphViewerView.h"
#include "SaveDlg.h"

#include <fstream>

using namespace std;

//------------------------------------------------------------------------------------------//
IMPLEMENT_DYNCREATE(CGraphViewerView, CFormView)
	//------------------------------------------------------------------------------------------//
	BEGIN_MESSAGE_MAP(CGraphViewerView, CFormView)
		//{{AFX_MSG_MAP(CGraphViewerView)
		ON_WM_SIZE()
		ON_WM_LBUTTONDOWN()
		ON_COMMAND(ID_ZOOMIN, OnZoomIn)
		ON_COMMAND(ID_ZOOMOUT, OnZoomOut)
		ON_BN_CLICKED(RUNBUTTON, OnRun)
		ON_COMMAND(ID_SAVETXT, OnSavetxt)
		ON_COMMAND(ID_ZOOMIN_ALONG_OX, OnZoomInAlongOx)
		ON_COMMAND(ID_ZOOMOUT_ALONG_OX, OnZoomOutAlongOx)
		ON_COMMAND(ID_ZOOMIN_ALONG_OY, OnZoominAlongOy)
		ON_COMMAND(ID_ZOOMOUT_ALONG_OY, OnZoomoutAlongOy)
		ON_CBN_CLOSEUP(FUNCCOMBO, OnCloseupFuncCombo)
		ON_COMMAND(ID_SHOWGRID, OnShowgrid)
		ON_UPDATE_COMMAND_UI(ID_SAVETXT, OnUpdateSavetxt)
		ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomin)
		ON_UPDATE_COMMAND_UI(ID_ZOOMIN_ALONG_OX, OnUpdateZoominAlongOx)
		ON_UPDATE_COMMAND_UI(ID_ZOOMIN_ALONG_OY, OnUpdateZoominAlongOy)
		ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomout)
		ON_UPDATE_COMMAND_UI(ID_ZOOMOUT_ALONG_OX, OnUpdateZoomoutAlongOx)
		ON_UPDATE_COMMAND_UI(ID_ZOOMOUT_ALONG_OY, OnUpdateZoomoutAlongOy)
		ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
		ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
		ON_UPDATE_COMMAND_UI(GOTOBUT, OnUpdateGotoButton)
		ON_COMMAND(ID_BUILD, OnBuild)
		ON_BN_CLICKED(GOTOBUT, OnGoto)
		ON_WM_RBUTTONDOWN()
		ON_UPDATE_COMMAND_UI(ID_SHOWGRID, OnUpdateShowgrid)
		ON_COMMAND(ID_FIT_TO_WINDOW, OnFitToWindow)
		ON_UPDATE_COMMAND_UI(ID_FIT_TO_WINDOW, OnUpdateFitToWindow)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	//------------------------------------------------------------------------------------------//
	CGraphViewerView::CGraphViewerView() : CFormView(CGraphViewerView::IDD)
	{
		//{{AFX_DATA_INIT(CGraphViewerView)

		//}}AFX_DATA_INIT
		m_curFunc = NULL;
		m_graph = NULL;
	}
	//------------------------------------------------------------------------------------------//
	CGraphViewerView::~CGraphViewerView()
	{
		if(m_graph)
			delete m_graph;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnInitialUpdate()
	{
		CFormView::OnInitialUpdate();
		CGraphViewerDoc *pDoc = GetDocument();
		UpdateSize();

		if(pDoc->m_parsed)
		{
			m_curFunc = &pDoc->m_fAr.back();
			COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			CMainFrame *pMain = static_cast<CMainFrame*>(AfxGetMainWnd());
			CComboBoxEx *pCombo = (CComboBoxEx*)pMain->GetControl(0);
			pMain->ClearFields();
			int j = 0;
			for(Function *i = &pDoc->m_fAr.back(); i >= &pDoc->m_fAr.front(); i--)
			{
				item.iItem = j++;
				item.pszText = const_cast<TCHAR*>((LPCTSTR)i->params.fName);
				item.cchTextMax = i->params.fName.GetLength() + 1;
				pCombo->InsertItem(&item); 
			}
			pCombo->SetCurSel(0);
			CEdit *pEdit = (CEdit*)pMain->GetControl(1);
			CString s;
			s.Format("%lf", m_curFunc->params.from);
			pEdit->SetWindowText(s);
			pEdit = (CEdit*)pMain->GetControl(2);
			s.Format("%lf", m_curFunc->params.to);
			pEdit->SetWindowText(s);
			pEdit = (CEdit*)pMain->GetControl(3);
			s.Format("%lf", m_curFunc->params.delta);
			pEdit->SetWindowText(s);
			if (m_graph) {
				delete m_graph;
				m_graph = new CGraph(m_GraphRect, m_curFunc, RGB(0, 0, 255));
			}
			ResetZoom();
			UpdateGraph(2);
		}
		m_interval = pDoc->m_interval;
	}
	//------------------------------------------------------------------------------------------//
#ifdef _DEBUG
	void CGraphViewerView::AssertValid() const
	{
		CFormView::AssertValid();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::Dump(CDumpContext& dc) const
	{
		CFormView::Dump(dc);
	}
	//------------------------------------------------------------------------------------------//
	CGraphViewerDoc* CGraphViewerView::GetDocument() 
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphViewerDoc)));
		return (CGraphViewerDoc*)m_pDocument;
	}
#endif 
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnDraw(CDC *pDC) 
	{	
		CGraphViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		UpdateScrollPos();

		FParser Parser;
		if (m_graph) {
			m_graph->DrawGraph(pDC->m_hDC, m_showGrid, m_curFunc, &Parser, m_clRect, m_scrlpos, m_interval, m_hWnd);
			ShowPoint(pDC);
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::ShowPoint(CDC *pDC)
	{
		if(m_showPt)
		{
			COLORREF col = RGB(0, 0, 0);
			FParser Parser;
			CBrush br;
			CPen *pPen = new CPen(PS_SOLID, 1, col);

			br.CreateSolidBrush(col);
			pDC->SelectObject(pPen);
			pDC->SelectObject(&br);

			int x = m_graph->ArgToScreenX(m_clArg);
			int y = m_graph->FValToScreenY(Parser.Execute(m_clArg, &(m_curFunc->ops)), m_curFunc);

			pDC->Ellipse(x - 3, y - 3, x + 3, y + 3);
			DeleteObject(&br);
			DeleteObject(pPen);
			delete pPen;
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnSize(UINT nType, int cx, int cy) 
	{
		int parsed = GetDocument()->m_parsed;
		CFormView::OnSize(nType, cx, cy);
		int holdCX = m_clRect.right;
		int holdCY = m_clRect.bottom;

		if(cx > ZOOMSTEP)
			m_clRect.right = cx;
		if(cy > ZOOMSTEP)
			m_clRect.bottom = cy;

		if(parsed)
		{
			m_GraphRect.right = MulDiv(m_GraphRect.right, m_clRect.right, holdCX);
			m_GraphRect.bottom = MulDiv(m_GraphRect.bottom, m_clRect.bottom, holdCY);
			UpdateGraph(2);
			UpdateDraw();
		}
	}
	
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::GetAbsMaxValue()
	{
		CGraphViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		double arg = m_curFunc->params.from;
		int n = int(N);
		FParser Parser;
		double max, min, val;
		int from = 1;
		while(from <= n)
		{
			min = (max = Parser.Execute(arg, &(m_curFunc->ops)));
			if(!_isnan(min)) //is not a NaN
				break;
			arg += m_curFunc->params.delta;
			from++;
		}
		if(from == n)
		{
			max = 1;
			min = -1;
		}
		for(int i = from; i <= n; i++)
		{
			val = Parser.Execute(arg, &(m_curFunc->ops));
			arg += m_curFunc->params.delta;
			if(_isnan(val)) //is a NaN
				continue;
			if(max < val)
				max = val;
			if(min > val)
				min = val;
		}
		pDoc->m_parsed = 1;
		fabs(max) > fabs(min) ? m_curFunc->absMax = fabs(max) : m_curFunc->absMax = fabs(min);
		m_curFunc->max = max;
		m_curFunc->min = min;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		if(GetDocument()->m_parsed)
		{
			double arg = m_graph->ScreenXToArg(m_scrlpos.x + point.x);
			if(arg >= m_curFunc->params.from && arg <= m_curFunc->params.to)
			{
				RECT rc;
				FParser Parser;
				int x, y;
				if(m_showPt)
				{
					x = m_graph->ArgToScreenX(m_clArg) - m_scrlpos.x; 
					y = m_graph->FValToScreenY(Parser.Execute(m_clArg, &(m_curFunc->ops)), m_curFunc) - m_scrlpos.y;
					rc.left = x - 4; rc.right = x + 4; rc.top = y - 4; rc.bottom = y + 4; 
					//hide old point
					m_showPt = false; 
					UpdateDraw(&rc); 
				}
				int i;
				m_showPt = true;

				//show new point
				m_clArg = arg;
				double funcRes = Parser.Execute(m_clArg, &(m_curFunc->ops));
				x = m_graph->ArgToScreenX(m_clArg) - m_scrlpos.x;
				y = m_graph->FValToScreenY(funcRes, m_curFunc) - m_scrlpos.y;
				rc.left = x - 4; rc.right = x + 4; rc.top = y - 4; rc.bottom = y + 4;
				UpdateDraw(&rc);

				//show x and f(x) in status bar
				CString argStr, funcStr;
				argStr.Format("%.22lf", m_clArg);
				funcStr.Format("%.22lf", funcRes);
				USHORT lenArgStr = argStr.GetLength() - 1;
				USHORT lenFStr = funcStr.GetLength() - 1;

				bool flag = true;
				for(i = lenArgStr; ; i--)
				{
					if(argStr.GetAt(i) == '.')
					{
						if(flag)
							argStr.Delete(i); //delete the point, because all digits after it are 0.
						break;
					}
					if(argStr.GetAt(i) == '0')
						argStr.Delete(i);
					else
						flag = false;
				}

				flag = true;
				for(i = lenFStr; ; i--)
				{
					if(funcStr.GetAt(i) == '.')
					{
						if(flag)
							funcStr.Delete(i); //delete the point, because all digits after it are 0.
						break;
					}
					if(funcStr.GetAt(i) == '0')
						funcStr.Delete(i);
					else
						flag = false;
				}
				((CMainFrame*)AfxGetMainWnd())->SetStatusText(argStr, funcStr);

				//scroll to point position if it is out of viewport
				if(y > m_clRect.bottom || y < m_clRect.top)
				{
					DWORD wparam = SB_THUMBTRACK;
					wparam |= (1 << 16)*(y + m_scrlpos.y);
					SendMessage(WM_VSCROLL, wparam, NULL);
				}
			}
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnRun()
	{
		CGraphViewerDoc* pDoc = GetDocument();
		int i;
		CString str[3];
		CMainFrame *pMain = static_cast<CMainFrame*>(AfxGetMainWnd());
		CComboBoxEx *pCombo = static_cast<CComboBoxEx*>(pMain->GetControl(0));

		for(i = 1; i <= 3; i++)
		{
			CEdit *pEdit = static_cast<CEdit*>(pMain->GetControl(i));
			pEdit->GetWindowText(str[i - 1]);
			str[i - 1].Remove(' ');
		}
		funcParams Params;
		pCombo->GetWindowText(Params.fName);

		for(i = 0; i < Params.fName.GetLength()-1; ++i)
			if(Params.fName.GetAt(i) == '(' && Params.fName.GetAt(i+1) == ')') {
				::MessageBox(0, "Argument is not specified", "Cuation", MB_OK|MB_ICONINFORMATION); return;
		}
		CString errStr = GetError(Params, str);
		Params.fName.Remove(' ');
		Params.fName.MakeLower();
		bool doNotAdd = false;
		for (i = 0; i < pCombo->GetCount(); i++)
		{
			CString s;
			int n = pCombo->GetLBTextLen(i);
			pCombo->GetLBText(i, const_cast<char*>((LPCTSTR)s));
			if(Params.fName == s)
			{
				m_curFunc->params = Params;
				doNotAdd = true;
				pDoc->SetModifiedFlag();
				break;
			}
		}
		if(!doNotAdd)
		{
			FParser Parser;
			Parser.SetFunction(Params.fName);
			pDoc->m_fAr.push_back(Function());
			pDoc->m_fCount++;
			m_curFunc = &pDoc->m_fAr.back();
			Parser.GetOpers(m_curFunc->ops);
			m_curFunc->opCount = m_curFunc->ops.size();
			m_curFunc->params = Params;
			GetDocument()->SetTitle(Params.fName);

			COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			item.iItem = 0;
			item.pszText = const_cast<char*>((LPCSTR)(Params.fName));
			item.cchTextMax = Params.fName.GetLength() + 1;
			pCombo->InsertItem(&item);
			pDoc->SetModifiedFlag();
		}
		GetAbsMaxValue();
		ResetZoom();
		delete m_graph;
		m_graph = new CGraph(m_GraphRect, m_curFunc, GetDocument()->m_gridCol); 
		OnFitToWindow();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnSavetxt() 
	{
		CGraphViewerDoc* pDoc = GetDocument();
		Function *iter;
		FuncArray buf;
		CSaveDlg sv_dlg(NULL, pDoc->m_fAr, buf);
		if(sv_dlg.DoModal()!=IDOK || buf.empty()) return;
		CFileDialog dlg(false, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Text Files (*.txt)|*.txt|");
		if (dlg.DoModal() == IDOK)
		{
			std::ofstream ofs((LPCSTR)dlg.GetPathName());
			if (ofs.is_open()) {
				FParser Parser;
				int count = 0;
				for (iter = &buf.front(); iter <= &buf.back(); iter++)
					count += int((iter->params.to - iter->params.from) / iter->params.delta + 1);
				const int n = int(N);
				for (iter = &buf.front(); iter <= &buf.back(); iter++)
				{
					CString buf;
					buf.Format("%lf ", iter->params.from);
					CString fTitle = "[BEGIN]\nf(x) = " + iter->params.fName + "\nXmin = " + buf;
					buf.Format("%lf ", iter->params.to);
					fTitle += "Xmax = " + buf + "Xdelta = ";
					buf.Format("%lf\n", iter->params.delta);
					fTitle += buf;
					ofs << (LPCSTR)fTitle;
					FParser::Initialize(iter->ops);
					for (double arg = iter->params.from; arg <= iter->params.to; arg += iter->params.delta)
					{
						buf.Format("%lf", arg);
						fTitle = "f(" + buf + ") = ";
						buf.Format("%lf\n", Parser.Execute(arg, &(iter->ops)));
						fTitle += buf;
						ofs << (LPCSTR)fTitle;
					}
					fTitle = "[END]\n";
					ofs << (LPCSTR)fTitle;
				}
			}
			ofs.close();
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoomIn() 
	{
		int holdWidth = m_GraphRect.right;
		DoZoomInAlongOx();
		m_GraphRect.bottom = MulDiv(m_GraphRect.right, m_GraphRect.bottom, holdWidth);
		UpdateGraph(2);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoomOut() 
	{
		int holdWidth = m_GraphRect.right;
		DoZoomOutAlongOx();
		m_GraphRect.bottom = MulDiv(m_GraphRect.right, m_GraphRect.bottom, holdWidth);
		UpdateGraph(2);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoomInAlongOx() 
	{
		DoZoomInAlongOx();
		UpdateGraph(0);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoomOutAlongOx() 
	{
		DoZoomOutAlongOx();
		UpdateGraph(0);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoominAlongOy() 
	{
		DoZoomInAlongOy();
		UpdateGraph(1);
		UpdateDraw();	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnZoomoutAlongOy() 
	{
		DoZoomOutAlongOy();
		UpdateGraph(1);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnFitToWindow() 
	{
		m_GraphRect = m_clRect;
		UpdateGraph(2);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnCloseupFuncCombo()
	{
		CGraphViewerDoc *pDoc = GetDocument();
		CMainFrame *pMain = static_cast<CMainFrame*>(AfxGetMainWnd());
		CComboBoxEx *pCombo = (CComboBoxEx*)pMain->GetControl(0);
		CString s;
		pCombo->GetWindowText(s);
		pCombo->SetCurSel(pCombo->GetCurSel());
		CString s1;
		pCombo->GetWindowText(s1);

		if(s!=s1)
		{
			if(s1 && (pDoc->m_fAr.size() > 1))
				for(FuncArray::iterator i = pDoc->m_fAr.begin(); i < pDoc->m_fAr.end(); i++)
					if(i->params.fName == s1)
					{ 
						m_curFunc = &*i;
						FParser::Initialize(m_curFunc->ops);
						pDoc->SetTitle(s1);
						double params[] = {m_curFunc->params.from, m_curFunc->params.to, m_curFunc->params.delta};
						for(int j = 1; j <= 3; j++)
						{
							CEdit *pEdit = (CEdit*)pMain->GetControl(j);
							CString s2;
							s2.Format("%lf", params[j - 1]);
							pEdit->SetWindowText(s2);
						}
						ResetZoom();
						UpdateGraph(2);
						pMain->SetStatusText("", "");
						m_showPt = false;
						UpdateDraw();
						break;
					}
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::UpdateGraph(USHORT how) //how == 0 --> by width,
		//how == 1 --> by height
		//how == 2 --> both
	{
		if(m_graph)
		{
			m_graph->UpdateRect(m_GraphRect, m_curFunc);
			CSize sz = GetTotalSize();
			if(how == 0 || how == 2)
			{
				sz.cx = m_GraphRect.right;
				if((m_disableGoto = (sz.cx <= m_clRect.right)))
					sz.cx = 0;
			}
			if(how == 1 || how == 2)
			{
				sz.cy = m_GraphRect.bottom;
				if(sz.cy <= m_clRect.bottom)
					sz.cy = 0;
			}
			SetScrollSizes(MM_TEXT, sz, CSize(m_clRect.right, m_clRect.bottom), 
				CSize(m_interval, m_interval));
		}
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnShowgrid() 
	{
		CMenu* menu = AfxGetMainWnd()->GetMenu();
		UINT state = menu->GetMenuState(ID_SHOWGRID, MF_BYCOMMAND);
		m_showGrid = !m_showGrid;
		menu->CheckMenuItem(ID_SHOWGRID, (state & MF_CHECKED) != 0 ? MF_UNCHECKED : MF_CHECKED);
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateSavetxt(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoomin(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoominAlongOx(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoominAlongOy(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoomout(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoomoutAlongOx(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateZoomoutAlongOy(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateFileSave(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
	{
		pCmdUI->Enable(GetDocument()->m_parsed);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateShowgrid(CCmdUI* pCmdUI) 
	{
		pCmdUI->SetRadio(m_showGrid);	
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateGotoButton(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(!m_disableGoto);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnUpdateFitToWindow(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(GetDocument()->m_parsed);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnBuild() 
	{
		OnRun();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnRButtonDown(UINT nFlags, CPoint point) 
	{
		CPoint ScreenPoint(point);
		CMenu *pMenu = ((AfxGetMainWnd())->GetMenu())->GetSubMenu(2);
		ClientToScreen(&ScreenPoint);
		pMenu->TrackPopupMenu(TPM_RIGHTBUTTON, ScreenPoint.x, ScreenPoint.y, AfxGetMainWnd(), NULL);
		CFormView::OnRButtonDown(nFlags, point);
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::DoZoomInAlongOx()
	{
		m_GraphRect.right += ZOOMSTEP;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::DoZoomOutAlongOx()
	{
		m_GraphRect.right -= ZOOMSTEP;
		if(m_GraphRect.right < ZOOMSTEP)
			m_GraphRect.right += ZOOMSTEP;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::DoZoomInAlongOy()
	{
		m_GraphRect.bottom += ZOOMSTEP;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::DoZoomOutAlongOy()
	{
		m_GraphRect.bottom -= ZOOMSTEP;
		if(m_GraphRect.bottom < ZOOMSTEP)
			m_GraphRect.bottom += ZOOMSTEP;
	}
	//------------------------------------------------------------------------------------------//
	CString CGraphViewerView::GetError(funcParams &params, CString strs[])
	{
		CString errorStr = CheckFunctionName(params);
		for(int i = 0; i < 3; i++)
		{
			char *chPtr = const_cast<char*>((LPCSTR)strs[i]);
			if(!FParser::isNumber(chPtr, chPtr + strs[i].GetLength()))
			{
				errorStr += "Some parameter(s) in edit box(es) is(are) not a number.";
				return errorStr;
			}
			strs[i].Remove(' ');
		}
		params.from = atof((LPCTSTR)strs[0]);
		params.to = atof((LPCTSTR)strs[1]);
		params.delta = atof((LPCTSTR)strs[2]);

		if(params.delta <= 0)
			return (errorStr += "The delta parameter must be positive.\n");

		if(params.from >= params.to)
			errorStr += "The Final value should be greater than Initial value.\n";

		if((params.to - params.from)/params.delta + 1 < MINSTEPCOUNT)
		{
			CString minStr;
			minStr.Format("%d", MINSTEPCOUNT);

			errorStr += "Not enough iterations to build a graph.\nThe minimum is " + minStr + ".\n";
		}
		return errorStr;
	}
	//------------------------------------------------------------------------------------------//
	CString CGraphViewerView::CheckFunctionName(funcParams &params)
	{
		params.fName.Remove(' ');
		params.fName.MakeLower();
		CString errStr = "";
		if(!(params.fName.GetLength()))
			errStr = "The function field is empty.\n";
		else
			if(count((LPCSTR)params.fName, (LPCSTR)params.fName + params.fName.GetLength(), '(') != 
				count((LPCSTR)params.fName, (LPCSTR)params.fName + params.fName.GetLength(), ')'))
				errStr = "There is a problem with brackets.\n";
		errStr += CheckString(params.fName);
		return errStr;
	}
	//------------------------------------------------------------------------------------------//
	CString CGraphViewerView::CheckString(CString &str)
	{
		LPCSTR pStr = (LPCSTR)str;
		CString errStr = "";

		int len = strlen(pStr);

		/*********************Looking for function*****************/
		LPCSTR pos = strchr(pStr, '(');

		while(pos)
		{  
			LPCSTR it = pos;
			vector<char> funcName;
			while((--it) >= pStr && !ISOP(*it))
				funcName.push_back(*it);
			reverse(funcName.begin(), funcName.end());
			funcName.push_back('\0');
			for(int i = 0; i < ARRSIZE(fncs); i++)
				if(!strcmp(&funcName[0], fncs[i].fName))
				{
					funcName.clear();
					break;
				}
				int sz = funcName.size() == 1 ? 0 : funcName.size();
				if(sz)
				{
					errStr = &funcName[0];
					return (errStr += " is unrecognized function.");
				}
				if(pos==pStr + len)
					break;
				pos = strchr(pos + 1, '(');
		}
		/***********************Looking for operation******************/
		return "";
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::Reset()
	{
		CGraphViewerDoc* pDoc = GetDocument();
		m_scrlpos.x = m_scrlpos.y = 0;
		m_showPt = false;
		m_clArg = 0.0;
		//pDoc->m_fAr.push_back(Function());
		//size_t sz = pDoc->m_fAr.size();
		//m_curFunc = &(pDoc->m_fAr.front());

		CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
		if(pMain)
			pMain->SetStatusText("", "");
		SetScrollSizes(MM_TEXT, CSize(0, 0), CSize(m_clRect.right, m_clRect.bottom), 
			CSize(m_interval, m_interval));
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerView::OnGoto()
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
		if(!pMain)
			return;
		CString str;
		pMain->GetGotoContent(str);
		if(!FParser::isNumber((LPCSTR)str, (LPCSTR)str + str.GetLength()))
			return;
		double to = atof((LPCSTR)str);
		if(to < m_curFunc->params.from || to > m_curFunc->params.to)
			return;
		SetScrollPos(SB_HORZ, m_graph->ArgToScreenX(to));
		UpdateDraw();
	}
	//------------------------------------------------------------------------------------------//


