#pragma once

//====== Общие файлы
#include "./Texture.h"
#include "./globals.h"

class SkyBox // Объект неба
{
protected:
  TextureClass t; // Текстура для неба
  GLuint listID;  // ID списка
public:
  SkyBox();           // Конструктор
  ~SkyBox();          // Деструктор
  void LoadTextures(  // Загрузка текстур неба
      ILenum Type,    // Тип файла
      char *NameOfTex // Массив имён текстур
  );
  void operator()(); // Отрисовать небо
  void Render();     // тоже самое только для красоты
};

extern SkyBox MSB; // Главный объект неба