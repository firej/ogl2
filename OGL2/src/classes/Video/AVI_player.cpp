#ifdef WIN32
#include "../../LocusAFX.h"
#pragma comment(lib, "vfw32.lib")  // Search For VFW32.lib While Linking
#else
// На macOS/Linux используем FFmpeg
#include <cstdlib>
#include <cstring>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// FFmpeg headers
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <algorithm>
#endif

#include "../Time.h"
#include "../globals.h"
#include "AVI_player.h"

bool end_of_file = false;

#ifdef WIN32
void flipIt(void *b, int texS)  // Flips The Red And Blue Bytes (256x256)
{
    __asm          // Assembler Code To Follow
    {
		mov ecx, texS  // Counter Set To Dimensions Of Our Memory Block
		mov ebx, b  // Points ebx To Our Data (b)
		label:  // Label Used For Looping
			mov al,[ebx+0]  // Loads Value At ebx Into al
			mov ah,[ebx+2]  // Loads Value At ebx+2 Into ah
			mov [ebx+2],al  // Stores Value In al At ebx+2
			mov [ebx+0],ah  // Stores Value In ah At ebx

			add ebx,3  // Moves Through The Data By 3 Bytes
			dec ecx  // Decreases Our Loop Counter
			jnz label  // If Not Zero Jump Back To Label
    }
}
#else
void flipIt(void* b, int texS)  // Меняет местами красный и синий байты
{
    unsigned char* bytes = (unsigned char*)b;
    for (int i = 0; i < texS; i++) {
        unsigned char temp = bytes[0];
        bytes[0] = bytes[2];
        bytes[2] = temp;
        bytes += 3;
    }
}
#endif

AVI_player::AVI_player(void) {
#ifdef WIN32
    twidth = theight = 0;
    textureID = 0;
    pboID = 0;
    usePBO = true;
#else
    // Инициализация для macOS/Linux с FFmpeg
    twidth = theight = 0;
    lastframe = mpf = frame = 0;
    data = nullptr;
    textureID = 0;
    pboID = 0;
    usePBO = true;

    // FFmpeg структуры
    formatContext = nullptr;
    codecContext = nullptr;
    swsContext = nullptr;
    avFrame = nullptr;
    frameRGB = nullptr;
    packet = nullptr;
    rgbBuffer = nullptr;
    videoStreamIndex = -1;

    // Инициализация FFmpeg (для старых версий)
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
#endif
}

AVI_player::~AVI_player(void) {
#ifndef WIN32
    CloseAVI();
#endif
}

bool AVI_player::Open(TCHAR *FileName, int Width, int Height) {
    this->twidth = Width;
    this->theight = Height;
    return OpenAVI(FileName);
}

#ifdef WIN32
bool AVI_player::OpenAVI(char *szFile)  // Opens An AVI File (szFile)
{
    hdc = CreateCompatibleDC(0);
    AVIFileInit();  // Opens The AVIFile Library
    if (GetLastError()) {
        return FJC_ERROR;
    }
    hdd = DrawDibOpen();

    if (AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) != 0) {
        return FJC_ERROR;
    }

    AVIStreamInfo(pavi, &psi, sizeof(psi));
    width = psi.rcFrame.right - psi.rcFrame.left;
    height = psi.rcFrame.bottom - psi.rcFrame.top;

    lastframe = AVIStreamLength(pavi);
    mpf = AVIStreamSampleToTime(pavi, lastframe) / lastframe;

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;
    bmih.biWidth = twidth;
    bmih.biHeight = theight;
    bmih.biCompression = BI_RGB;

    hBitmap = CreateDIBSection(hdc, (BITMAPINFO *)(&bmih), DIB_RGB_COLORS, (void **)(&data), NULL, NULL);
    DWORD err = GetLastError();
    if (err != 0) LF.Logf("AVI Player", "Error #%d on CreateDIBSection", err);
    SelectObject(hdc, hBitmap);

    pgf = AVIStreamGetFrameOpen(pavi, NULL);
    if (pgf == NULL) {
        return FJC_ERROR;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    LF.Logf("AVI_player", "Opened AVI video: %s (%dx%d, %d frames), textureID: %u",
            szFile, width, height, lastframe, textureID);

    return FJC_NO_ERROR;
}

void AVI_player::GrabAVIFrame()
{
    frame = (int)((1000.0 * GT.GetWorldTime()) / (double)mpf);
    if (frame >= lastframe) return;
    LPBITMAPINFOHEADER lpbi;
    lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);
    if (!lpbi) return;
    pdata = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);

    DrawDibDraw(hdd, hdc, 0, 0, twidth, theight, lpbi, pdata, 0, 0, width, height, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void AVI_player::GrabAVIFrame(int frame)
{
    this->frame = frame;
    LPBITMAPINFOHEADER lpbi;
    lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);
    pdata = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);

    DrawDibDraw(hdd, hdc, 0, 0, twidth, theight, lpbi, pdata, 0, 0, width, height, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void AVI_player::CloseAVI()
{
    DeleteObject(hBitmap);
    DrawDibClose(hdd);
    AVIStreamGetFrameClose(pgf);
    AVIStreamRelease(pavi);
    AVIFileExit();
}
#else
// ============================================================================
// Реализация для macOS/Linux с использованием FFmpeg
// ============================================================================

bool AVI_player::OpenAVI(char* szFile) {
    // Закрываем предыдущий файл если был открыт
    CloseAVI();

    // Открываем файл
    int ret = avformat_open_input(&formatContext, szFile, nullptr, nullptr);
    if (ret < 0) {
        char errBuf[256];
        av_strerror(ret, errBuf, sizeof(errBuf));
        LF.Logf("AVI_player", "Failed to open file '%s': %s", szFile, errBuf);
        return FJC_ERROR;
    }

    // Получаем информацию о потоках
    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret < 0) {
        LF.Logf("AVI_player", "Failed to find stream info");
        CloseAVI();
        return FJC_ERROR;
    }

    // Ищем видео поток
    videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex < 0) {
        LF.Logf("AVI_player", "No video stream found in file");
        CloseAVI();
        return FJC_ERROR;
    }

    AVStream* videoStream = formatContext->streams[videoStreamIndex];
    AVCodecParameters* codecParams = videoStream->codecpar;

    // Находим декодер
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        LF.Logf("AVI_player", "Unsupported codec");
        CloseAVI();
        return FJC_ERROR;
    }

    // Создаем контекст кодека
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LF.Logf("AVI_player", "Failed to allocate codec context");
        CloseAVI();
        return FJC_ERROR;
    }

    // Копируем параметры кодека
    ret = avcodec_parameters_to_context(codecContext, codecParams);
    if (ret < 0) {
        LF.Logf("AVI_player", "Failed to copy codec parameters");
        CloseAVI();
        return FJC_ERROR;
    }

    // Открываем кодек
    ret = avcodec_open2(codecContext, codec, nullptr);
    if (ret < 0) {
        LF.Logf("AVI_player", "Failed to open codec");
        CloseAVI();
        return FJC_ERROR;
    }

    // Получаем информацию о видео
    int videoWidth = codecContext->width;
    int videoHeight = codecContext->height;

    // ВАЖНО: Если twidth/theight не заданы (равны 0), используем оригинальное разрешение видео
    if (twidth == 0 || theight == 0) {
        twidth = videoWidth;
        theight = videoHeight;
        LF.Logf("AVI_player", "Using original video resolution: %dx%d", twidth, theight);
    }

    // Вычисляем frame rate
    double frameRate;
    if (videoStream->avg_frame_rate.den != 0) {
        frameRate = av_q2d(videoStream->avg_frame_rate);
    } else if (videoStream->r_frame_rate.den != 0) {
        frameRate = av_q2d(videoStream->r_frame_rate);
    } else {
        frameRate = 25.0;
    }

    // Вычисляем количество кадров
    if (videoStream->nb_frames > 0) {
        lastframe = (int)videoStream->nb_frames;
    } else {
        double duration;
        if (formatContext->duration != AV_NOPTS_VALUE) {
            duration = formatContext->duration / (double)AV_TIME_BASE;
        } else if (videoStream->duration != AV_NOPTS_VALUE) {
            duration = videoStream->duration * av_q2d(videoStream->time_base);
        } else {
            duration = 0.0;
        }
        lastframe = (int)(duration * frameRate);
    }

    // Миллисекунды на кадр
    mpf = (int)(1000.0 / frameRate);

    // Выделяем фреймы
    avFrame = av_frame_alloc();
    frameRGB = av_frame_alloc();
    if (!avFrame || !frameRGB) {
        LF.Logf("AVI_player", "Failed to allocate frames");
        CloseAVI();
        return FJC_ERROR;
    }

    // Выделяем буфер для RGB данных
    int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, twidth, theight, 1);
    rgbBuffer = new unsigned char[rgbBufferSize];

    // Связываем буфер с frameRGB
    av_image_fill_arrays(frameRGB->data, frameRGB->linesize, rgbBuffer,
                         AV_PIX_FMT_RGB24, twidth, theight, 1);

    // Создаем контекст масштабирования/конвертации
    swsContext = sws_getContext(
        videoWidth, videoHeight, codecContext->pix_fmt,
        twidth, theight, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    if (!swsContext) {
        LF.Logf("AVI_player", "Failed to create scaling context");
        CloseAVI();
        return FJC_ERROR;
    }

    // Выделяем пакет
    packet = av_packet_alloc();
    if (!packet) {
        LF.Logf("AVI_player", "Failed to allocate packet");
        CloseAVI();
        return FJC_ERROR;
    }

    frame = -1;  // -1 чтобы первый кадр (0) был загружен
    data = rgbBuffer;

    // Создаем OpenGL текстуру
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Декодируем и загружаем первый кадр
    if (decodeNextFrame()) {
        convertFrameToRGB();
        // Создаем текстуру с первым кадром
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbBuffer);
        LF.Logf("AVI_player", "First frame loaded and uploaded to texture");
    } else {
        // Создаем пустую текстуру если не удалось загрузить первый кадр
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        LF.Logf("AVI_player", "WARNING: Failed to load first frame");
    }

    // Создаем PBO если поддерживается
    if (usePBO) {
        glGenBuffers(1, &pboID);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, twidth * theight * 3, nullptr, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        LF.Logf("AVI_player", "PBO created with ID: %u", pboID);
    }

    LF.Logf("AVI_player", "Opened video: %s (%dx%d, %d frames, %.1f fps), textureID: %u, PBO: %s",
            szFile, videoWidth, videoHeight, lastframe, frameRate, textureID,
            usePBO ? "enabled" : "disabled");

    return FJC_NO_ERROR;
}

bool AVI_player::decodeNextFrame() {
    if (!formatContext || !codecContext) return false;

    while (true) {
        // Читаем пакет
        int ret = av_read_frame(formatContext, packet);
        if (ret < 0) {
            return false;  // Конец файла или ошибка
        }

        // Проверяем что это видео пакет
        if (packet->stream_index != videoStreamIndex) {
            av_packet_unref(packet);
            continue;
        }

        // Отправляем пакет декодеру
        ret = avcodec_send_packet(codecContext, packet);
        av_packet_unref(packet);

        if (ret < 0) {
            continue;
        }

        // Получаем декодированный фрейм
        ret = avcodec_receive_frame(codecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        }
        if (ret < 0) {
            return false;
        }

        // Успешно декодировали кадр
        frame++;
        return true;
    }
}

void AVI_player::convertFrameToRGB() {
    if (!avFrame || !frameRGB || !swsContext) return;

    sws_scale(swsContext,
              avFrame->data, avFrame->linesize, 0, codecContext->height,
              frameRGB->data, frameRGB->linesize);
}

void AVI_player::GrabAVIFrame() {
    if (!formatContext || !codecContext) return;

    // Вычисляем какой кадр должен быть показан на основе времени
    int targetFrame = (int)((1000.0 * GT.GetWorldTime()) / (double)mpf);

    if (targetFrame >= lastframe) {
        return;
    }

    // Если нужный кадр уже декодирован - просто обновляем текстуру
    if (targetFrame == frame) {
        // Кадр уже декодирован, просто конвертируем и обновляем текстуру
        convertFrameToRGB();
    } else if (targetFrame > frame) {
        // Нужно декодировать следующие кадры
        while (frame < targetFrame) {
            if (!decodeNextFrame()) {
                return;
            }
        }
        // Конвертируем последний декодированный кадр
        convertFrameToRGB();
    } else {
        // targetFrame < frame - нужно перемотать назад (не должно происходить при последовательном воспроизведении)
        return;
    }

    // ВАЖНО: Привязываем текстуру ПЕРЕД обновлением
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Обновляем текстуру напрямую без PBO для надежности
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_RGB, GL_UNSIGNED_BYTE, rgbBuffer);

    // Отвязываем текстуру
    glBindTexture(GL_TEXTURE_2D, 0);
}

void AVI_player::GrabAVIFrame(int frameNum) {
    if (!formatContext || !codecContext) return;

    // Если это тот же кадр - ничего не делаем
    if (frameNum == frame && data != nullptr) {
        return;
    }

    // Если нужен кадр раньше текущего - перематываем на начало
    if (frameNum < frame) {
        av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(codecContext);
        frame = -1;
    }

    // Декодируем кадры до нужного
    while (frame < frameNum) {
        if (!decodeNextFrame()) {
            return;
        }
    }

    // Конвертируем в RGB
    convertFrameToRGB();

    // Обновляем текстуру
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_RGB, GL_UNSIGNED_BYTE, rgbBuffer);
}

void AVI_player::CloseAVI() {
    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    if (packet) {
        av_packet_free(&packet);
        packet = nullptr;
    }

    if (frameRGB) {
        av_frame_free(&frameRGB);
        frameRGB = nullptr;
    }

    if (avFrame) {
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }

    if (codecContext) {
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
    }

    if (formatContext) {
        avformat_close_input(&formatContext);
        formatContext = nullptr;
    }

    if (rgbBuffer) {
        delete[] rgbBuffer;
        rgbBuffer = nullptr;
    }

    // Освобождаем текстуру OpenGL
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    // Освобождаем PBO
    if (pboID != 0) {
        glDeleteBuffers(1, &pboID);
        pboID = 0;
    }

    data = nullptr;
    frame = 0;
    lastframe = 0;
    videoStreamIndex = -1;
}
#endif

bool AVI_player::End() {
    if ((lastframe - frame) <= 0)
        return true;
    else
        return false;
}
