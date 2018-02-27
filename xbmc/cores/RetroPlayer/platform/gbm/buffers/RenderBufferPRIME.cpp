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

#include "RenderBufferPRIME.h"
#include "ServiceBroker.h"

#include "windowing/gbm/WinSystemGbmGLESContext.h"

using namespace KODI;
using namespace RETRO;

CRenderBufferPRIME::CRenderBufferPRIME(CRenderContext &context,
                                       int fourcc,
                                       int bpp) :
  m_context(context),
  m_fourcc(fourcc),
  m_bpp(bpp),
  m_egl(new CEGLImage(dynamic_cast<CWinSystemGbmGLESContext*>(&CServiceBroker::GetWinSystem())->GetEGLDisplay())),
  m_bo(new CGBMBufferObject(fourcc))
{
}

CRenderBufferPRIME::~CRenderBufferPRIME()
{
  DeleteTexture();
}

bool CRenderBufferPRIME::Allocate(AVPixelFormat format, unsigned int width, unsigned int height, unsigned int size)
{
  // Initialize IRenderBuffer
  m_format = format;
  m_width = width;
  m_height = height;

  m_bo->CreateBufferObject(m_width, m_height);

  return true;
}

void CRenderBufferPRIME::CreateTexture()
{
  glGenTextures(1, &m_textureId);

  glBindTexture(m_textureTarget, m_textureId);

  glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(m_textureTarget, 0);
}

bool CRenderBufferPRIME::UploadTexture()
{
  if (m_bo->GetFd() == -1)
    return false;

  if (!glIsTexture(m_textureId))
    CreateTexture();

  glBindTexture(m_textureTarget, m_textureId);

  eglAttrs imageAttrs =
  {
    (int)m_width,
    (int)m_height,
    m_fourcc,
    1,
    { m_bo->GetFd(), 0, 0 },
    { 0, 0, 0 },
    { m_bo->GetStride(), 0, 0 },
  };

  if (m_egl->CreateImage(imageAttrs))
    m_egl->UploadImage(m_textureTarget);

  m_egl->DestroyImage();

  glBindTexture(m_textureTarget, 0);

  return true;
}

void CRenderBufferPRIME::DeleteTexture()
{
  if (glIsTexture(m_textureId))
    glDeleteTextures(1, &m_textureId);

  m_textureId = 0;
}

uint8_t *CRenderBufferPRIME::GetMemory()
{
  return m_bo->GetMemory();
}

void CRenderBufferPRIME::ReleaseMemory()
{
  m_bo->ReleaseMemory();
}
