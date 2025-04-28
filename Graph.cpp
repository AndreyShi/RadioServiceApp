
#include "stdafx.h"

void GraphHandler(CRadioServiceAppDlg* pDlgFrame){

	CPaintDC dc(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	// Очищаем фон
	dc.FillSolidRect(clientRect, RGB(255, 255, 255));

	// Область графика с отступами
	CRect graphRect = clientRect;
	graphRect.DeflateRect(40, 30, 30, 40);

	// Рисуем оси координат (Y теперь направлена вверх)
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&axisPen);

	// Ось X (горизонтальная)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.right, graphRect.bottom);

	// Ось Y (вертикальная, направлена ВВЕРХ)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.left, graphRect.top);

	// 2. Рисуем сетку
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200)); // Тонкие пунктирные линии
	dc.SelectObject(&gridPen);

	// Вертикальные линии сетки (параллельные оси Y)
	for (int x = graphRect.left; x <= graphRect.right; x += graphRect.Width() / 20) {
		dc.MoveTo(x, graphRect.bottom);
		dc.LineTo(x, graphRect.top);
	}

	// Горизонтальные линии сетки (параллельные оси X)
	for (int y = graphRect.bottom; y >= graphRect.top; y -= graphRect.Height() / 20) {
		dc.MoveTo(graphRect.left, y);
		dc.LineTo(graphRect.right, y);
	}

	// 3. Возвращаем перо для осей
	dc.SelectObject(&axisPen);

	// Пример данных
	std::vector<std::pair<double, double>> dataPoints = {
		{ 0, 5 }, { 1, 3 }, { 2, 7 }, { 3, 2 }, { 4, 8 }, { 5, 4 }
	};

	// Масштабирование с правильной ориентацией Y
	double minX = 0, maxX = 5;
	double minY = 0, maxY = 8;

	std::vector<CPoint> scaledPoints;
	for (const auto& point : dataPoints) {
		int x = graphRect.left + static_cast<int>(
			(point.first - minX) / (maxX - minX) * graphRect.Width());

		// Правильное вычисление Y (ось направлена вверх)
		int y = graphRect.bottom - static_cast<int>(
			(point.second - minY) / (maxY - minY) * graphRect.Height());

		scaledPoints.push_back(CPoint(x, y));
	}

	// Рисуем график
	CPen graphPen(PS_SOLID, 2, RGB(255, 0, 0));
	dc.SelectObject(&graphPen);

	if (!scaledPoints.empty()) {
		dc.MoveTo(scaledPoints[0]);
		for (size_t i = 1; i < scaledPoints.size(); ++i) {
			dc.LineTo(scaledPoints[i]);
		}
	}

	// Рисуем точки
	CBrush pointBrush(RGB(0, 0, 255));
	dc.SelectObject(&pointBrush);
	for (const auto& pt : scaledPoints) {
		dc.Ellipse(pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
	}

	// Подписи осей
	dc.SetBkMode(TRANSPARENT);
	dc.TextOutW(graphRect.left - 30, graphRect.top - 20, L"Y");
	dc.TextOutW(graphRect.right + 5, graphRect.bottom + 5, L"X");

	// Подписи значений (исправленные для правильной ориентации)
	for (int i = 0; i <= 5; i++) {
		// Ось X
		int xPos = graphRect.left + static_cast<int>(
			static_cast<double>(i) / 5 * graphRect.Width());
		CString xLabel;
		xLabel.Format(L"%d", i);
		dc.TextOutW(xPos - 10, graphRect.bottom + 10, xLabel);

		// Ось Y (значения увеличиваются снизу вверх)
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

	// Очищаем фон
	dc.FillSolidRect(clientRect, RGB(255, 255, 255));

	// Область графика с отступами
	CRect graphRect = clientRect;
	graphRect.DeflateRect(50, 40, 40, 50);

	// Рисуем оси координат
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&axisPen);

	// Ось X (горизонтальная)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.right, graphRect.bottom);

	// Ось Y (вертикальная)
	dc.MoveTo(graphRect.left, graphRect.bottom);
	dc.LineTo(graphRect.left, graphRect.top);

	// Параметры графика sin(x)
	const double xMin = 0;       // Начало по X
	const double xMax = 4 * 3.1416; // 4 периода (4*2пи примерно 4*3.1416)
	const double yMin = -1.2;    // Минимальное значение Y
	const double yMax = 1.2;     // Максимальное значение Y

	// Рисуем сетку
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
	dc.SelectObject(&gridPen);

	// Вертикальные линии сетки (каждый пи/2)
	for (double x = 0; x <= xMax; x += 3.1416 / 2) {
		int pixelX = graphRect.left + static_cast<int>(
			(x - xMin) / (xMax - xMin) * graphRect.Width());

		dc.MoveTo(pixelX, graphRect.bottom);
		dc.LineTo(pixelX, graphRect.top);

		// Подписи на оси X (пи, 2пи и т.д.)
		if (x > 0) {
			CString label;
			if (x == 3.1416) label = L"пи";
			else if (x == 2 * 3.1416) label = L"2пи";
			else if (x == 3 * 3.1416) label = L"3пи";
			else if (x == 4 * 3.1416) label = L"4пи";
			else label.Format(L"%.1f", x);

			dc.TextOutW(pixelX - 10, graphRect.bottom + 10, label);
		}
	}

	// Горизонтальные линии сетки
	for (double y = -1.0; y <= 1.0; y += 0.5) {
		int pixelY = graphRect.bottom - static_cast<int>(
			(y - yMin) / (yMax - yMin) * graphRect.Height());

		dc.MoveTo(graphRect.left, pixelY);
		dc.LineTo(graphRect.right, pixelY);

		// Подписи на оси Y
		CString label;
		label.Format(L"%.1f", y);
		dc.TextOutW(graphRect.left - 40, pixelY - 8, label);
	}

	// Рисуем график sin(x)
	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 255));
	dc.SelectObject(&graphPen);

	const int pointsCount = 200; // Количество точек для плавности
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

	// Подписи осей
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.TextOutW(graphRect.left - 20, graphRect.top - 20, L"sin(x)");
	dc.TextOutW(graphRect.right + 5, graphRect.bottom + 5, L"x");
	dc.TextOutW(graphRect.left - 30, graphRect.bottom + 5, L"0");
}

void GraphHandlerSinDoubleBuffer(CRadioServiceAppDlg* pDlgFrame){
	CPaintDC pDC(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	// 1. Создаем буфер в памяти
	CDC memDC;
	memDC.CreateCompatibleDC(&pDC);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&pDC, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

	// 2. Очищаем буфер
	memDC.FillSolidRect(clientRect, RGB(255, 255, 255));

	// Область графика с отступами
	CRect graphRect = clientRect;
	graphRect.DeflateRect(50, 40, 40, 50);

	// Рисуем оси координат
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectObject(&axisPen);

	// Ось X (горизонтальная)
	memDC.MoveTo(graphRect.left, graphRect.bottom);
	memDC.LineTo(graphRect.right, graphRect.bottom);

	// Ось Y (вертикальная)
	memDC.MoveTo(graphRect.left, graphRect.bottom);
	memDC.LineTo(graphRect.left, graphRect.top);

	// Параметры графика sin(x)
	const double xMin = 0;
	const double xMax = 14 * 3.1416;
	const double yMin = -1.2;
	const double yMax = 1.2;

	// Рисуем сетку
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
	memDC.SelectObject(&gridPen);

	// Вертикальные линии сетки
	for (double x = 0; x <= xMax; x += 3.1416 / 2) {
		int pixelX = graphRect.left + static_cast<int>(
			(x - xMin) / (xMax - xMin) * graphRect.Width());

		memDC.MoveTo(pixelX, graphRect.bottom);
		memDC.LineTo(pixelX, graphRect.top);

		// Подписи на оси X
		if (x > 0) {
			CString label;
			if (x == 3.1416) label = L"пи";
			else if (x == 2 * 3.1416) label = L"2пи";
			else if (x == 3 * 3.1416) label = L"3пи";
			else if (x == 4 * 3.1416) label = L"4пи";
			else label.Format(L"%.1f", x);

			memDC.TextOutW(pixelX - 10, graphRect.bottom + 10, label);
		}
	}

	// Горизонтальные линии сетки
	for (double y = -1.0; y <= 1.0; y += 0.5) {
		int pixelY = graphRect.bottom - static_cast<int>(
			(y - yMin) / (yMax - yMin) * graphRect.Height());

		memDC.MoveTo(graphRect.left, pixelY);
		memDC.LineTo(graphRect.right, pixelY);

		// Подписи на оси Y
		CString label;
		label.Format(L"%.1f", y);
		memDC.TextOutW(graphRect.left - 40, pixelY - 8, label);
	}

	// Рисуем график sin(x)
	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 255));
	memDC.SelectObject(&graphPen);

	const int pointsCount = 200;
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
			memDC.MoveTo(prevPoint.x, prevPoint.y);
			memDC.LineTo(pixelX, pixelY);
		}

		prevPoint = CPoint(pixelX, pixelY);
		firstPoint = false;
	}

	// Подписи осей
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(0, 0, 0));
	memDC.TextOutW(graphRect.left - 20, graphRect.top - 20, L"sin(x)");
	memDC.TextOutW(graphRect.right + 5, graphRect.bottom + 5, L"x");
	memDC.TextOutW(graphRect.left - 30, graphRect.bottom + 5, L"0");

	// 3. Копируем буфер на экран
	pDC.BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
		&memDC, 0, 0, SRCCOPY);

	// 4. Восстанавливаем старый битмап
	memDC.SelectObject(pOldBitmap);
}

void GraphHandlerSinDoubleBufferScroll(CRadioServiceAppDlg* pDlgFrame)
{
	CPaintDC pDC(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	// Создаем буфер в памяти
	CDC memDC;
	memDC.CreateCompatibleDC(&pDC);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&pDC, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

	// Очищаем буфер
	memDC.FillSolidRect(clientRect, RGB(255, 255, 255));

	// Учитываем масштаб и прокрутку
	double zoom = pDlgFrame->m_dZoomFactor;
	CPoint scrollPos = pDlgFrame->m_ScrollPos;

	// Виртуальные размеры графика
	int virtualWidth = static_cast<int>(4 * 3.1416 * 100 * zoom);
	int virtualHeight = static_cast<int>(600 * zoom);

	// Область графика
	CRect graphRect(-scrollPos.x, -scrollPos.y,
		virtualWidth - scrollPos.x,
		virtualHeight - scrollPos.y);

	// Рисуем оси координат
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectObject(&axisPen);

	// Ось X (с учетом прокрутки)
	int axisYPos = static_cast<int>(300 * zoom) - scrollPos.y; // Центр по Y
	memDC.MoveTo(graphRect.left, axisYPos);
	memDC.LineTo(graphRect.right, axisYPos);

	// Ось Y (с учетом прокрутки)
	int axisXPos = 50 - scrollPos.x; // Фиксированное смещение по X
	memDC.MoveTo(axisXPos, graphRect.top);
	memDC.LineTo(axisXPos, graphRect.bottom);

	// Рисуем график sin(x) с учетом масштаба и прокрутки
	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 255));
	memDC.SelectObject(&graphPen);

	const int pointsCount = 1000;
	CPoint prevPoint;
	bool firstPoint = true;

	for (int i = 0; i <= pointsCount; ++i) {
		double x = 4 * 3.1416 * i / pointsCount;
		double y = sin(x);

		int pixelX = static_cast<int>(x * 100 * zoom) - scrollPos.x;
		int pixelY = axisYPos - static_cast<int>(y * 100 * zoom);

		if (!firstPoint) {
			memDC.MoveTo(prevPoint.x, prevPoint.y);
			memDC.LineTo(pixelX, pixelY);
		}

		prevPoint = CPoint(pixelX, pixelY);
		firstPoint = false;
	}

	// Отображаем текущий масштаб
	CString status;
	status.Format(L"Масштаб: %.1fx | Прокрутка: (%d, %d)",
		zoom, scrollPos.x, scrollPos.y);
	memDC.TextOutW(10, 10, status);

	// Копируем буфер на экран
	pDC.BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
		&memDC, 0, 0, SRCCOPY);

	// Восстанавливаем ресурсы
	memDC.SelectObject(pOldBitmap);
}

void GraphHandler_fft(CRadioServiceAppDlg* pDlgFrame)
{
	CPaintDC pDC(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	if (pDlgFrame->m_frequencyData.empty()) {
		printf("m_frequencyData is empty, load 30_5.txt data\n");
		for (int i = 0; i < 512; i += 2){
			CRadioServiceAppDlg::FrequencyData data;
			data.frequency = fft30_5[i];
			data.amplitude = fft30_5[i + 1];
			pDlgFrame->m_frequencyData.push_back(data);
		}
	}

	// 1. Создаем буфер в памяти
	CDC memDC;
	memDC.CreateCompatibleDC(&pDC);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&pDC, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

	// 2. Очищаем буфер
	memDC.FillSolidRect(clientRect, RGB(255, 255, 255));

	// Область графика с отступами
	CRect graphRect = clientRect;
	graphRect.DeflateRect(50, 40, 40, 50);

	// Находим минимальные и максимальные значения
	double minFreq = pDlgFrame->m_frequencyData.front().frequency;
	double maxFreq = pDlgFrame->m_frequencyData.back().frequency;
	double minAmp = pDlgFrame->m_frequencyData[0].amplitude;
	double maxAmp = pDlgFrame->m_frequencyData[0].amplitude;

	for (const auto& data : pDlgFrame->m_frequencyData) {
		if (data.amplitude < minAmp) minAmp = data.amplitude;
		if (data.amplitude > maxAmp) maxAmp = data.amplitude;
	}

	// Добавляем немного места по краям
	minAmp -= 5;
	maxAmp += 5;

	// Рисуем оси координат
	CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectObject(&axisPen);

	// Ось X (горизонтальная)
	memDC.MoveTo(graphRect.left, graphRect.bottom);
	memDC.LineTo(graphRect.right, graphRect.bottom);

	// Ось Y (вертикальная)
	memDC.MoveTo(graphRect.left, graphRect.bottom);
	memDC.LineTo(graphRect.left, graphRect.top);

	// Рисуем сетку
	CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
	memDC.SelectObject(&gridPen);

	// Вертикальные линии сетки (каждые 0.1 МГц)
	for (double freq = minFreq; freq <= maxFreq; freq += 0.1) {
		int pixelX = graphRect.left + static_cast<int>(
			(freq - minFreq) / (maxFreq - minFreq) * graphRect.Width());

		// Проверяем, чтобы линия не выходила за границы
		if (pixelX < graphRect.left || pixelX > graphRect.right) continue;

		memDC.MoveTo(pixelX, graphRect.bottom);
		memDC.LineTo(pixelX, graphRect.top);

		// Подписи на оси X (каждые 0.5 МГц)
		if (fmod(freq, 0.5) < 0.01) {
			CString label;
			label.Format(L"%.1f", freq);
			memDC.TextOutW(pixelX - 15, graphRect.bottom + 5, label);
		}
	}

	// Горизонтальные линии сетки (каждые 10 dBm)
	for (double amp = floor(minAmp / 10) * 10; amp <= maxAmp; amp += 10) {
		int pixelY = graphRect.bottom - static_cast<int>(
			(amp - minAmp) / (maxAmp - minAmp) * graphRect.Height());

		// Проверяем, чтобы линия не выходила за границы
		if (pixelY < graphRect.top || pixelY > graphRect.bottom) continue;

		memDC.MoveTo(graphRect.left, pixelY);
		memDC.LineTo(graphRect.right, pixelY);

		// Подписи на оси Y
		CString label;
		label.Format(L"%.0f", amp);
		memDC.TextOutW(graphRect.left - 45, pixelY - 8, label);
	}

	// Рисуем график
	CPen graphPen(PS_SOLID, 2, RGB(0, 0, 255));
	memDC.SelectObject(&graphPen);

	CPoint prevPoint;
	bool firstPoint = true;

	for (const auto& data : pDlgFrame->m_frequencyData) {
		int pixelX = graphRect.left + static_cast<int>(
			(data.frequency - minFreq) / (maxFreq - minFreq) * graphRect.Width());

		int pixelY = graphRect.bottom - static_cast<int>(
			(data.amplitude - minAmp) / (maxAmp - minAmp) * graphRect.Height());

		// Проверяем, чтобы точка не выходила за границы
		pixelX = max(graphRect.left, min(graphRect.right, pixelX));
		pixelY = max(graphRect.top, min(graphRect.bottom, pixelY));

		if (!firstPoint) {
			memDC.MoveTo(prevPoint.x, prevPoint.y);
			memDC.LineTo(pixelX, pixelY);
		}

		prevPoint = CPoint(pixelX, pixelY);
		firstPoint = false;
	}

	// Подписи осей
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(0, 0, 0));
	memDC.TextOutW(graphRect.left - 35, graphRect.top - 30, L"Амплитуда dBm");
	memDC.TextOutW(graphRect.right - 55, graphRect.bottom + 20, L"Частота (МГц)");

	// 3. Копируем буфер на экран
	pDC.BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
		&memDC, 0, 0, SRCCOPY);

	// 4. Восстанавливаем старый битмап
	memDC.SelectObject(pOldBitmap);
}