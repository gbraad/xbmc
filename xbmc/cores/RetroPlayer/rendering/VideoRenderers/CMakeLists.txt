set(SOURCES RPBaseRenderer.cpp
            RPRendererGuiTexture.cpp)
set(HEADERS RPBaseRenderer.h
            RPRendererGuiTexture.h)

if(CORE_SYSTEM_NAME STREQUAL windows)
  list(APPEND SOURCES RPWinRenderer.cpp)
  list(APPEND HEADERS RPWinRenderer.h)
endif()

if(OPENGL_FOUND OR OPENGLES_FOUND)
  list(APPEND SOURCES RPRendererOpenGLES.cpp)
  list(APPEND HEADERS RPRendererOpenGLES.h)

  if(OPENGLES_FOUND AND CORE_PLATFORM_NAME_LC STREQUAL gbm)
    list(APPEND SOURCES RPRendererOpenGLESPRIME.cpp)
    list(APPEND HEADERS RPRendererOpenGLESPRIME.h)
  endif()
endif()

if(OPENGL_FOUND)
  list(APPEND SOURCES RPRendererOpenGL.cpp)
  list(APPEND HEADERS RPRendererOpenGL.h)
endif()

if(MMAL_FOUND)
  list(APPEND SOURCES MMALProcess.cpp
                      MMALRenderer.cpp
                      RPRendererMMAL.cpp)
  list(APPEND HEADERS MMALProcess.h
                      MMALRenderer.h
                      RPRendererMMAL.h)
endif()

core_add_library(rp-videorenderers)
