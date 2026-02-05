cmake_minimum_required(VERSION 3.10)
project(OGL2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Для macOS/Linux добавляем пути к библиотекам FFmpeg ДО add_executable
if(APPLE)
    # Пути для Homebrew (Apple Silicon и Intel)
    link_directories(
        /opt/homebrew/lib
        /opt/homebrew/Cellar/ffmpeg/8.0.1_2/lib
        /opt/homebrew/opt/ffmpeg/lib
        /usr/local/lib
        /usr/local/opt/ffmpeg/lib
    )
elseif(UNIX AND NOT APPLE)
    # Linux - pkg-config добавит пути автоматически
endif()

# Определяем базовые файлы
set(BASE_SOURCE_FILES
    ogl2/src/classes/cons/CCFuncs.cpp
    ogl2/src/classes/cons/FuncsMap.cpp
    ogl2/src/classes/shared/assert.cpp
    ogl2/src/classes/shared/MyMath.cpp
    ogl2/src/classes/Files.cpp
    ogl2/src/renderscene.cpp
    ogl2/src/main.cpp
    ssrc/iff.h)

# Добавляем платформо-специфичные файлы
if(WIN32)
    set(SOURCE_FILES ${BASE_SOURCE_FILES}
        ogl2/src/classes/sound/Sound.cpp
        ogl2/src/classes/Video/AVI_player.cpp
        ogl2/src/classes/ApplicationClass.cpp
        ogl2/src/classes/camera.cpp
        ogl2/src/classes/Console.cpp
        ogl2/src/classes/ConsoleFunctions.cpp
        ogl2/src/classes/lwo_mesh.cpp
        ogl2/src/classes/objects.cpp
        ogl2/src/classes/ParticleSystem.cpp
        ogl2/src/classes/resman.cpp
        ogl2/src/classes/simple_mesh.cpp
        ogl2/src/classes/Text.cpp
        ogl2/src/classes/Texture.cpp
        ogl2/src/LocusAFX.cpp
        ogl2/src/renderscene.cpp)
else()
    # Для macOS/Linux добавляем необходимые файлы (FFmpeg встроен в AVI_player.cpp)
    set(SOURCE_FILES ${BASE_SOURCE_FILES}
        ogl2/src/classes/ApplicationClass.cpp
        ogl2/src/classes/sound/Sound.cpp
        ogl2/src/classes/Video/AVI_player.cpp
        ogl2/src/classes/camera.cpp
        ogl2/src/classes/Console.cpp
        ogl2/src/classes/ConsoleFunctions.cpp
        ogl2/src/classes/lwo_mesh.cpp
        ogl2/src/classes/objects.cpp
        ogl2/src/classes/ParticleSystem.cpp
        ogl2/src/classes/resman.cpp
        ogl2/src/classes/simple_mesh.cpp
        ogl2/src/classes/Text.cpp
        ogl2/src/classes/Texture.cpp)
endif()

add_executable(OGL2 ${SOURCE_FILES})

# Добавляем библиотеки для разных платформ
if(APPLE)
    # Подавляем предупреждения о deprecated OpenGL на macOS
    add_definitions(-DGL_SILENCE_DEPRECATION)

    # macOS библиотеки
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(OPENAL_LIBRARY OpenAL)

    # Добавляем GLFW для создания окон и OpenGL контекста
    find_library(GLFW_LIBRARY glfw PATHS /opt/homebrew/lib /usr/local/lib)

    # FFmpeg библиотеки для видеоплеера
    # Используем pkg-config для поиска FFmpeg
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(FFMPEG QUIET libavcodec libavformat libswscale libavutil)
    endif()

    # Если pkg-config нашел FFmpeg, используем его данные
    if(FFMPEG_FOUND)
        message(STATUS "FFmpeg found via pkg-config: ${FFMPEG_LIBRARY_DIRS}")
        # Используем полные пути к библиотекам
        set(FFMPEG_LIBS avcodec avformat swscale avutil)
    else()
        # Если pkg-config не нашел, ищем вручную
        set(FFMPEG_SEARCH_PATHS
            /opt/homebrew/lib
            /opt/homebrew/Cellar/ffmpeg/8.0.1_2/lib
            /opt/homebrew/opt/ffmpeg/lib
            /usr/local/lib
            /usr/local/opt/ffmpeg/lib
        )
        set(FFMPEG_INCLUDE_PATHS
            /opt/homebrew/include
            /opt/homebrew/Cellar/ffmpeg/8.0.1_2/include
            /opt/homebrew/opt/ffmpeg/include
            /usr/local/include
            /usr/local/opt/ffmpeg/include
        )

        find_library(AVCODEC_LIBRARY avcodec PATHS ${FFMPEG_SEARCH_PATHS})
        find_library(AVFORMAT_LIBRARY avformat PATHS ${FFMPEG_SEARCH_PATHS})
        find_library(SWSCALE_LIBRARY swscale PATHS ${FFMPEG_SEARCH_PATHS})
        find_library(AVUTIL_LIBRARY avutil PATHS ${FFMPEG_SEARCH_PATHS})

        if(AVCODEC_LIBRARY AND AVFORMAT_LIBRARY AND SWSCALE_LIBRARY AND AVUTIL_LIBRARY)
            set(FFMPEG_LIBS ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${SWSCALE_LIBRARY} ${AVUTIL_LIBRARY})
            set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_PATHS})
            set(FFMPEG_FOUND TRUE)
            message(STATUS "FFmpeg found manually: ${AVCODEC_LIBRARY}")
        endif()
    endif()

    if(FFMPEG_FOUND)
        message(STATUS "FFmpeg libraries found - video playback enabled (MP4, AVI, MKV, etc.)")
        add_definitions(-DFFMPEG_ENABLED)
    else()
        message(WARNING "FFmpeg not found - video playback will be disabled")
        message(WARNING "Install FFmpeg with: brew install ffmpeg")
    endif()

    # Основные библиотеки
    set(MACOS_LIBS ${OPENGL_LIBRARY} ${OPENAL_LIBRARY})

    if(GLFW_LIBRARY)
        list(APPEND MACOS_LIBS ${GLFW_LIBRARY})
        add_definitions(-DUSE_GLFW)
        message(STATUS "GLFW library found - OpenGL context creation enabled")
    else()
        message(WARNING "GLFW library not found - install with: brew install glfw")
    endif()

    if(FFMPEG_FOUND)
        list(APPEND MACOS_LIBS ${FFMPEG_LIBS})
    endif()

    target_link_libraries(OGL2 ${MACOS_LIBS})

    # Добавляем пути к заголовкам
    target_include_directories(OGL2 PRIVATE
        /opt/homebrew/include
        /opt/homebrew/opt/ffmpeg/include
        /usr/local/include
        /usr/local/opt/ffmpeg/include
    )

    if(FFMPEG_FOUND AND FFMPEG_INCLUDE_DIRS)
        target_include_directories(OGL2 PRIVATE ${FFMPEG_INCLUDE_DIRS})
    endif()
elseif(WIN32)
    # Windows библиотеки (если нужно)
    target_link_libraries(OGL2
        opengl32
        glu32
        # Добавить другие Windows библиотеки по необходимости
    )
else()
    # Linux библиотеки
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    find_library(OPENAL_LIBRARY openal)
    find_library(IL_LIBRARY IL)
    find_library(ILU_LIBRARY ILU)
    find_library(ILUT_LIBRARY ILUT)

    # FFmpeg для Linux
    pkg_check_modules(FFMPEG QUIET libavcodec libavformat libswscale libavutil)

    if(FFMPEG_FOUND)
        message(STATUS "FFmpeg libraries found - video playback enabled")
        add_definitions(-DFFMPEG_ENABLED)
    else()
        message(WARNING "FFmpeg not found - video playback will be disabled")
        message(WARNING "Install FFmpeg with: sudo apt install libavcodec-dev libavformat-dev libswscale-dev libavutil-dev")
    endif()

    set(LINUX_LIBS
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${IL_LIBRARY}
        ${ILU_LIBRARY}
        ${ILUT_LIBRARY}
        ${GLFW_LIBRARIES}
    )

    if(FFMPEG_FOUND)
        list(APPEND LINUX_LIBS ${FFMPEG_LIBRARIES})
    endif()

    target_link_libraries(OGL2 ${LINUX_LIBS})

    target_include_directories(OGL2 PRIVATE ${GLFW_INCLUDE_DIRS})

    if(FFMPEG_FOUND)
        target_include_directories(OGL2 PRIVATE ${FFMPEG_INCLUDE_DIRS})
    endif()

    target_compile_options(OGL2 PRIVATE ${GLFW_CFLAGS_OTHER})
    add_definitions(-DUSE_GLFW)
endif()
