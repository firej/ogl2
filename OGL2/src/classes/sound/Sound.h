#pragma once

class SoundController;								// Управление звуком
class soundSource;									// Источник звука
class SoundBuf;										// Звуковой буфер

//=========================================================================================================
//====[ Основное управление ]==============================================================================
//=========================================================================================================
class SoundController
{
protected:
	ALCdevice	*ALDevice;
	ALCcontext	*ALContext;
	ALboolean	inSuspend;

	ALenum		ALerror;
// Listener properties
	Point3f		LPosition;							// Позиция ушей
	Vector3f	LVelocity;							// Скорость ушей
	Vector3f	LDirection;							// Напрвление взгляда ушастого
	Vector3f	LUpVector;							// Куда напрвлена макушка ушастого
public:
	SoundController();
	~SoundController();
	ALboolean	Init			();					// создание контекста рендеринга для AL
	ALboolean	Suspend			();					// Пауза общая для всего контекста (полная остановка обработки)
	ALboolean	Process			();					// Продолжение проигрывания
	ALboolean	DeInit			();					// удаление контекста рендеринга для AL

	ALvoid	SetDistModel	( ALenum value );	// Установка модели учёта расстояния

	ALvoid	SetLPosition	(Point3f	Pos);	// Установка позиции ушей
	ALvoid	SetLVelocity	(Vector3f	Vel);	// Установка скорости ушей
	ALvoid	SetLOrientation	(Vector3f	Direction,// Установка ориентации ушей
							 Vector3f	UpVector);
	ALvoid	SetMasterGain	(ALfloat	MasterGain);// Установка параметра GAIN
};
extern SoundController Sound;
//=========================================================================================================
//====[ Управление файлами ]===============================================================================
//=========================================================================================================
class SoundBuf										// Звуковой буфер
{
protected:
	ALuint		id;									// ID буфера
	ALuint		FileType;							// Тип файла используемого как источник звука
public:
	SoundBuf();
	~SoundBuf();
	ALvoid		Init		();						// Инициализация
	ALvoid		DeInit		();						// Деинициализация
	ALuint		GetID		()						// Получения идентификатора для линковки с сточником звука
		{	return id;	};
	ALboolean LoadWav(const char *FileName);		// "Загрузка вавки"
};

//=========================================================================================================
//====[ Класс "источник звука" ]===========================================================================
//=========================================================================================================
class soundSource									// Источник звука
{
protected:
	ALuint		id;									// идентификатор звука
	ALuint		BufID;								// идентификатор буфера
	Vector3f	pos;								// Позиция источника звука
	Vector3f	dir;								// Направление источника звука
	Vector3f	vel;								// Скорость источника звука
	ALboolean	now_playing;						// Признак того что файл в данный момент проигрывается
	ALboolean	now_generated;						// Сгенерирован сорц
	ALboolean	linked_with_buffer;					// Связан с буфером
	SoundBuf	*Buf;								// Указатель на используемый звуковой буфер

public:
	soundSource();									// Конструктор
	~soundSource();									// Деструктор

	ALvoid		Init();								// Инициализация источника звука
	ALvoid		DeInit();							// антиинициализация источника звука

	ALboolean	LinkWithBufer		(SoundBuf *Buffer);	// Связать источник с буфером

	ALvoid SetPos(const Vector3f &Pos);				// Установка позиции
		Vector3f GetPos()
		{return pos;};
	ALvoid SetVel(const Vector3f &Vel);				// Установка скорости
		Vector3f GetVel()
		{return vel;};
	ALvoid SetDir(const Vector3f &Dir);				// Задание направления источника звука
		Vector3f GetDir()
		{return dir;};
	ALboolean NowPlaying()							// Сейчас проигрывается
	{	return now_playing;}
	ALvoid SetPitch		( ALfloat pitch );
	ALvoid SetGain		( ALfloat gain);
	ALvoid SetLooping	( ALboolean Looping);		// Установка флага повторения
	ALvoid Play			(ALvoid);					// Воспроизвести(начать воспроизведение)
	ALvoid Pause		(ALvoid);					// Пауза
	ALvoid Stop			(ALvoid);					// Остановить воспроизведение
	ALvoid Rewind		(ALvoid);					// Начать сначала
};

extern	SoundBuf		TestBuf;
extern	soundSource		TestSource;