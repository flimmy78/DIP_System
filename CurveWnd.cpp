// AdjustCurveWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CurveDlg.h"
#include "CurveWnd.h"

#define BUTTON_WIDTH       40
#define BUTTON_HEIGHT      20

// CCurveWnd

IMPLEMENT_DYNAMIC(CCurveWnd, CWnd)

// ����
CCurveWnd::CCurveWnd()
{
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = NULL;
	}
	m_pComboBoxRGB = NULL;
	m_pBnGroupBox = NULL;
	m_pBnChooseCurve = NULL;
	m_pBnChooseLine = NULL;
	m_pBnReset = NULL;
	m_pBnInverse = NULL;
	m_pBnImportCurve = NULL;
	m_pBnExportCurve = NULL;
	m_pBnOK = NULL;
	m_pBnCancel = NULL;
	m_pXPosition = NULL;
	m_pYPosition = NULL;
	m_pXlabel = NULL;
	m_pYlabel = NULL;
	m_pOnPaintRect = NULL;
	m_nChannelSelected = CHANNEL_RGB;//Ĭ��ͨ����RGB
	m_bUserWillDrag = FALSE;
	m_posCurrentPeg = NULL;
	m_nLinePattern = LINE_PATTERN_SPLINE;
	m_bInverseImage = FALSE;
	//ͼ����Ϣ
	m_dataSrc = NULL;
	m_curData = NULL;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nPixelQuantity = 0;
	m_nbytesWidth = 0;
	m_nChannel = 0;
	m_nlenData = 0;

	//OnSize
	m_pWndSize = NULL;

	//Font
	m_FontName = L"Arial";
	m_nFontSize = 100;
}

// ����
CCurveWnd::~CCurveWnd()
{
	for (int i = 0; i<4; i++)
	{
		if (m_pfHistogram[i] != NULL)
		{
			delete [] m_pfHistogram[i];
		}

		m_V4Transform[i].~vector();

		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint *Temp = m_pPegsList[i].GetNext(pos).m_pt;
			delete Temp;
		}
		m_pPegsList[i].RemoveAll();
	}
	if (m_pComboBoxRGB != NULL)  {	delete m_pComboBoxRGB;}
	if (m_pBnGroupBox != NULL)   {	delete m_pBnGroupBox;}
	if (m_pBnChooseCurve != NULL){	delete m_pBnChooseCurve;}
	if (m_pBnChooseLine != NULL) {	delete m_pBnChooseLine;}
	if (m_pBnReset != NULL)      {	delete m_pBnReset;}
	if (m_pBnInverse != NULL)    {	delete m_pBnInverse;}
	if (m_pBnImportCurve != NULL){	delete m_pBnImportCurve;}
	if (m_pBnExportCurve != NULL){	delete m_pBnExportCurve;}
	if (m_pBnOK != NULL)         {	delete m_pBnOK;}
	if (m_pBnCancel != NULL)     {	delete m_pBnCancel;}
	if (m_pXPosition != NULL)    {	delete m_pXPosition;}
	if (m_pYPosition != NULL)    {	delete m_pYPosition;}
	if (m_pXlabel != NULL)       {	delete m_pXlabel;}
	if (m_pYlabel != NULL)       {	delete m_pYlabel;}
	if (m_pOnPaintRect != NULL)  {	delete m_pOnPaintRect;}
	if (m_dataSrc != NULL)       {  delete [] m_dataSrc;}
	if (m_curData != NULL)       {  delete [] m_curData;}
	if (m_pWndSize != NULL)      {  delete m_pWndSize;}
	if (m_pAppFont != NULL)      {	delete m_pAppFont;}
}

BEGIN_MESSAGE_MAP(CCurveWnd, CWnd)
	//������������Ϣӳ��
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//�����Ϣӳ��
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	//��ť��Ϣӳ��
	ON_CBN_SELCHANGE(IDC_COMBOBOX_RGB, &CCurveWnd::OnComboboxRgb)
	ON_BN_CLICKED(IDB_CHOOSE_CURVE, OnChooseCurve)
	ON_BN_CLICKED(IDB_CHOOSE_LINE, &CCurveWnd::OnChooseLine)
	ON_BN_CLICKED(IDB_CURVEWND_RESET, &CCurveWnd::OnCmdCurvewndReset)
	ON_BN_CLICKED(IDB_CURVEWND_INVERSE, &CCurveWnd::OnCmdCurvewndInverse)
	ON_BN_CLICKED(IDB_IMPORT_CURVE, &CCurveWnd::OnCmdImportCurve)
	ON_BN_CLICKED(IDB_EXPORT_CURVE, &CCurveWnd::OnCmdExportCurve)
	ON_BN_CLICKED(IDB_CURVE_OK, &CCurveWnd::OnCmdCurveOk)
	ON_BN_CLICKED(IDB_CURVE_CANCEL, &CCurveWnd::OnCmdCurveCancel)
END_MESSAGE_MAP()

// CCurveWnd ��Ϣ��������

int CCurveWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pAppFont = new CFont;
	m_pAppFont->CreatePointFont(m_nFontSize, m_FontName);

	m_pWndSize = new CRect;
	GetClientRect(m_pWndSize);

	int nButtonPosX1 = m_pWndSize->right-50;
	int nButtonPosX2 = nButtonPosX1+BUTTON_WIDTH;
	double Space = m_pWndSize->Height()/9.0;

	//��ͼ��Rect
	m_pOnPaintRect = new CRect(40, 40, m_pWndSize->right-70, m_pWndSize->bottom-80);

	//���������255���ű�
	m_nRectWidth = m_pOnPaintRect->Width();
	m_nRectHeight = m_pOnPaintRect->Height();
	m_fWidthRatio = m_nRectWidth/255.0;
	m_fHeightRatio = m_nRectHeight/255.0;

	//Group Box
	m_pBnGroupBox = new CButton;
	m_pBnGroupBox->Create(L"ͨ��", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(20, 20, m_pWndSize->right-60, m_pWndSize->bottom-60), this, IDB_GROUP_BOX);
	m_pBnGroupBox->SetFont(m_pAppFont, TRUE);

	//ͨ��:RGB, ��Ͽ������б��߶��ڴ�����
	m_pComboBoxRGB = new CComboBox;
	m_pComboBoxRGB->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(70, 15, 130, 115), this, IDC_COMBOBOX_RGB);
	m_pComboBoxRGB->AddString(L"RGB");
	m_pComboBoxRGB->AddString(L"��");
	m_pComboBoxRGB->AddString(L"��");
	m_pComboBoxRGB->AddString(L"��");
	m_pComboBoxRGB->SetCurSel(0);
	m_pComboBoxRGB->SetFont(m_pAppFont, TRUE);

	//ѡ������
	m_pBnChooseCurve = new CMFCButton;
	m_pBnChooseCurve->Create(L"����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(Space), nButtonPosX2, int(Space+BUTTON_HEIGHT)), this, IDB_CHOOSE_CURVE);
	m_pBnChooseCurve->SetFont(m_pAppFont, TRUE);
	m_pBnChooseCurve->SetTooltip(L"����������ֵ");

	//ѡ��ֱ��
	m_pBnChooseLine = new CMFCButton;
	m_pBnChooseLine->Create(L"ֱ��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(2*Space), nButtonPosX2, int(2*Space+BUTTON_HEIGHT)), this, IDB_CHOOSE_LINE);
	m_pBnChooseLine->SetFont(m_pAppFont, TRUE);
	m_pBnChooseLine->SetTooltip(L"�ֶ����Բ�ֵ");

	//����
	m_pBnReset = new CMFCButton;
	m_pBnReset->Create(L"����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(3*Space), nButtonPosX2, int(3*Space+BUTTON_HEIGHT)), this, IDB_CURVEWND_RESET);
	m_pBnReset->SetFont(m_pAppFont, TRUE);
	m_pBnReset->SetTooltip(L"�������в���");

	//��ת
	m_pBnInverse = new CMFCButton;
	m_pBnInverse->Create(L"��ת", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(4*Space), nButtonPosX2, int(4*Space+BUTTON_HEIGHT)), this, IDB_CURVEWND_INVERSE);
	m_pBnInverse->SetFont(m_pAppFont, TRUE);
	m_pBnInverse->SetTooltip(L"��תͼ��");

	//��������
	m_pBnImportCurve = new CMFCButton;
	m_pBnImportCurve->Create(L"����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(5*Space), nButtonPosX2, int(5*Space+BUTTON_HEIGHT)), this, IDB_IMPORT_CURVE);
	m_pBnImportCurve->SetFont(m_pAppFont, TRUE);
	m_pBnImportCurve->SetTooltip(L"������������");

	//��������
	m_pBnExportCurve = new CMFCButton;
	m_pBnExportCurve->Create(L"����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(6*Space), nButtonPosX2, int(6*Space+BUTTON_HEIGHT)), this, IDB_EXPORT_CURVE);
	m_pBnExportCurve->SetFont(m_pAppFont, TRUE);
	m_pBnExportCurve->SetTooltip(L"�����任����");

	//X����
	m_pXPosition = new CEdit;
	m_pXPosition->Create(WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1+20, int(7*Space), nButtonPosX2, int(7*Space+BUTTON_HEIGHT)), this, IDE_CURVEWND_XPOSITION);
	m_pXPosition->SetFont(m_pAppFont, TRUE);

	//Y����
	m_pYPosition = new CEdit;
	m_pYPosition->Create(WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1+20, int(8*Space), nButtonPosX2, int(8*Space+BUTTON_HEIGHT)), this, IDE_CURVEWND_YPOSITION);
	m_pYPosition->SetFont(m_pAppFont, TRUE);

	//X��ǩ
	m_pXlabel = new CStatic;
	m_pXlabel->Create(L"X:", WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1, int(7*Space), nButtonPosX2-30, int(7*Space+BUTTON_HEIGHT)), this, IDS_CURVEWND_XLABEL);
	m_pXlabel->SetFont(m_pAppFont, TRUE);

	//Y��ǩ
	m_pYlabel = new CStatic;
	m_pYlabel->Create(L"Y:", WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1, int(8*Space), nButtonPosX2-30, int(8*Space+BUTTON_HEIGHT)), this, IDS_CURVEWND_YLABEL);
	m_pYlabel->SetFont(m_pAppFont, TRUE);

	//OK
	m_pBnOK = new CMFCButton;
	m_pBnOK->Create(L"ȷ��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(40, m_pWndSize->bottom-40, 40+BUTTON_WIDTH, m_pWndSize->bottom-40+BUTTON_HEIGHT), this, IDB_CURVE_OK);
	m_pBnOK->SetFont(m_pAppFont, TRUE);
	m_pBnOK->SetTooltip(L"�����޸�");

	//CANCEL
	m_pBnCancel = new CMFCButton;
	m_pBnCancel->Create(L"ȡ��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(m_pWndSize->right-70-BUTTON_WIDTH, m_pWndSize->bottom-40, m_pWndSize->right-70, m_pWndSize->bottom-40+BUTTON_HEIGHT), this, IDB_CURVE_CANCEL);
	m_pBnCancel->SetFont(m_pAppFont, TRUE);
	m_pBnCancel->SetTooltip(L"ȡ���޸�");

	//*** m_pImage ***//
	m_pImage = GetImage();

	//��ʼ��ֱ��ͼ
	GetHistogram();

	return 0;
}

//
// *** ���� ***
//

//��ȡ��ĵ�
CDIP_SystemDoc* CCurveWnd::GetDocument()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemDoc *)pChild->GetActiveDocument();
}

//��ȡ���ͼ
CDIP_SystemView* CCurveWnd::GetActiveView()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemView *)pChild->GetActiveView();
}

//��ȡͼ��ָ��
CImage* CCurveWnd::GetImage()
{
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return NULL;
	return pDoc->DIBImage();
}

//��ȡֱ��ͼ
BOOL CCurveWnd::GetHistogram()
{
	if (m_pImage == NULL) return FALSE;

	//��ʼ��pegs
	for (int i = 0; i<4; i++)
	{
		peg pBegin;
		pBegin.m_dRatioX = 0;
		pBegin.m_dRatioY = 1;
		pBegin.m_pt = new CPoint(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		
		peg pEnd;
		pEnd.m_dRatioX = 1;
		pEnd.m_dRatioY = 0;
		pEnd.m_pt = new CPoint(m_pOnPaintRect->right, m_pOnPaintRect->top);
		m_pPegsList[i].AddTail(pBegin);
		m_pPegsList[i].AddTail(pEnd);
	}

	//��ȡͼ����Ϣ
	m_nImageWidth = m_pImage->GetWidth();
	m_nImageHeight = m_pImage->GetHeight();
	m_nPixelQuantity = m_nImageWidth*m_nImageHeight;
	m_nChannel = m_pImage->GetBPP()/8;
	m_nbytesWidth = abs(m_pImage->GetPitch());
	m_nlenData = m_nbytesWidth*m_nImageHeight;
	//��ͼ��Դ���ݴ�������
	m_dataSrc = new BYTE[m_nlenData];
	m_curData = new BYTE[m_nlenData];
	m_pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));//���ͼ�������׵�ַ
	memcpy(m_dataSrc, m_pBits, m_nlenData);
	memcpy(m_curData, m_pBits, m_nlenData);

	//��ʼ��ֱ��ͼ
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = new double[256];
		for (int j = 0; j<256; j++)
		{
			m_pfHistogram[i][j] = 0;
		}
	}

	//����ֱ��ͼ
	BYTE *Pixel;
	for (int i = 0; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{//RGB24������������˳��Ϊ��BGR, RGB32������������˳��Ϊ��BGRA
			Pixel = m_dataSrc + i * m_nbytesWidth + j * m_nChannel;
			m_pfHistogram[3][int(*Pixel)]++;                                              //Blue
			m_pfHistogram[2][int(*(Pixel+1))]++;                                          //Green
			m_pfHistogram[1][int(*(Pixel+2))]++;                                          //Red
			if (*Pixel == *(Pixel+1) && *(Pixel+1) == *(Pixel+2))
			{//�˴����ֱ��ͼ��ĳЩ�Ҷ�ȱʧ������,2014.4.14
				m_pfHistogram[0][int(*Pixel)]++;                                              //RGB
			}
			else
			{
				m_pfHistogram[0][int(*Pixel*0.299 + *(Pixel+1)*0.587 + *(Pixel+2)*0.114)]++;  //RGB
			}
		}
	}

	//��һ��
	for (int i = 0, j; i<4; i++)
	{
		for (j = 0; j<256; j++)
		{
			m_pfHistogram[i][j] = m_pfHistogram[i][j]/m_nPixelQuantity;
		}
	}
	return TRUE;
}

//���Ƿ��Ѵ���,��������Ҫ�滻
BOOL CCurveWnd::PegIsExist(CPoint& point)
{
	//���Ƿ��Ѵ���
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	double errorX = max(m_nRectWidth/100, 3.0), errorY = max(m_nRectHeight/100, 3.0);
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		double err = fabs( 1.0*point.x - Temp.m_pt->x );//x����֮��
		if (Temp.m_pt->x == m_pOnPaintRect->left || Temp.m_pt->x == m_pOnPaintRect->right)
		{//���ڱ߽�㣺�߽���x���겻�ܱ��޸�
			if (err<errorX)
			{
				m_posCurrentPeg = prev;
				Temp.m_pt->y = point.y;
				Temp.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
				break;
			}
		}
		else if (err == 0) //x�������,�˴������⡣����ܵ���ͼ�񻭲�������2014.4.23��ע
		{
			return -1;
		}
		else if ( err < errorX && fabs( 1.0*point.y - Temp.m_pt->y ) < errorY)
		{//���ڷǱ߽�㣺����pegs֮������С���㲻������peg,��ʱ�滻peg
			m_posCurrentPeg = prev;
			*(Temp.m_pt) = point;
			Temp.m_dRatioX = 1.0*(point.x-m_pOnPaintRect->left)/m_nRectWidth;
			Temp.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
			break;
		}
	}
	if (pos != NULL)
	{//peg�Ѿ�����
		return TRUE;
	}
	return FALSE;
}

//����һ���µ�peg
POSITION CCurveWnd::AddNewPeg(CPoint& point)
{
	//�����ж�peg������
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		if (point.x < Temp.m_pt->x )
		{
			//��peg�嵽Tempǰ��
			peg NewPoint;
			NewPoint.m_pt = new CPoint(point);
			NewPoint.m_dRatioX = 1.0*(point.x-m_pOnPaintRect->left)/m_nRectWidth;
			NewPoint.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
			m_pPegsList[m_nChannelSelected].InsertBefore(prev, NewPoint);
			m_pPegsList[m_nChannelSelected].GetPrev(prev);//����peg��λ��
			m_posCurrentPeg = prev;
			return prev;
		}
	}
	return NULL;
}

//�û���ı�ĳ��peg������Ϊ�������λ����ĳ��peg�ܽӽ�
BOOL CCurveWnd::RePlacePeg(CPoint& point)
{
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		if (fabs(1.0*point.x - Temp.m_pt->x)<max(m_nRectWidth/100, 3.0) || fabs(1.0*point.y - Temp.m_pt->y)<max(m_nRectHeight/100, 3.0))
		{
			//��prevλ�õ�pegɾ��
			if (Temp.m_pt->x != m_pOnPaintRect->left && Temp.m_pt->x != m_pOnPaintRect->right)
			{//�߽�㲻�ܱ�ɾ��
				delete m_pPegsList[m_nChannelSelected].GetAt(prev).m_pt;
				m_pPegsList[m_nChannelSelected].RemoveAt(prev);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//�������û�������һ��peg
BOOL CCurveWnd::UserWillDrag(CPoint& point)
{
	int error = point.y - m_V4Transform[m_nChannelSelected][point.x - m_pOnPaintRect->left];
	if (error < max(m_nRectHeight/100, 16)) return TRUE;
	return FALSE;
}

//��������:����m_pPegList��ֵ��[0,255]�ĻҶȱ任
void CCurveWnd::SplineFunc()
{
    //ԭ��: VECTOR Spline(VECTOR Node, VECTOR Value, VECTOR x0, double c1, double c2, int CASE = 2)
	if (m_pImage == NULL) return;
	VECTOR Node, Value, x0, y0;
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
		Node.push_back(double(Temp->x));
		Value.push_back(double(Temp->y));
	}
	for (int i = m_pOnPaintRect->left; i<=m_pOnPaintRect->right; i++)
	{
		x0.push_back(double(i));
	}
	y0 = Spline(Node, Value, x0, 0, 0);
	for (int i = m_pOnPaintRect->left, j = 0; i<=m_pOnPaintRect->right; i++, j++)
	{
		m_V4Transform[m_nChannelSelected][j] = int(y0[j]);
	}
	Threshold();
	Node.~vector();
	Value.~vector();
	x0.~vector();
	y0.~vector();
	return;
}

//�ֶ����Ժ���:����m_pPegList��ֵ��[0,255]�ĻҶȱ任
void CCurveWnd::PieceWiseFunc()
{
	if (m_pImage == NULL) return;
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
		if (pos != NULL)
		{
			CPoint *Next = m_pPegsList[m_nChannelSelected].GetAt(pos).m_pt;
			double Slope = double(Temp->y - Next->y)/(Temp->x - Next->x);
			for (int i = Temp->x; i<=Next->x; i++)
			{
				m_V4Transform[m_nChannelSelected][i-m_pOnPaintRect->left] = long(Slope*(i-Temp->x) + Temp->y);
			}
		}
		else
		{
			break;
		}
	}
}

//����ڻ�ͼ����֮��
BOOL CCurveWnd::CursorIsInPaintRect(CPoint& point)
{
	if (point.x >= m_pOnPaintRect->left && point.x <= m_pOnPaintRect->right && point.y >= m_pOnPaintRect->top && point.y <= m_pOnPaintRect->bottom) return TRUE;
	return FALSE;
}

//ת�����꣬����ԭ��ΪDialog�����ϽǶ���
CPoint CCurveWnd::TransPositionToDialog(CPoint& point)
{
	return CPoint(point.x + m_pOnPaintRect->left, m_pOnPaintRect->bottom - point.y);
}

//ת�����꣬����ԭ��ΪRect�����½Ƕ���
CPoint CCurveWnd::TransPositionToRectangle(CPoint& point)
{
	return CPoint(point.x - m_pOnPaintRect->left, m_pOnPaintRect->bottom - point.y);
}

//���ݲ�ֵ�����ĻҶȱ任�������Ҷȱ任
BOOL CCurveWnd::GrayTransform()
{
	if (m_pImage == NULL) return FALSE;

	//ÿ�λҶȱ任ʱ����Ҫ��������ͼ��Դ����
	//��ֹÿ�λҶȱ任��Ч�����Ӳ�������Ч��
	memcpy(m_curData, m_dataSrc, m_nlenData);
	//ֱ��ͼԤ������ʹ�ú�����ͷ�TempCell���ڴ�
	int* TempCell = PreTransform();
	BYTE* Pixel;
	if (m_nChannelSelected == CHANNEL_RGB)
	{//RGBͨ��
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				Pixel = m_curData + i * m_nbytesWidth + j * m_nChannel;
				for (int k = 0; k<m_nChannel; k++)
				{//����ڴ��������,2014.4.17
					*(Pixel+k) = TempCell[int(*(Pixel+k))];
				}
			}
		}
	}
	else
	{//����ͨ��
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				Pixel = m_curData + i * m_nbytesWidth + j * m_nChannel;
				*(Pixel+3-m_nChannelSelected) = TempCell[int(*(Pixel+3-m_nChannelSelected))];
			}
		}
	}
	//�޸�ͼ��
	memcpy(m_pBits, m_curData, m_nlenData);
	delete [] TempCell;
	return TRUE;
}

//�Գ���Rect�ı任ֵ���д���
void CCurveWnd::Threshold()
{
	for (int i = 0; i<m_nRectWidth+1; i++)
	{
		if(m_V4Transform[m_nChannelSelected][i]<m_pOnPaintRect->top)
		{
			m_V4Transform[m_nChannelSelected][i] = m_pOnPaintRect->top;
			continue;
		}
		if(m_V4Transform[m_nChannelSelected][i]>m_pOnPaintRect->bottom)
		{//bottom-1�˶�����Σ����top��bottom���Ʋ���ֱ�ߵ�����.2014.4.15
			m_V4Transform[m_nChannelSelected][i] = m_pOnPaintRect->bottom-1;
			continue;
		}
	}
}

//��m_pnTransformԤ����,������256�����ݣ���Ӧ��0��255�ĻҶȱ任ֵ
//ʹ�ú����ʹ��delete�ͷ��ڴ�
int* CCurveWnd::PreTransform()
{
	int *Result = new int[256];
	for (int j = 0; j<256; j++)
	{
		Result[j] = int ((m_pOnPaintRect->bottom - m_V4Transform[m_nChannelSelected][int(j*m_fWidthRatio)])/m_fHeightRatio);
	}
	return Result;
}

//ͼ��ת
void CCurveWnd::InverseTransform()
{
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		int gray = m_pOnPaintRect->bottom - Temp.m_pt->y;
		Temp.m_pt->y = m_pOnPaintRect->top + gray;
		Temp.m_dRatioY = 1.0*gray/m_nRectHeight;
	}
}

//����ˢ��ͼ��:�ȶ�ͼ����лҶȱ任��֮��ѵ�ǰ״̬����ջ
void CCurveWnd::ImshowImmediately()
{
	if(GrayTransform() == FALSE) return;

	CDIP_SystemView* pView = GetActiveView();
	memcpy(m_pBits, m_curData, m_nlenData);
	pView->Invalidate();
	pView->UpdateWindow();
}

//�ȸ���pegs����ֵ��Ȼ����Ʊ任����
void CCurveWnd::ShowGrayTransform()
{
	if (m_pImage == NULL) return;
	switch (m_nLinePattern)
	{//��Ҫ��ʲô����
	case LINE_PATTERN_SPLINE:
		SplineFunc();
		break;
	case LINE_PATTERN_LINEAR:
		PieceWiseFunc();
		break;
	default:
		SplineFunc();
		break;
	}
	InvalidateRect(m_pOnPaintRect);
}

//���浱ǰ״̬�������л�ͨ��ʱ�򱣴�ͼ��
void CCurveWnd::SaveCurrentState()
{
	if (m_pImage == NULL) return;

	memcpy(m_dataSrc, m_curData, m_nlenData);
}

//
//*** ��Ӧ ***
//

//����
void CCurveWnd::OnPaint()
{
	//��ָ������rect�������ֱ��ͼ
	CPaintDC dc(this);
	CBitmap MemBitmap;
	CDC MemDC;
	CRect WndSize;
	GetClientRect(&WndSize);
	MemDC.CreateCompatibleDC(&dc);
	MemBitmap.CreateCompatibleBitmap(&dc, WndSize.Width(), WndSize.Height());

	CBitmap* pOldBmp = MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(m_pOnPaintRect, RGB(240, 240, 240));
	CPen *pOldPen, newPen;
	COLORREF LineColor;

	//ˮƽ������ɫ��
	for (int i = m_pOnPaintRect->left, k = 0; i<m_pOnPaintRect->right; i++, k++)
	{
		LineColor = ColorSetting(m_nChannelSelected, int(k/m_fWidthRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(i, m_pOnPaintRect->bottom + 5);
		MemDC.LineTo(i, m_pOnPaintRect->bottom + 10);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}
	//��ֱ������ɫ��
	for (int j = m_pOnPaintRect->top, k = 0; j<m_pOnPaintRect->bottom; j++, k++)
	{

		LineColor = ColorSetting(m_nChannelSelected, int(k/m_fHeightRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(m_pOnPaintRect->left - 10, m_pOnPaintRect->bottom-j);
		MemDC.LineTo(m_pOnPaintRect->left - 5, m_pOnPaintRect->bottom-j);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	if (m_pImage != NULL)
	{
		//ֱ��ͼ
		LineColor = ColorSetting(m_nChannelSelected, 128);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		double rate = 0.20*m_nRectHeight*m_nRectWidth/m_fWidthRatio;//ʹֱ��ͼ������������ϵ���ұ��ڹ۲�
		for (int i = 0, nTemp; i<m_nRectWidth; i++)
		{
			nTemp = m_pOnPaintRect->left+i;
			MemDC.MoveTo(nTemp, m_pOnPaintRect->bottom);
			MemDC.LineTo(nTemp, int(m_pOnPaintRect->bottom - m_pfHistogram[m_nChannelSelected][int(i/m_fWidthRatio)]*rate));
		}
		newPen.DeleteObject();

		//�任����
		LineColor = ColorSetting(m_nChannelSelected, 255);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		for (int i = 1; i<=m_nRectWidth; i++)
		{
			MemDC.MoveTo(m_pOnPaintRect->left + i-1, m_V4Transform[m_nChannelSelected][i-1]);
			MemDC.LineTo(m_pOnPaintRect->left + i, m_V4Transform[m_nChannelSelected][i]);
		}
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();

		//pegs
		CBrush *pOldBrush, newBrush;
		newPen.CreatePen(PS_SOLID, 1, RGB(0, 128, 128));
		pOldPen = MemDC.SelectObject(&newPen);
		POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
		while (pos != NULL)
		{
			if (pos != m_posCurrentPeg)
			{//�ǵ�ǰק����peg
				CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
				newBrush.CreateSolidBrush(RGB(0, 128, 128));
				pOldBrush = MemDC.SelectObject(&newBrush);
				MemDC.Rectangle(CRect(CPoint(Temp->x - 3, Temp->y - 3), CPoint(Temp->x + 3, Temp->y + 3)));
				MemDC.SelectObject(pOldBrush);
				newBrush.DeleteObject();
			}
			else
			{//��ǰק����peg
				CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
				newBrush.CreateSolidBrush(RGB(255, 0, 0));
				pOldBrush = MemDC.SelectObject(&newBrush);
				MemDC.Rectangle(CRect(CPoint(Temp->x - 3, Temp->y - 3), CPoint(Temp->x + 3, Temp->y + 3)));
				MemDC.SelectObject(pOldBrush);
				newBrush.DeleteObject();
			}
		}
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	//�Խ���,Ӧ�û���ֱ��ͼ���棬���ⱻ����
	newPen.CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	pOldPen = MemDC.SelectObject(&newPen);
	if (m_bInverseImage)
	{//ͼ���ѷ�ת,�Խ���Ҫ�仯,2014.4.15
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->top);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->bottom);
	}
	else
	{
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->top);
	}
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();

	//����
	dc.BitBlt(m_pOnPaintRect->left, m_pOnPaintRect->top, m_nRectWidth, m_nRectHeight, &MemDC, m_pOnPaintRect->left, m_pOnPaintRect->top, SRCCOPY);
	dc.BitBlt(m_pOnPaintRect->left - 10, m_pOnPaintRect->top, 5, m_nRectHeight, &MemDC, m_pOnPaintRect->left - 10, m_pOnPaintRect->top, SRCCOPY);    //��ֱ color bar
	dc.BitBlt(m_pOnPaintRect->left, m_pOnPaintRect->bottom + 5, m_nRectWidth, 5, &MemDC, m_pOnPaintRect->left, m_pOnPaintRect->bottom + 5, SRCCOPY); //ˮƽ color bar
	MemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	MemBitmap.DeleteObject();
}

void CCurveWnd::OnChooseCurve()
{
	m_nLinePattern = LINE_PATTERN_SPLINE;
	SplineFunc();
	GrayTransform();
	/*ˢ����ͼ*/
	InvalidateRect(m_pOnPaintRect);
	m_pBnChooseCurve->EnableWindow(FALSE);
	m_pBnChooseLine->EnableWindow(TRUE);
}

void CCurveWnd::OnChooseLine()
{
	m_nLinePattern = LINE_PATTERN_LINEAR;
	PieceWiseFunc();
	GrayTransform();
	/*ˢ����ͼ*/
	InvalidateRect(m_pOnPaintRect);
	m_pBnChooseCurve->EnableWindow(TRUE);
	m_pBnChooseLine->EnableWindow(FALSE);
}

void CCurveWnd::OnComboboxRgb()
{
	// TODO: �ڴ�����������������
	m_nChannelSelected = m_pComboBoxRGB->GetCurSel();
	SaveCurrentState();
	//�޸�peg������
	ReshapePegs();
	//�޸ĻҶȱ任����
	ReshapeTransform();
	// ��ɫ��Ҳ����
	CRect rect = CRect(m_pOnPaintRect->left - 10, m_pOnPaintRect->top, m_pOnPaintRect->right, m_pOnPaintRect->bottom + 10);
	InvalidateRect(rect);
	//��1���������������ߺ��л�ͨ�������任����������,2014.4.14
	ShowGrayTransform();
	//GrayTransform();//��2������ֱ��ͼ���֡����Ѵ���.2014.6.11
	//�����÷����������µķ���������⣨1��
}

void CCurveWnd::OnCmdCurvewndReset()
{
	// TODO: �ڴ�����������������
	// ����pegs
	for (int i = 0; i<4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint* pt = m_pPegsList[i].GetNext(pos).m_pt;
			delete pt;
		}
		m_pPegsList[i].RemoveAll();
		peg pBegin;
		pBegin.m_pt = new CPoint(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		pBegin.m_dRatioX = 0;
		pBegin.m_dRatioY = 1;
		peg pEnd;
		pEnd.m_pt = new CPoint(m_pOnPaintRect->right, m_pOnPaintRect->top);
		pEnd.m_dRatioX = 1;
		pEnd.m_dRatioY = 0;
		m_pPegsList[i].AddTail(pBegin);
		m_pPegsList[i].AddTail(pEnd);
	}
	//���ñ任����
	ShowGrayTransform();
	//һЩ��������
	m_bInverseImage = FALSE;
	//������ͼ
	ImshowImmediately();//�������ʧЧ����,2014.4.15
}

void CCurveWnd::OnCmdCurvewndInverse()
{
	// TODO: �ڴ�����������������
	m_bInverseImage = !m_bInverseImage;
	InverseTransform();
	ShowGrayTransform();
	ImshowImmediately();//�����תʧЧ����,2014.4.15
}

void CCurveWnd::OnCmdImportCurve()
{
	if (m_pImage == NULL) return;

	CString FilePath = L"";
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY, TEXT("curve(*.cv)|*.cv||"), NULL);
	hFileDlg.m_ofn.hwndOwner = m_hWnd;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("�������ļ�...\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if(hFileDlg.DoModal() == IDOK)
	{
		FilePath = hFileDlg.GetPathName();
	}
	else return;

	CFile file;
	if(!file.Open(FilePath, CFile::modeRead))
	{
		MessageBox(L"���ļ�"+FilePath+L"ʧ��!");
		return ;
	}
	CArchive ar(&file, CArchive::load);
	//����pegs
	for (int i = 0, PegsQuantity; i<4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint* pt = m_pPegsList[i].GetNext(pos).m_pt;
			delete pt;
		}
		m_pPegsList[i].RemoveAll();
		ar>>PegsQuantity;
		for(int j = 0; j<PegsQuantity; j++)
		{
			peg Temp;
			Temp.m_pt = new CPoint;
			ar>>*(Temp.m_pt);
			ar>>Temp.m_dRatioX;
			ar>>Temp.m_dRatioY;
			m_pPegsList[i].AddTail(Temp);
		}
	}
	ar.Close();
	file.Close();
	//�任��ˢ����ͼ
	int Swap = m_nChannelSelected;
	for (int i = 0; i<4; i++)
	{   //����������������֮���л�ͨ����ʾ������������
		//Ҳ���ֱ��ͼ����"���Ѵ�"������(���л�ͨ������)
		//�� 2014.4.14 �� 2014.6.11 �����©��.
		m_nChannelSelected = i;
		ReshapeTransform();
		ShowGrayTransform();
		GrayTransform();
		SaveCurrentState();
	}
	m_nChannelSelected = Swap;
	ImshowImmediately();
}

void CCurveWnd::OnCmdExportCurve()
{
	if (m_pImage == NULL) return;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CString CurTime;
	CurTime.Format(L"%04d%02d%02d%02d%02d%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	CString FilePath = L"";

	CFileDialog dlg(FALSE, NULL, CurTime, NULL, TEXT("curve(*.cv)|*.cv||"), NULL);
	dlg.m_ofn.hwndOwner = m_hWnd;
	dlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	dlg.m_ofn.lpstrTitle = TEXT("��������...\0");
	dlg.m_ofn.nMaxFile = MAX_PATH;

	if ( dlg.DoModal() != IDOK)
		return;
	else
	{
		FilePath += dlg.GetPathName();
		if (FilePath.Right(3) != L".cv")
		{
			FilePath += L".cv";
		}
	}

	CFile file;
	file.Open(FilePath, CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	for (int i = 0; i<4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		ar<<m_pPegsList[i].GetSize();
		while (pos != NULL)
		{
			peg& Temp = m_pPegsList[i].GetNext(pos);
			ar<<*(Temp.m_pt);
			ar<<Temp.m_dRatioX;
			ar<<Temp.m_dRatioY;
		}
	}
	ar.Close();
	file.Close();
	MessageBox(L"�����Ѵ���:"+FilePath);
}

void CCurveWnd::OnCmdCurveOk()
{
	//ֱ�ӹر�
	::SendMessage(GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
}

void CCurveWnd::OnCmdCurveCancel()
{
	//�ָ�ͼ���˳�

	CDIP_SystemView* pView = GetActiveView();
	if (m_pImage != NULL)
	{
		memcpy(m_pBits, m_dataSrc, m_nlenData);
		pView->Invalidate();
	}
	::SendMessage(GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
}

void CCurveWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pImage == NULL) return;
	//�������������ж��û���Ϊ
	if (CursorIsInPaintRect(point))
	{
		RePlacePeg(point);//�ȿ����Ƿ��޸�peg
		m_bUserWillDrag = UserWillDrag(point);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CCurveWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pImage == NULL) return;
	if (CursorIsInPaintRect(point) && m_bUserWillDrag)
	{//����ϷŽ��������յ㲻��pegs����
		if (PegIsExist(point) == FALSE)
		{
			AddNewPeg(point);
		}
		ShowGrayTransform();
		ImshowImmediately();
		//����m_bUserWillDrag
		m_bUserWillDrag = FALSE;
	}

	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CCurveWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (CursorIsInPaintRect(point))
	{
		//��ʾ��ǰ�Ҷȱ任������/���
		CString xPos, yPos;
		CPoint AfterTransform = TransPositionToRectangle(point);
		xPos.Format(L"%d", int(AfterTransform.x/m_fWidthRatio));
		yPos.Format(L"%d", int(AfterTransform.y/m_fHeightRatio));
		SetDlgItemText(IDE_CURVEWND_XPOSITION, xPos);
		SetDlgItemText(IDE_CURVEWND_YPOSITION, yPos);
		if (m_pImage == NULL) return;
		if (m_bUserWillDrag)
		{
			//AddNewPeg������Ϊ���϶����ʱ�ܿ���Ч��
			POSITION pos = AddNewPeg(point);
			ShowGrayTransform();
			UpdateWindow();//����������,���ܿ�����ǰ�϶���peg
			//�������ͷ�ָ��ָ����ڴ棬�����ڴ�й¶
			if (pos != NULL)
			{
				delete m_pPegsList[m_nChannelSelected].GetAt(pos).m_pt;
				m_pPegsList[m_nChannelSelected].RemoveAt(pos);
			}
		}
	}
	else
	{
		SetDlgItemText(IDE_CURVEWND_XPOSITION, L"");
		SetDlgItemText(IDE_CURVEWND_YPOSITION, L"");
	}
	CWnd::OnMouseMove(nFlags, point);
}

//
// *** ������������ ***
//

VECTOR CCurveWnd::Spline(VECTOR& Node, VECTOR& Value, VECTOR& x0, double c1, double c2, int CASE)
{
	//CASE���߽�����������
	//c1���߽�����1
	//c2���߽�����2
	VECTOR u, Lamda, d, Mj, h, f1, s;
	double temp, f2;
	int i, j, n = Node.size(), sta = 0;
	for(j = 1; j<n; j++)
	{
		h.push_back(Node[j]-Node[j-1]);
		f1.push_back((Value[j]-Value[j-1])/h[j-1]);
	}
	u.push_back(0);
	Lamda.push_back(0);
	d.push_back(0);
	for(j = 1; j<n-1; j++)
	{
		u.push_back(h[j-1]/(h[j-1]+h[j]));
		Lamda.push_back(h[j]/(h[j-1]+h[j]));
		f2 = (f1[j]-f1[j-1])/(Node[j+1]-Node[j-1]);
		d.push_back(6*f2);
	}

	//�߽�����
	switch(CASE)
	{
	case 1:
		u.push_back(1);
		Lamda[0] = 1;
		d[0] = 6*(f1[0]-c1)/h[0];
		d.push_back(6*(c2-f1[n-2])/h[n-2]);
		break;
	case 2:
		u.push_back(0);
		Lamda[0] = 0;
		d[0] = 2*c1;
		d.push_back(2*c2);
		break;
	default:
		return VECTOR(0);
	}

	VECTOR b;
	for(i = 0; i<n; i++)
		b.push_back(2.0);
	u[0] = 0;
	Lamda.push_back(0);
	//׷�Ϸ����
	Chase(u, b, Lamda, d, Mj, n);
	for (i = 0; i<int(x0.size()); i++)
	{
		if(x0[i]<Node[0])
		{
			j = 0;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s.push_back(temp);
			continue;
		}
		if(x0[i]>Node[n-1])
		{
			j = n-2;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s.push_back(temp);
			continue;
		}
		for(j = 0; j<n-1; j++)
		{
			if(Node[j]<=x0[i] && x0[i]<=Node[j+1])
			{
				temp= Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
				s.push_back(temp);
				break;
			}
		}
	}

	u.~vector();
	Lamda.~vector();
	d.~vector();
	Mj.~vector();
	h.~vector();
	f1.~vector();
	b.~vector();
	return s;
}

//׷�Ϸ�
void CCurveWnd::Chase(VECTOR& a, VECTOR& b, VECTOR& c, VECTOR& f, VECTOR& x, int n)
{
	//a,b,c�����Խ���Ԫ��
	//x�����̵Ľ�
	//n���������
	int i;
	VECTOR belta;
	double tmp;
	//׷�Ĺ���
	belta.push_back(c[0]/b[0]);
	x.push_back(f[0]/b[0]);
	for(i = 1; i<n-1; i++)
	{
		tmp = b[i]-a[i]*belta[i-1];
		belta.push_back(c[i]/tmp);
		x.push_back((f[i]-a[i]*x[i-1])/tmp);
	}
	tmp = b[i]-a[i]*belta[i-1];
	x.push_back((f[i]-a[i]*x[i-1])/tmp);
	//�ϵĹ���
	for(i = n-2; i>=0; i--)
	{
		x[i] = x[i]-belta[i]*x[i+1];
	}
	belta.~vector();
}

//����������ͨ����ɫ,Ϊ��ʹ��ɫ��Ҳ��ʹ���������,WhereUse�˲�������Ϊ�˽�ʡ������
COLORREF CCurveWnd::ColorSetting(int Channel, int Strength, int WhereUse)
{
	//Channel:ͨ��
	//Strength:��ɫǿ��
	COLORREF LineColor;
	switch(Channel)
	{
	case 0:
		if (WhereUse)
		{
			Strength = 255 - Strength;
		}
		LineColor = RGB(255-Strength, 255-Strength, 255-Strength);
		break;
	case 1:
		LineColor = RGB(Strength, 0, 0);
		break;
	case 2:
		LineColor = RGB(0, Strength, 0);
		break;
	case 3:
		LineColor = RGB(0, 0, Strength);
		break;
	default:
		break;
	}
	return LineColor;
}

void CCurveWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (CursorIsInPaintRect(point))
	{
		if (PegIsExist(point) == TRUE)
		{
			delete m_pPegsList[m_nChannelSelected].GetAt(m_posCurrentPeg).m_pt;
			m_pPegsList[m_nChannelSelected].RemoveAt(m_posCurrentPeg);

			ShowGrayTransform();
			GrayTransform();
			ImshowImmediately();
		}
	}
	CWnd::OnRButtonUp(nFlags, point);
}

void CCurveWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(m_pWndSize);

	//��ͼ��Rect
	m_pOnPaintRect->right = m_pWndSize->right-70;
	m_pOnPaintRect->bottom = m_pWndSize->bottom-80;

	//���������255���ű�
	m_nRectWidth = m_pOnPaintRect->Width();
	m_nRectHeight = m_pOnPaintRect->Height();
	m_fWidthRatio = m_nRectWidth/255.0;
	m_fHeightRatio = m_nRectHeight/255.0;

	//�޸�peg������
	ReshapePegs();
	
	//�޸ĻҶȱ任����
	ReshapeTransform();

	int nButtonPosX1 = m_pWndSize->right-50;
	int nButtonPosX2 = nButtonPosX1+BUTTON_WIDTH;
	double Space = m_pWndSize->Height()/9.0;

	CWnd* hWnd;

	//Group Box
	hWnd = GetDlgItem(IDB_GROUP_BOX);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(20, 20, m_pWndSize->right-80, m_pWndSize->bottom-80);
	}
	//ѡ������
	ReShapeButtons(IDB_CHOOSE_CURVE, nButtonPosX1, nButtonPosX2, Space, 1);
	//ѡ��ֱ��
	ReShapeButtons(IDB_CHOOSE_LINE, nButtonPosX1, nButtonPosX2, Space, 2);
	//����
	ReShapeButtons(IDB_CURVEWND_RESET, nButtonPosX1, nButtonPosX2, Space, 3);
	//��ת
	ReShapeButtons(IDB_CURVEWND_INVERSE, nButtonPosX1, nButtonPosX2, Space, 4);
	//��������
	ReShapeButtons(IDB_IMPORT_CURVE, nButtonPosX1, nButtonPosX2, Space, 5);
	//��������
	ReShapeButtons(IDB_EXPORT_CURVE, nButtonPosX1, nButtonPosX2, Space, 6);
	//X����
	hWnd = GetDlgItem(IDE_CURVEWND_XPOSITION);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1+15, int(7*Space), BUTTON_WIDTH-15, BUTTON_HEIGHT);
	}
	//Y����
	hWnd = GetDlgItem(IDE_CURVEWND_YPOSITION);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1+15, int(8*Space), BUTTON_WIDTH-15, BUTTON_HEIGHT);
	}
	//X��ǩ
	hWnd = GetDlgItem(IDS_CURVEWND_XLABEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1, int(7*Space), BUTTON_WIDTH-25, BUTTON_HEIGHT);
	}
	//Y��ǩ
	hWnd = GetDlgItem(IDS_CURVEWND_YLABEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1, int(8*Space), BUTTON_WIDTH-25, BUTTON_HEIGHT);
	}

	//OK
	hWnd = GetDlgItem(IDB_CURVE_OK);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(40, m_pWndSize->bottom-40, BUTTON_WIDTH, BUTTON_HEIGHT);
	}
	//CANCEL
	hWnd = GetDlgItem(IDB_CURVE_CANCEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(m_pWndSize->right-70-BUTTON_WIDTH, m_pWndSize->bottom-40, BUTTON_WIDTH, BUTTON_HEIGHT);
	}
	if (m_pImage == NULL) return;
	//����m_pImage�ǿ�ʱ��ִ����һ��
	ShowGrayTransform();
}

//���ڱ仯ʱ��������ťλ��
void CCurveWnd::ReShapeButtons(int nItemID, int nButtonPosX1, int nButtonPosX2, double Space, int Number)
{
	CWnd *hWnd = GetDlgItem(nItemID);
	if (hWnd != NULL)
	{//MoveWindow:int x, int y, int nWidth, int nHeight
		hWnd->MoveWindow(nButtonPosX1, int(Number*Space-10), BUTTON_WIDTH, BUTTON_HEIGHT);
	}
}

//���ڱ仯ʱ��Pegs����ͬʱ�仯
void CCurveWnd::ReshapePegs()
{
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		//�����Rect���Ͻ�λ��
		Temp.m_pt->x = LONG(m_pOnPaintRect->left+Temp.m_dRatioX*m_nRectWidth);
		Temp.m_pt->y = LONG(m_pOnPaintRect->top+Temp.m_dRatioY*m_nRectHeight);
	}
}

//���ڱ仯ʱ����ŻҶȱ任��vectorҲ�仯
void CCurveWnd::ReshapeTransform()
{
	if (m_pImage == NULL) return;
	for (int i = 0; i<4; i++)
	{
		m_V4Transform[i].clear();
		for (int j = 0; j<m_nRectWidth+1; j++)
		{
			m_V4Transform[i].push_back(0);
		}
	}
}

BOOL CCurveWnd::OnEraseBkgnd(CDC* pDC)
{
	//ʹ�䲻Ҫ�ػ汳��
	return TRUE;
}
