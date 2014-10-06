#include "LocusAFX.h"
#include "./camera.h"
#include "./time.h"
#include "./sound/sound.h"


//Конструктор по умолчанию - просто обнуляет позицию и направление взгляда

CameraSet	CamS;
SkyBox		MSB;
Camera*		GCam;

Camera::Camera(void)
{
	memset(&Position,	0,	sizeof(Position)	);
//	memset(&Direction,	0,	sizeof(Vector3f));
	memset(&Angles	,	0,	sizeof(Angles));
}
//Деструктор камеры
Camera::~Camera(void)
{
}

void Camera::Init	(void)
{
	center.d.c.x = Globals.VP.Width  >> 1;					// Находим середину длины
	center.d.c.y = Globals.VP.Height >> 1;					// Находим серелину высоты
	SetCursorPos(center.d.c.x, center.d.c.y);				// Установка указателя в "спокойное" состояние
	Position = CamS.CamStartPosition;						// Стартовая позиция камеры
}

void Camera::Look	(void)
{
	GetDirection();
	GetStrafeLeft();
	glLoadIdentity();
	MouseLook();
	glRotated(Angles.d.c.x,		1.0f,	0.0f,	0.0f);
	glRotated(360-Angles.d.c.y,	0.0f,	1.0f,	0.0f);
	glRotated(Angles.d.c.z,		0.0f,	0.0f,	1.0f);

	MSB();											// Отрисовка неба

	glTranslated(	Position.d.c.x,
					Position.d.c.y,
					Position.d.c.z);// Перемещает камеру в точку Position

	Sound.SetLPosition(Point3f((float)Position.d.c.x,(float)Position.d.c.y,(float)Position.d.c.z));
	Sound.SetLOrientation(Vector3f((float)Direction.d.c.x,(float)Direction.d.c.y,(float)Direction.d.c.z), Vector3f(0.0f,1.0f,0.0f));
	
}

void Camera::Move(char MODE,double speed)
{
	static double realspeed;
	realspeed = speed * GT.GetFrameTime();
	switch (MODE)
	{
	case CAM_MOVE_FORWARD	:
		Position	+=	Direction*realspeed;
		break;
	case CAM_MOVE_BACKWARD	:
		Position	-=	Direction*realspeed;
		break;
	case CAM_MOVE_LEFT		:
		Position	+=	Strafe*realspeed;
		break;
	case CAM_MOVE_RIGHT		:
		Position	-=	Strafe*realspeed;
		break;
	}
}

void Camera::MouseLook(void)
{
	static POINT		mousePos;			// This is a window structure that holds an X and Y
	GetCursorPos(&mousePos);								// Get the mouse's current X,Y position
	
	// If our cursor is still in the middle, we never moved... so don't update the screen
	if( (mousePos.x == center.d.c.x) && (mousePos.y == center.d.c.y) ) return;

	// Set the mouse position to the middle of our window
	SetCursorPos(center.d.c.x, center.d.c.y);							

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	Angles.d.c.x	+= (float) CamS.Sensitivity*((mousePos.y - center.d.c.y)  / 25.0f);		
	Angles.d.c.y	-= (float) CamS.Sensitivity*((mousePos.x - center.d.c.x)  / 25.0f);
	Angles.d.c.z	= 0;
	Angles.d.c.y	=	fmod(Angles.d.c.y,360.0);
	if (Angles.d.c.x > 90.0f)		Angles.d.c.x =  90.0f;
	else if (Angles.d.c.x < -90)	Angles.d.c.x = -90.0f;
}
Vector3d Camera::GetDirection	(void)
{
	Direction.d.c.x	=	+ cost(Angles.d.c.x)*sint(Angles.d.c.y);	/*cost((const short int)Angles.y);*/
	Direction.d.c.y	=	+ sint(Angles.d.c.x);					/*sint((const short int)Angles.x);*/
	Direction.d.c.z	=	+ cost(Angles.d.c.x)*cost(Angles.d.c.y);	/*sint((const short int)Angles.y);*/
	Direction.normalize();
	return Direction;
}
Vector3d Camera::GetStrafeLeft	(void)
{
	Strafe.d.c.x	=	+ sint(Angles.d.c.y + 90);
	Strafe.d.c.y	=	0;
	Strafe.d.c.z	=	+ cost(Angles.d.c.y + 90);
	Strafe.normalize();
	return Strafe;
}
Point3d Camera::GetPosition	(void)
{
	return Camera::Position;
}
void Camera::SetPosition(Point3d Pos)
{
	Position = Pos;
}

void Camera::KeybMove		(void)
{
	static double speed;
	if (Input::I->get(KeySet.MoreFastMove))		speed = 3*CamS.BackwardSpeed;
		else speed = CamS.BackwardSpeed;
	if (Input::I->get(KeySet.CamMoveForward))
	{
		if (Input::I->get(KeySet.MoreFastMove))	speed = 3*CamS.ForwardSpeed;
			else speed = CamS.ForwardSpeed;
		Move(CAM_MOVE_FORWARD,speed);
	}
	if (Input::I->get(KeySet.CamMoveBackward))
	{
		if (Input::I->get(KeySet.MoreFastMove))	speed = 3*CamS.BackwardSpeed;
			else speed = CamS.BackwardSpeed;
		Move(CAM_MOVE_BACKWARD,speed);
	}
	if (Input::I->get(KeySet.CamMoveLeft))
	{
		if (Input::I->get(KeySet.MoreFastMove))	speed = 3*CamS.StrafeSpeed;
			else speed = CamS.StrafeSpeed;
		Move(CAM_MOVE_LEFT,speed);
	}
	if (Input::I->get(KeySet.CamMoveRight))
	{
		if (Input::I->get(KeySet.MoreFastMove))	speed = 3*CamS.StrafeSpeed;
			else speed = CamS.StrafeSpeed;
		Move(CAM_MOVE_RIGHT,speed);
	}
	if (Input::I->get(KeySet.CamBackToCenter))
		Camera::SetPosition( CamS.CamStartPosition);
}

// FJ_INPUT интерфейс камеры

void Camera::RecalculateDirection	(void)						// Пересчёт вектора вперёд
{	Direction.d.c.x	=	+ cost(Angles.d.c.x)*sint(Angles.d.c.y);	/*cost((const short int)Angles.y);*/
	Direction.d.c.y	=	+ sint(Angles.d.c.x);					/*sint((const short int)Angles.x);*/
	Direction.d.c.z	=	+ cost(Angles.d.c.x)*cost(Angles.d.c.y);	/*sint((const short int)Angles.y);*/
	Direction.normalize();
}
void Camera::RecalculateStrafe		(void)						// Пересчёт вектора направленного влево
{	Strafe.d.c.x	=	+ sint(Angles.d.c.y + 90);
	Strafe.d.c.y	=	0;
	Strafe.d.c.z	=	+ cost(Angles.d.c.y + 90);
	Strafe.normalize();
}
void Camera::MoveModelViewMatrix	(void)						// Перемещение системы координат для рендеринга
{	glTranslated(	Position.d.c.x,
					Position.d.c.y,
					Position.d.c.z);// Перемещает камеру в точку Position
}
void Camera::RotateModelViewMatrix	(void)						// Поворот системы координат
{	glRotated(Angles.d.c.x		,	1.0f,	0.0f,	0.0f);
	glRotated(360-Angles.d.c.y	,	0.0f,	1.0f,	0.0f);
	glRotated(Angles.d.c.z		,	0.0f,	0.0f,	1.0f);
}