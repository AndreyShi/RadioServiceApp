
#include "stdafx.h"

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
	for (double freq = minFreq; freq <= ceil(maxFreq) + 0.1F; freq += 0.1) {
		int pixelX = graphRect.left + static_cast<int>(
			(freq - minFreq) / (maxFreq - minFreq) * graphRect.Width());

		// Подписи на оси X (каждые 0.5 МГц)
		if (fmod(freq, 0.5) < 0.01) {
			CString label;
			label.Format(L"%.1f", freq);
			memDC.TextOutW(pixelX - 15, graphRect.bottom + 5, label);
		}

		// Проверяем, чтобы линия не выходила за границы
		if (pixelX <= graphRect.left || pixelX > graphRect.right) continue;

		memDC.MoveTo(pixelX, graphRect.bottom);
		memDC.LineTo(pixelX, graphRect.top);
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

void GraphHandler_fft_zoom(CRadioServiceAppDlg* pDlgFrame)
{
	CPaintDC pDC(pDlgFrame);
	CRect clientRect;
	pDlgFrame->GetClientRect(&clientRect);

	if (pDlgFrame->m_frequencyData.empty()) {
		printf("m_frequencyData is empty, load 30_5.txt data\n");
		for (int i = 0; i < 512; i += 2) {
			CRadioServiceAppDlg::FrequencyData data;
			data.frequency = fft30_5[i];
			data.amplitude = fft30_5[i + 1];
			pDlgFrame->m_frequencyData.push_back(data);
		}

		// Инициализация начального диапазона
		pDlgFrame->m_visibleMinFreq = pDlgFrame->m_frequencyData.front().frequency;
		pDlgFrame->m_visibleMaxFreq = pDlgFrame->m_frequencyData.back().frequency;
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

	// Находим минимальные и максимальные значения амплитуды в видимом диапазоне
	double minAmp = pDlgFrame->m_frequencyData[0].amplitude;
	double maxAmp = pDlgFrame->m_frequencyData[0].amplitude;

	for (const auto& data : pDlgFrame->m_frequencyData) {
		if (data.frequency >= pDlgFrame->m_visibleMinFreq && data.frequency <= pDlgFrame->m_visibleMaxFreq) {
			if (data.amplitude < minAmp) minAmp = data.amplitude;
			if (data.amplitude > maxAmp) maxAmp = data.amplitude;
		}
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
	for (double freq = floor(pDlgFrame->m_visibleMinFreq * 10) / 10; freq <= ceil(pDlgFrame->m_visibleMaxFreq) + 0.1F; freq += 0.1) {
		int pixelX = graphRect.left + static_cast<int>(
			(freq - pDlgFrame->m_visibleMinFreq) / (pDlgFrame->m_visibleMaxFreq - pDlgFrame->m_visibleMinFreq) * graphRect.Width());

		// Подписи на оси X (каждые 0.5 МГц)
		if (fmod(freq, 0.5) < 0.01) {
			CString label;
			label.Format(L"%.1f", freq);
			memDC.TextOutW(pixelX - 15, graphRect.bottom + 5, label);
		}

		// Проверяем, чтобы линия не выходила за границы
		if (pixelX <= graphRect.left || pixelX > graphRect.right) continue;

		memDC.MoveTo(pixelX, graphRect.bottom);
		memDC.LineTo(pixelX, graphRect.top);
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
		if (data.frequency < pDlgFrame->m_visibleMinFreq || data.frequency > pDlgFrame->m_visibleMaxFreq)
			continue;

		int pixelX = graphRect.left + static_cast<int>(
			(data.frequency - pDlgFrame->m_visibleMinFreq) / (pDlgFrame->m_visibleMaxFreq - pDlgFrame->m_visibleMinFreq) * graphRect.Width());

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