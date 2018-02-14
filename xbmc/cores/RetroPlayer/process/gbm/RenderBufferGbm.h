/*
 *      Copyright (C) 2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "EGL.h"
#include "cores/RetroPlayer/process/RenderBufferSysMem.h"
#include "windowing/gbm/WinSystemGbmGLESContext.h"

#include "system_gl.h"

#include <gbm.h>

namespace KODI
{
namespace RETRO
{
  class CRenderContext;

  class CRenderBufferGbm : public CRenderBufferSysMem
  {
  public:
    CRenderBufferGbm(CRenderContext &context,
                     int fourcc,
                     int bpp);
    ~CRenderBufferGbm() override;


    // implementation of IRenderBuffer via CRenderBufferSysMem
    bool Allocate(AVPixelFormat format, unsigned int width, unsigned int height, unsigned int size) override;
    size_t GetFrameSize() const override { return m_stride * m_height; }
    uint8_t *GetMemory() override;
    void ReleaseMemory() override;

    // implementation of IRenderBuffer
    bool UploadTexture() override;

    GLuint TextureID() const { return m_textureId; }

  protected:
    // Construction parameters
    CRenderContext &m_context;
    const int m_fourcc = 0;
    const int m_bpp;

    const GLenum m_textureTarget = GL_TEXTURE_2D;
    GLuint m_textureId = 0;

  private:
    void CreateTexture();
    void DeleteTexture();

    std::unique_ptr<CEGL> m_EGL;
    int m_fd = -1;
    uint32_t m_stride = 0;
    struct gbm_bo *m_bo = nullptr;
    uint8_t *m_map = nullptr;
    void *m_map_data = nullptr;
    CWinSystemGbmGLESContext *m_winSystem = nullptr;
  };
}
}
