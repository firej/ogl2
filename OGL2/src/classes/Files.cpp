#ifdef WIN32
#include "../LocusAFX.h"
#else
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#endif
#include "./Files.h"
#include "./shared/assert.h"

// Реализация базового класса File
File::File(){};

File::File(const char *Fname, char Omode, char Ftype, bool isExtended) {
  Open(Fname, Omode, Ftype, isExtended);
};

bool File::Open(const char *Fname, char Omode, char Ftype, bool isExtended) {
  memset(modestr, 0, 4);
  modestr[0] = Omode;
  if (isExtended) { // Проверка нужен ли + в fopen
    modestr[1] = '+';
    modestr[2] = Ftype;
  } else {
    modestr[1] = Ftype;
  }
  isOpen = true;
  f = fopen(Fname, modestr);

  FASSERT(f);
  //	return (bool)f;
  return true; // не имеет смысла из-за FASSERT'а выше
};

void File::Close() {
  //	FASSERT(f);
  if (f)
    fclose(f);
};
File::~File() {
  if (f)
    fclose(f);
}
/*=[ Файлы: Реализация:
]=======================================================================\\
||	1) Лог файл
||	2) Конфигурационный файл
||
\\==============================================================================================*/

void FJCGetTD(char mode, char *str) // Находим время для вывода в файл
{ // mode = 0 - нужна дата без времени
#ifdef WIN32
  SYSTEMTIME LocalTime;
  GetLocalTime(&LocalTime);
  if (mode & FJC_NEED_TIME)   // Нужно время
    if (mode & FJC_NEED_DATE) // Нужна дата
      sprintf(str, "(%02d:%02d:%02d) %02d.%02d.%04d",
              LocalTime.wHour,   // Часы
              LocalTime.wMinute, // Минуты
              LocalTime.wSecond, // Секунды
              LocalTime.wDay,    // День(число)
              LocalTime.wMonth,  // Месяц
              LocalTime.wYear    // Год
      );
    else // if (mode && FJC_NEED_DATE)
      sprintf(str, "(%02d:%02d:%02d)",
              LocalTime.wHour,   // Часы
              LocalTime.wMinute, // Минуты
              LocalTime.wSecond  // Секунды
      );
  else // if (mode && FJC_NEED_TIME)
    sprintf(str, "%02d.%02d.%04d",
            LocalTime.wDay,   // День(число)
            LocalTime.wMonth, // Месяц
            LocalTime.wYear   // Год
    );
#else
  // macOS/Linux version using standard C time functions
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  if (mode & FJC_NEED_TIME)   // Нужно время
    if (mode & FJC_NEED_DATE) // Нужна дата
      snprintf(str, 1024, "(%02d:%02d:%02d) %02d.%02d.%04d",
               timeinfo->tm_hour,       // Часы
               timeinfo->tm_min,        // Минуты
               timeinfo->tm_sec,        // Секунды
               timeinfo->tm_mday,       // День(число)
               timeinfo->tm_mon + 1,    // Месяц (tm_mon is 0-11)
               timeinfo->tm_year + 1900 // Год (tm_year is years since 1900)
      );
    else // if (mode && FJC_NEED_DATE)
      snprintf(str, 1024, "(%02d:%02d:%02d)",
               timeinfo->tm_hour, // Часы
               timeinfo->tm_min,  // Минуты
               timeinfo->tm_sec   // Секунды
      );
  else // if (mode && FJC_NEED_TIME)
    snprintf(str, 1024, "%02d.%02d.%04d",
             timeinfo->tm_mday,       // День(число)
             timeinfo->tm_mon + 1,    // Месяц (tm_mon is 0-11)
             timeinfo->tm_year + 1900 // Год (tm_year is years since 1900)
    );
#endif
}

bool LogFile::Start(const char *message) {
  if (!f)
    return NULL;
  msgLength = (unsigned int)strlen(message);
  memset(msg, 0, 256);
  strcpy(msg, message);
  FJCGetTD(FJC_NEED_TIME | FJC_NEED_DATE, buf2);
  snprintf(buf, 256, "%s: %31.31s %s", msg, "Session sucessfully started at",
           buf2);
  Msg(buf);
  return true;
}
bool LogFile::Msg(const char *msg) {
  if (!f)
    return NULL;
  fprintf(f, "%s\n", msg); // Простое запихивание строки в файл
  return true;
}
bool LogFile::Logf(const char *block, const char *str, ...) {
  if (!f)
    return NULL;
  FJCGetTD(FJC_NEED_TIME, buf);
  snprintf(buf2, 256, "\t%s : <%15.15s> :: %s\n", buf, block, str);
  va_list ap; // Указатель на список аргументов
  va_start(ap, str); // Разбор строки переменных
  static char formatbuf[512];
  vsnprintf(formatbuf, 512, buf2, ap);
  va_end(ap); // Результат помещается в строку

  fprintf(f, "%s", formatbuf); // И конвертирование символов в реальные коды
  printf("%s", formatbuf); // копию оставляем в консоли

  fflush(f);
  return true;
}

bool LogFile::Log(const char *block, const char *msg, bool ok) {
  if (!f)
    return NULL;
  FJCGetTD(FJC_NEED_TIME, buf);
  sprintf(buf2, "\t%s : <%15.15s> :: %50.50s\t\t\t ", buf, block, msg);
  if (ok)
    strcat(buf2, "[   OK   ]");
  else
    strcat(buf2, "[ FAILED ]");
  Msg(buf2);
  return true;
}
bool LogFile::End(bool is_normal) {
  if (!f)
    return NULL;
  FJCGetTD(FJC_NEED_TIME | FJC_NEED_DATE, buf2);
  if (is_normal)
    snprintf(buf, 256, "%s: %31.31s %s", msg, "Session sucessfully ended at",
             buf2);
  else // Не нормально :=)
    snprintf(buf, 256, "%s: Fatal error at %s", msg, buf2);
  Msg(buf);
  return true;
}

bool CFGFile::Gets(char *string) {
  char buf[1024];

  if (!this->f)
    return false;

  while (true) {
    if (fgets(buf, sizeof(buf), this->f) == NULL) {
      // Handle end of file or read error
      return false;
    }

    CleanString(buf);

    // Skip empty lines and comments
    if (buf[0] == '\0' || buf[0] == ';' || (buf[0] == '/' && buf[1] == '/')) {
      continue;
    }

    strncpy(string, buf, 256);
    return true;
  }
}

void CleanString(char *s) {
  // Создаем буфер для результата
  char result[512];
  int j = 0; // Индекс в результирующем буфере
  bool inWord = false; // Флаг нахождения внутри слова

  // Обрабатываем всю строку
  for (int i = 0; s[i] != '\0' && s[i] != '\n'; i++) {
    if (isspace(s[i])) {
      // Если встретили пробел после слова - добавляем один пробел
      if (inWord) {
        result[j++] = ' ';
        inWord = false;
      }
    } else {
      // Добавляем символ слова
      result[j++] = s[i];
      inWord = true;
    }
  }

  // Удаляем последний пробел, если он есть
  if (j > 0 && result[j - 1] == ' ') {
    j--;
  }

  // Завершаем строку
  result[j] = '\0';

  // Копируем результат обратно
  strcpy(s, result);
}
