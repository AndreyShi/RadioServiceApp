
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

void GraphHandler_fft_zoom_mouse(CRadioServiceAppDlg* pDlgFrame)
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

	//изменяемое кол -во отображаемых значение по x
	int div = 5;
	int res = (int)pDlgFrame->m_visibleMaxFreq * 10 - (int)pDlgFrame->m_visibleMinFreq * 10;
	if (res > 850)
	    { div = 50;}
	else if (res > 650)
	    { div = 40;}
	else if (res > 450)
	    { div = 30;}
	else if (res > 200)
	    { div = 20;}
	else if (res > 100)
	    { div = 10;}

	//printf("res: %d  div: %d\n", res, div);

	// Вертикальные линии сетки (каждые 0.1 МГц)
	for (double freq = floor(pDlgFrame->m_visibleMinFreq * 10) / 10; freq <= ceil(pDlgFrame->m_visibleMaxFreq) + 0.1F; freq += 0.1) {
		int pixelX = graphRect.left + static_cast<int>(
			(freq - pDlgFrame->m_visibleMinFreq) / (pDlgFrame->m_visibleMaxFreq - pDlgFrame->m_visibleMinFreq) * graphRect.Width());

		// Подписи на оси X (каждые 0.5 МГц)
		if (int(freq * 10) % div == 0){
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
	std::vector<CPoint> graphPoints; // Сохраняем все точки графика для проверки пересечения

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

		graphPoints.push_back(CPoint(pixelX, pixelY));
		prevPoint = CPoint(pixelX, pixelY);
		firstPoint = false;
	}

	// Проверяем позицию курсора мыши
	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	pDlgFrame->ScreenToClient(&cursorPos);

	// Проверяем, находится ли курсор в области графика
	if (graphRect.PtInRect(cursorPos)) {
		// Преобразуем позицию курсора в частоту
		double cursorFreq = pDlgFrame->m_visibleMinFreq +
			(cursorPos.x - graphRect.left) *
			(pDlgFrame->m_visibleMaxFreq - pDlgFrame->m_visibleMinFreq) /
			graphRect.Width();

		// Ищем ближайшую точку данных в видимом диапазоне
		double minDistance = DBL_MAX;
		double closestFreq = 0;
		double closestAmp = 0;
		CPoint closestPoint;

		for (const auto& data : pDlgFrame->m_frequencyData) {
			if (data.frequency >= pDlgFrame->m_visibleMinFreq &&
				data.frequency <= pDlgFrame->m_visibleMaxFreq) {

				// Вычисляем позицию точки на графике
				int pixelX = graphRect.left + static_cast<int>(
					(data.frequency - pDlgFrame->m_visibleMinFreq) /
					(pDlgFrame->m_visibleMaxFreq - pDlgFrame->m_visibleMinFreq) *
					graphRect.Width());

				int pixelY = graphRect.bottom - static_cast<int>(
					(data.amplitude - minAmp) / (maxAmp - minAmp) * graphRect.Height());

				// Проверяем расстояние до курсора только по оси X
				int distance = abs(pixelX - cursorPos.x);
				if (distance < minDistance) {
					minDistance = distance;
					closestFreq = data.frequency;
					closestAmp = data.amplitude;
					closestPoint = CPoint(pixelX, pixelY);
				}
			}
		}

		// Если курсор достаточно близко к линии графика
		if (minDistance < 20) {
			// Рисуем маркер на точке
			CPen markerPen(PS_SOLID, 2, RGB(255, 0, 0));
			memDC.SelectObject(&markerPen);
			memDC.MoveTo(closestPoint.x - 5, closestPoint.y - 5);
			memDC.LineTo(closestPoint.x + 5, closestPoint.y + 5);
			memDC.MoveTo(closestPoint.x - 5, closestPoint.y + 5);
			memDC.LineTo(closestPoint.x + 5, closestPoint.y - 5);

			// Отображаем координаты
			CString coordText;
			coordText.Format(L"Частота: %.3f МГц\nАмплитуда: %.1f dBm", closestFreq, closestAmp);

			CRect textRect(closestPoint.x + 10, closestPoint.y - 30,
				closestPoint.x + 200, closestPoint.y + 10);

			// Проверяем, чтобы текст не выходил за границы экрана
			if (textRect.right > clientRect.right) {
				textRect.OffsetRect(-(textRect.right - clientRect.right) - 20, 0);
			}
			if (textRect.top < clientRect.top) {
				textRect.OffsetRect(0, -(textRect.top - clientRect.top));
			}

			memDC.SetBkMode(OPAQUE);
			memDC.SetBkColor(RGB(255, 255, 200));
			memDC.SetTextColor(RGB(0, 0, 0));
			memDC.DrawText(coordText, textRect, DT_LEFT | DT_TOP | DT_WORDBREAK);
		}
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