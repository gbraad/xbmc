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

#include "RenderBufferGbm.h"
#include "ServiceBroker.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

using namespace KODI;
using namespace RETRO;

CRenderBufferGbm::CRenderBufferGbm(CRenderContext &context,
                                   int fourcc,
                                   int bpp) :
  m_context(context),
  m_fourcc(fourcc),
  m_bpp(bpp)
{
  m_winSystem = dynamic_cast<CWinSystemGbmGLESContext*>(&CServiceBroker::GetWinSystem());
}

CRenderBufferGbm::~CRenderBufferGbm()
{
  DeleteTexture();

  if (m_bo)
    gbm_bo_destroy(m_bo);
}

bool CRenderBufferGbm::Allocate(AVPixelFormat format, unsigned int width, unsigned int height, unsigned int size)
{
  // Initialize IRenderBuffer
  m_format = format;
  m_width = width;
  m_height = height;

  m_bo = gbm_bo_create(m_winSystem->GetGBMDevice(), m_width, m_height, m_fourcc, GBM_BO_USE_LINEAR);

  if (!m_bo)
    return false;

  m_fd = gbm_bo_get_fd(m_bo);

  return true;
}

void CRenderBufferGbm::CreateTexture()
{
  glGenTextures(1, &m_textureId);

  glBindTexture(m_textureTarget, m_textureId);

  glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(m_textureTarget, 0);
}

bool CRenderBufferGbm::UploadTexture()
{
  if (!glIsTexture(m_textureId))
    CreateTexture();

  if (m_fd == -1)
    return false;

  glBindTexture(m_textureTarget, m_textureId);

  const EGLint attr[] =
  {
    EGL_WIDTH, (EGLint)m_width,
    EGL_HEIGHT, (EGLint)m_height,
    EGL_LINUX_DRM_FOURCC_EXT, m_fourcc,
    EGL_DMA_BUF_PLANE0_FD_EXT, m_fd,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, (EGLint)m_stride,
    EGL_NONE
  };

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

  auto img = eglCreateImageKHR(m_winSystem->GetEGLDisplay(), EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attr);

  glEGLImageTargetTexture2DOES(m_textureTarget, img);

  eglDestroyImageKHR(m_winSystem->GetEGLDisplay(), img);

  glBindTexture(m_textureTarget, 0);

  return true;
}

void CRenderBufferGbm::DeleteTexture()
{
  if (glIsTexture(m_textureId))
    glDeleteTextures(1, &m_textureId);

  m_textureId = 0;
}

uint8_t *CRenderBufferGbm::GetMemory()
{
  if (m_bo)
  {
    m_map = static_cast<uint8_t*>(gbm_bo_map(m_bo, 0, 0, m_width, m_height, GBM_BO_TRANSFER_WRITE, &m_stride, &m_map_data));
    if (m_map)
      return m_map;
  }

  return nullptr;
}

void CRenderBufferGbm::ReleaseMemory()
{
  if (m_bo && m_map)
  {
    gbm_bo_unmap(m_bo, m_map_data);
    m_map = nullptr;
    m_map_data = nullptr;
  }
}
