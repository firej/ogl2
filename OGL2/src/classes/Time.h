#pragma once
//--------------------------------------------------------------------------------------//
//  Created with help of	Yuan, Feng								www.fengyuan.com	//
//							(Windows Graphics Programming: Win32 GDI and DirectDraw)	//
//--------------------------------------------------------------------------------------//

#pragma warning(disable : 4035)
#include "./globals.h"

inline unsigned __int64 GetCycleCount(void)
{
    _asm    _emit 0x0F
    _asm    _emit 0x31
}

class Timer
{
protected:
	unsigned __int64	last_cycle;					// Последний отслеженый цикл
	unsigned __int64	now_cycle;					// "Сейчасный" отслеженый цикл
	double				q;							// Коэффициент умножения

    double				FrameTime;					// Время между последними кадрами
	double				GFrameTime;					// Game frame time - время для игровых объектов (+пауза)
	double				WorldTime;					// Общее игровое время
	double				TimeToShowFPS;				// Время прошедшее с последнего изменения значения FPS
	double				FPS;						// Кол-во кадров в секунду
	int					FramesToRefresh;			// Количество фреймов между обновлениями
			// Программа должна подстраивать это значение чтобы FPS'ы обновлялись примерно 2 раза в секунду
	int					i;							// Подсчёт фреймов для FPS
	bool				pause;						// Указывает на то что в мире царит ПАУЗА!

public:
    Timer()							  
	{									   
		last_cycle = 0;
		now_cycle = 0;
		FramesToRefresh = 50;
		i = 0;
	}
	void Start()
	{
		q = ((double)1.0) / ((double)(SysInfo.cpu.speed) * ((double)1000000.0));
		last_cycle = now_cycle = GetCycleCount();
		WorldTime = 0;
		pause = false;
	}
    void NewFrame	(void)								// Начало нового кадра
	{
		now_cycle = GetCycleCount();
		FrameTime = (double)(now_cycle - last_cycle) * q;
		if (!pause) GFrameTime = FrameTime;
					else GFrameTime = 0;
		last_cycle = now_cycle;
		WorldTime		+= FrameTime;
			i++;
			TimeToShowFPS	+= FrameTime;
			FramesToRefresh = (int)(0.25/FrameTime);
			if (i >= FramesToRefresh)
			{
				FPS = FramesToRefresh/TimeToShowFPS;
				i = 0;	TimeToShowFPS = 0;
			}
	};
	void	SetFramesToRefresh(const int Frames)
	{	FramesToRefresh = Frames;
	};
	double GetFrameTime		(void)
	{	return FrameTime;
	};
	double GetGameFrameTime	(void)
	{	return GFrameTime;
	};
	double GetFPS	(void)
	{	return FPS;
	};
	double GetWorldTime()
	{	return WorldTime;
	};
	int GetFramesToRefresh()
	{	return FramesToRefresh;
	};
};

extern Timer GT;	// Global Timer