/*
 *      Copyright (C) 2018 Team Kodi
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

#include "IRenderTexture.h"

#include "system_gl.h"

namespace KODI
{
namespace RETRO
{
  class CRendererTextureOpenGL : public IRenderTexture
  {
  public:
    CRendererTextureOpenGL(unsigned int width, unsigned int height) override;
    ~CRendererTextureOpenGL() override = default;

    // Implementation of IRenderTexture
    unsigned int GetWidth() const override { return m_width; }
    unsigned int GetHeight() const override { return m_height; }
    bool CreateTexture() override;
    void DestroyTexture() override;

    // OpenGL properties
    GLuint TextureID() const { return m_textureId; }
    GLenum TextureTarget() const { return m_textureTarget; }

  private:
    // Construction parameters
    const unsigned int m_width;
    const unsigned int m_height;
    const GLuint m_textureId;

    // OpenGL parameters
    const GLenum m_textureTarget = GL_TEXTURE_2D; //! @todo
  };
}
}
