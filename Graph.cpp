
#include "stdafx.h"

void GraphHandler(CRadioServiceAppDlg* pDlgFrame){

	CPaintDC dc(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	// ������� ���
	dc.FillSolidRect(clientRect, RGB(255, 255, 255));

	// ������� ������� � ���������
	CRect graphRect = clientRect;
	graphRect.DeflateRect(40, 30, 30, 40);

	// ������ ��� ��������� (Y ������ ���������� �����)
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&axisPen);

	// ��� X (��������������)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.right, graphRect.bottom);

	// ��� Y (������������, ���������� �����)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.left, graphRect.top);

	// 2. ������ �����
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200)); // ������ ���������� �����
	dc.SelectObject(&gridPen);

	// ������������ ����� ����� (������������ ��� Y)
	for (int x = graphRect.left; x <= graphRect.right; x += graphRect.Width() / 20) {
		dc.MoveTo(x, graphRect.bottom);
		dc.LineTo(x, graphRect.top);
	}

	// �������������� ����� ����� (������������ ��� X)
	for (int y = graphRect.bottom; y >= graphRect.top; y -= graphRect.Height() / 20) {
		dc.MoveTo(graphRect.left, y);
		dc.LineTo(graphRect.right, y);
	}

	// 3. ���������� ���� ��� ����
	dc.SelectObject(&axisPen);

	// ������ ������
	std::vector<std::pair<double, double>> dataPoints = {
		{ 0, 5 }, { 1, 3 }, { 2, 7 }, { 3, 2 }, { 4, 8 }, { 5, 4 }
	};

	// ��������������� � ���������� ����������� Y
	double minX = 0, maxX = 5;
	double minY = 0, maxY = 8;

	std::vector<CPoint> scaledPoints;
	for (const auto& point : dataPoints) {
		int x = graphRect.left + static_cast<int>(
			(point.first - minX) / (maxX - minX) * graphRect.Width());

		// ���������� ���������� Y (��� ���������� �����)
		int y = graphRect.bottom - static_cast<int>(
			(point.second - minY) / (maxY - minY) * graphRect.Height());

		scaledPoints.push_back(CPoint(x, y));
	}

	// ������ ������
	CPen graphPen(PS_SOLID, 2, RGB(255, 0, 0));
	dc.SelectObject(&graphPen);

	if (!scaledPoints.empty()) {
		dc.MoveTo(scaledPoints[0]);
		for (size_t i = 1; i < scaledPoints.size(); ++i) {
			dc.LineTo(scaledPoints[i]);
		}
	}

	// ������ �����
	CBrush pointBrush(RGB(0, 0, 255));
	dc.SelectObject(&pointBrush);
	for (const auto& pt : scaledPoints) {
		dc.Ellipse(pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
	}

	// ������� ����
	dc.SetBkMode(TRANSPARENT);
	dc.TextOutW(graphRect.left - 30, graphRect.top - 20, L"Y");
	dc.TextOutW(graphRect.right + 5, graphRect.bottom + 5, L"X");

	// ������� �������� (������������ ��� ���������� ����������)
	for (int i = 0; i <= 5; i++) {
		// ��� X
		int xPos = graphRect.left + static_cast<int>(
			static_cast<double>(i) / 5 * graphRect.Width());
		CString xLabel;
		xLabel.Format(L"%d", i);
		dc.TextOutW(xPos - 10, graphRect.bottom + 10, xLabel);

		// ��� Y (�������� ������������� ����� �����)
		int yPos = graphRect.bottom - static_cast<int>(
			static_cast<double>(i) / 8 * graphRect.Height());
		CString yLabel;
		yLabel.Format(L"%d", i);
		dc.TextOutW(graphRect.left - 30, yPos - 8, yLabel);
	}
}

void GraphHandlerSin(CRadioServiceAppDlg* pDlgFrame){
	CPaintDC dc(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	// ������� ���
	dc.FillSolidRect(clientRect, RGB(255, 255, 255));

	// ������� ������� � ���������
	CRect graphRect = clientRect;
	graphRect.DeflateRect(50, 40, 40, 50);

	// ������ ��� ���������
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&axisPen);

	// ��� X (��������������)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.right, graphRect.bottom);

	// ��� Y (������������)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.left, graphRect.top);

	// ��������� ������� sin(x)
	const double xMin = 0;       // ������ �� X
	const double xMax = 4 * 3.1416; // 4 ������� (4*2�� �������� 4*3.1416)
	const double yMin = -1.2;    // ����������� �������� Y
	const double yMax = 1.2;     // ������������ �������� Y

	// ������ �����
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
	dc.SelectObject(&gridPen);

	// ������������ ����� ����� (������ ��/2)
	for (double x = 0; x <= xMax; x += 3.1416 / 2) {
		int pixelX = graphRect.left + static_cast<int>(
			(x - xMin) / (xMax - xMin) * graphRect.Width());

		dc.MoveTo(pixelX, graphRect.bottom);
		dc.LineTo(pixelX, graphRect.top);

		// ������� �� ��� X (��, 2�� � �.�.)
		if (x > 0) {
			CString label;
			if (x == 3.1416) label = L"��";
			else if (x == 2 * 3.1416) label = L"2��";
			else if (x == 3 * 3.1416) label = L"3��";
			else if (x == 4 * 3.1416) label = L"4��";
			else label.Format(L"%.1f", x);

			dc.TextOutW(pixelX - 10, graphRect.bottom + 10, label);
		}
	}

	// �������������� ����� �����
	for (double y = -1.0; y <= 1.0; y += 0.5) {
		int pixelY = graphRect.bottom - static_cast<int>(
			(y - yMin) / (yMax - yMin) * graphRect.Height());

		dc.MoveTo(graphRect.left, pixelY);
		dc.LineTo(graphRect.right, pixelY);

		// ������� �� ��� Y
		CString label;
		label.Format(L"%.1f", y);
		dc.TextOutW(graphRect.left - 40, pixelY - 8, label);
	}

	// ������ ������ sin(x)
	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 255));
	dc.SelectObject(&graphPen);

	const int pointsCount = 200; // ���������� ����� ��� ���������
	CPoint prevPoint;
	bool firstPoint = true;

	for (int i = 0; i <= pointsCount; ++i) {
		double x = xMin + i * (xMax - xMin) / pointsCount;
		double y = sin(x);

		int pixelX = graphRect.left + static_cast<int>(
			(x - xMin) / (xMax - xMin) * graphRect.Width());

		int pixelY = graphRect.bottom - static_cast<int>(
			(y - yMin) / (yMax - yMin) * graphRect.Height());

		if (!firstPoint) {
			dc.MoveTo(prevPoint.x, prevPoint.y);
			dc.LineTo(pixelX, pixelY);
		}

		prevPoint = CPoint(pixelX, pixelY);
		firstPoint = false;
	}

	// ������� ����
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.TextOutW(graphRect.left - 20, graphRect.top - 20, L"sin(x)");
	dc.TextOutW(graphRect.right + 5, graphRect.bottom + 5, L"x");
	dc.TextOutW(graphRect.left - 30, graphRect.bottom + 5, L"0");
}