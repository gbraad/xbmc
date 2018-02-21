set(SOURCES RenderContext.cpp
            RenderGeometry.cpp
            RenderResourceSysMem.cpp
            RenderSettings.cpp
            RenderTranslator.cpp
            RenderVideoSettings.cpp
            RPRenderManager.cpp
)

set(HEADERS IRenderManager.h
            IRenderResource.h
            IRenderResourceMemory.h
            RenderContext.h
            RenderGeometry.h
            RenderResourceSysMem.h
            RenderSettings.h
            RenderTranslator.h
            RenderVideoSettings.h
            RPRenderManager.h
)

core_add_library(rp-rendering)
