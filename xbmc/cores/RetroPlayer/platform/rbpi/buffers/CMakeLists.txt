if(MMAL_FOUND)
  list(APPEND SOURCES RenderBufferMMAL.cpp
                      RenderBufferPoolMMAL.cpp)
  list(APPEND HEADERS RenderBufferMMAL.h
                      RenderBufferPoolMMAL.h)
  core_add_library(rp_platform_mmal)
endif()
