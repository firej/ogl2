cmake_minimum_required(VERSION 3.5)
project(OGL2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
    # Для macOS/Linux добавляем необходимые файлы
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

    # Добавляем DevIL библиотеки (если установлены через Homebrew) - опционально
    find_library(IL_LIBRARY IL PATHS /opt/homebrew/lib /usr/local/lib)
    find_library(ILU_LIBRARY ILU PATHS /opt/homebrew/lib /usr/local/lib)
    find_library(ILUT_LIBRARY ILUT PATHS /opt/homebrew/lib /usr/local/lib)

    # Основные библиотеки
    if(GLFW_LIBRARY)
        target_link_libraries(OGL2
            ${OPENGL_LIBRARY}
            ${OPENAL_LIBRARY}
            ${GLFW_LIBRARY}
        )
        add_definitions(-DUSE_GLFW)
        message(STATUS "GLFW library found - OpenGL context creation enabled")
    else()
        target_link_libraries(OGL2
            ${OPENGL_LIBRARY}
            ${OPENAL_LIBRARY}
        )
        message(WARNING "GLFW library not found - install with: brew install glfw")
    endif()

    # Добавляем пути к заголовкам GLFW и DevIL
    target_include_directories(OGL2 PRIVATE
        /opt/homebrew/include
        /usr/local/include
    )
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
    find_library(OPENAL_LIBRARY openal)
    find_library(IL_LIBRARY IL)
    find_library(ILU_LIBRARY ILU)
    find_library(ILUT_LIBRARY ILUT)

    target_link_libraries(OGL2
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${IL_LIBRARY}
        ${ILU_LIBRARY}
        ${ILUT_LIBRARY}
    )
endif()
