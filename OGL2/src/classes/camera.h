/* Камера : ====================================================================================\\
||	Класс камеры
\\==============================================================================================*/
#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include "./globals.h"
#include "./input.h"
#include "./objects.h"

class Camera
{
public:
	Point3d		Position	;								// Позиция камеры в мировых координатах
	Vector3d	Direction	;								// Направление куда смотрит камера (используется редко)
	Vector3d	Strafe		;								// Вектор стрейфа (влево)
	Vector3d	Angles		;								// Нбор углов для определения куда смотрит камера
	/*===[ Углы : ]============================================(Это проще чем вектор направления)===\\
	||	Угол X - определяет повороты вверх|вниз (вокруг оси OX)
	||	Угол Y - определяет поворот право|влево (вокруг оси OY)
	||	Угол Z - не используется ( крен влево-вправо вокруг оси OZ)
	\\==============================================================================================*/
	Point2i		center;										// Центр экрана


	Camera			();								// Constructor
	~Camera			();								// Destructor

	Vector3d			GetDirection	(void);				// Нахождение направления обзора
	Vector3d			GetStrafeLeft	(void);				// Нахождение вектора стрейфа (влево)
	Point3d				GetPosition		(void);				// Нахлждение позиции камеры (в мировых координатах)

	void Init			(void);								// Инициализаци
	void SetPosition	(Point3d Pos);						// Инициализация позиции камеры
//	void LookTo			(Point3f VAR, char MODE);				// Задание направления взгляда
							//		MODE:
						// CAM_LOOKTO_MODE_ANGLES	-	Данные в виде углов
						// CAM_LOOKTO_MODE_POINT	-	Данные в виде точки (куда глядеть)
						// CAM_LOOKTO_MODE_VECTOR	-	Данные в виде вектора направления
	void Move			(char MODE, double speed);			// Перемещение камеры
	void Look			(void);								// Применение матрицы к OpenGL
	void MouseLook		(void);								// Отслеживание перемещений мышы
    void KeybMove		(void);								// Отслеживание нажатий клавиш => перемещение камеры

// FJ Input functions
		// Эти функции надо вызывать после поворотов камеры (после перемещения они не нужны)
//#ifdef FJ_INPUT_ENABLED
	void RecalculateDirection	(void);						// Пересчёт вектора вперёд
	void RecalculateStrafe		(void);						// Пересчёт вектора направленного влево

	void MoveModelViewMatrix	(void);						// Перемещение системы координат для рендеринга
	void RotateModelViewMatrix	(void);						// Поворот системы координат
//#endif
};
