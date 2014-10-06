#ifndef	INPUT_HEADER
#define INPUT_HEADER

#define _CRT_SECURE_NO_DEPRECATE
#include <string.h>

typedef _W64 unsigned int WPARAM;			// From basetsd.h & windef.h
typedef	WPARAM				KeyID;
namespace Input
{
#define VK_LBUTTON             0x01
#define VK_RBUTTON             0x02
#define VK_MBUTTON             0x04
#define VK_XBUTTON1            0x05
#define VK_XBUTTON2            0x06

	class	Input_t
	{
		//===[ VARS ]========================================================================================
	private:bool		keys[256];			// Массив клавиш (+мышь)
			char		keysSN[256][32];		// Массив символьных имён клавиш
	public:	int			wheel;				// Поворот колеса
		//===[ METHODS ]=====================================================================================
			Input_t			()				// Конструктор
			{	memset(keys,false,sizeof(keys)); wheel = 0;
				memset(keysSN,0,sizeof(keysSN));
				setSN(0x01, "VK_LBUTTON");
				setSN(0x02, "VK_RBUTTON");
				setSN(0x03, "VK_CANCEL");
				setSN(0x04, "VK_MBUTTON");
				setSN(0x05, "VK_XBUTTON1");
				setSN(0x06, "VK_XBUTTON2");
				setSN(0x08, "VK_BACK");
				setSN(0x09, "VK_TAB");
				setSN(0x0C, "VK_CLEAR");
				setSN(0x0D, "VK_RETURN");
				setSN(0x10, "VK_SHIFT");
				setSN(0x11, "VK_CONTROL");
				setSN(0x12, "VK_MENU");
				setSN(0x13, "VK_PAUSE");
				setSN(0x14, "VK_CAPITAL");
				setSN(0x15, "VK_HANGUL");
				setSN(0x17, "VK_JUNJA");
				setSN(0x18, "VK_FINAL");
				setSN(0x19, "VK_KANJI");
				setSN(0x1B, "VK_ESCAPE");
				setSN(0x1C, "VK_CONVERT");
				setSN(0x1D, "VK_NONCONVERT");
				setSN(0x1E, "VK_ACCEPT");
				setSN(0x1F, "VK_MODECHANGE");
				setSN(0x20, "VK_SPACE");
				setSN(0x21, "VK_PRIOR");
				setSN(0x22, "VK_NEXT");
				setSN(0x23, "VK_END");
				setSN(0x24, "VK_HOME");
				setSN(0x25, "VK_LEFT");
				setSN(0x26, "VK_UP");
				setSN(0x27, "VK_RIGHT");
				setSN(0x28, "VK_DOWN");
				setSN(0x29, "VK_SELECT");
				setSN(0x2A, "VK_PRINT");
				setSN(0x2B, "VK_EXECUTE");
				setSN(0x2C, "VK_SNAPSHOT");
				setSN(0x2D, "VK_INSERT");
				setSN(0x2E, "VK_DELETE");
				setSN(0x2F, "VK_HELP");
				setSN(0x5B, "VK_LWIN");
				setSN(0x5C, "VK_RWIN");
				setSN(0x5D, "VK_APPS");
				setSN(0x5F, "VK_SLEEP");
				setSN(0x60, "VK_NUMPAD0");
				setSN(0x61, "VK_NUMPAD1");
				setSN(0x62, "VK_NUMPAD2");
				setSN(0x63, "VK_NUMPAD3");
				setSN(0x64, "VK_NUMPAD4");
				setSN(0x65, "VK_NUMPAD5");
				setSN(0x66, "VK_NUMPAD6");
				setSN(0x67, "VK_NUMPAD7");
				setSN(0x68, "VK_NUMPAD8");
				setSN(0x69, "VK_NUMPAD9");
				setSN(0x6A, "VK_MULTIPLY");
				setSN(0x6B, "VK_ADD");
				setSN(0x6C, "VK_SEPARATOR");
				setSN(0x6D, "VK_SUBTRACT");
				setSN(0x6E, "VK_DECIMAL");
				setSN(0x6F, "VK_DIVIDE");
				setSN(0x70, "VK_F1");
				setSN(0x71, "VK_F2");
				setSN(0x72, "VK_F3");
				setSN(0x73, "VK_F4");
				setSN(0x74, "VK_F5");
				setSN(0x75, "VK_F6");
				setSN(0x76, "VK_F7");
				setSN(0x77, "VK_F8");
				setSN(0x78, "VK_F9");
				setSN(0x79, "VK_F10");
				setSN(0x7A, "VK_F11");
				setSN(0x7B, "VK_F12");
				setSN(0x7C, "VK_F13");
				setSN(0x7D, "VK_F14");
				setSN(0x7E, "VK_F15");
				setSN(0x7F, "VK_F16");
				setSN(0x80, "VK_F17");
				setSN(0x81, "VK_F18");
				setSN(0x82, "VK_F19");
				setSN(0x83, "VK_F20");
				setSN(0x84, "VK_F21");
				setSN(0x85, "VK_F22");
				setSN(0x86, "VK_F23");
				setSN(0x87, "VK_F24");
				setSN(0x90, "VK_NUMLOCK");
				setSN(0x91, "VK_SCROLL");
				setSN(0x92, "VK_OEM_NEC_EQUAL");
				setSN(0x92, "VK_OEM_FJ_JISHO");
				setSN(0x93, "VK_OEM_FJ_MASSHOU");
				setSN(0x94, "VK_OEM_FJ_TOUROKU");
				setSN(0x95, "VK_OEM_FJ_LOYA");
				setSN(0x96, "VK_OEM_FJ_ROYA");
				setSN(0xA6, "VK_BROWSER_BACK");
				setSN(0xA7, "VK_BROWSER_FORWARD");
				setSN(0xA8, "VK_BROWSER_REFRESH");
				setSN(0xA9, "VK_BROWSER_STOP");
				setSN(0xAA, "VK_BROWSER_SEARCH");
				setSN(0xAB, "VK_BROWSER_FAVORITES");
				setSN(0xAC, "VK_BROWSER_HOME");
				setSN(0xAD, "VK_VOLUME_MUTE");
				setSN(0xAE, "VK_VOLUME_DOWN");
				setSN(0xAF, "VK_VOLUME_UP");
				setSN(0xB0, "VK_MEDIA_NEXT_TRACK");
				setSN(0xB1, "VK_MEDIA_PREV_TRACK");
				setSN(0xB2, "VK_MEDIA_STOP");
				setSN(0xB3, "VK_MEDIA_PLAY_PAUSE");
				setSN(0xB4, "VK_LAUNCH_MAIL");
				setSN(0xB5, "VK_LAUNCH_MEDIA_SELECT");
				setSN(0xB6, "VK_LAUNCH_APP1");
				setSN(0xB7, "VK_LAUNCH_APP2");
				setSN(0xBA, "VK_OEM_1");
				setSN(0xBB, "VK_OEM_PLUS");
				setSN(0xBC, "VK_OEM_COMMA");
				setSN(0xBD, "VK_OEM_MINUS");
				setSN(0xBE, "VK_OEM_PERIOD");
				setSN(0xBF, "VK_OEM_2");
				setSN(0xC0, "VK_OEM_3");
				setSN(0xDB, "VK_OEM_4");
				setSN(0xDC, "VK_OEM_5");
				setSN(0xDD, "VK_OEM_6");
				setSN(0xDE, "VK_OEM_7");
				setSN(0xDF, "VK_OEM_8");
				setSN(0xE1, "VK_OEM_AX");
				setSN(0xE2, "VK_OEM_102");
				setSN(0xE3, "VK_ICO_HELP");
				setSN(0xE4, "VK_ICO_00");
				setSN(0xE5, "VK_PROCESSKEY");
				setSN(0xE6, "VK_ICO_CLEAR");
				setSN(0xE7, "VK_PACKET");
				setSN(0xE9, "VK_OEM_RESET");
				setSN(0xEA, "VK_OEM_JUMP");
				setSN(0xEB, "VK_OEM_PA1");
				setSN(0xEC, "VK_OEM_PA2");
				setSN(0xED, "VK_OEM_PA3");
				setSN(0xEE, "VK_OEM_WSCTRL");
				setSN(0xEF, "VK_OEM_CUSEL");
				setSN(0xF0, "VK_OEM_ATTN");
				setSN(0xF1, "VK_OEM_FINISH");
				setSN(0xF2, "VK_OEM_COPY");
				setSN(0xF3, "VK_OEM_AUTO");
				setSN(0xF4, "VK_OEM_ENLW");
				setSN(0xF5, "VK_OEM_BACKTAB");
				setSN(0xF6, "VK_ATTN");
				setSN(0xF7, "VK_CRSEL");
				setSN(0xF8, "VK_EXSEL");
				setSN(0xF9, "VK_EREOF");
				setSN(0xFA, "VK_PLAY");
				setSN(0xFB, "VK_ZOOM");
				setSN(0xFC, "VK_NONAME");
				setSN(0xFD, "VK_PA1");
				setSN(0xFE, "VK_OEM_CLEAR");
				char	buf[16];
				for ( int i = 0x30 ; i <= 0x39 ; i++ )
				{sprintf(buf,"VK_%c",i); setSN(i,buf);	};
				for ( int i = 0x41 ; i <= 0x5A ; i++ )
				{sprintf(buf,"VK_%c",i); setSN(i,buf);	};
			};
			~Input_t			()
			{};
			inline	void	KeyDown				(WPARAM	key)
			{
				set(key,true);
			}
			inline	void	KeyUp				(WPARAM	key)
			{
				set(key,false);
			}
		// Установка статуса клавиши
			inline	void	set					(WPARAM key, bool status)
			{	keys[key] = status;						};
		// Получение статуса клавиши
			inline	bool	get					(WPARAM key)
			{	return keys[key];						};
			inline	bool	process				(WPARAM key)
			{	if (keys[key]) { keys[key] = false; return true;} return false;}
		// Записать в "хранилище" символьное имя для клавиши
			void			setSN				(WPARAM key,const char* SN)
			{	strncpy(keysSN[key],SN,32);	};
		// Возврат имени клавиши по коду
			char	*		getSN				(WPARAM key)
			{	return keysSN[key];						};
		// Возврат кода клавиши по символьному имени (без учёта региста)
			WPARAM			getBySN				(char	*	SN)
			{
				for(int i=0;i<256;i++)
				{
					size_t size = 0;
					if (strlen(keysSN[i]) != (size = strlen(SN))) continue;
					if (memcmp(keysSN[i],SN,size) == 0) return i;
				};
				return 0;
			};
			void			reset				(void)
			{memset(keys,false,sizeof(keys)); wheel = 0;}
	};
	extern Input_t *I;
}
//===[ Symbolyc names ]======================================================================================


#endif /*#ifndef	INPUT_HEADER*/