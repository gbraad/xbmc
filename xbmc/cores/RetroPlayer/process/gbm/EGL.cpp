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

#include "EGL.h"

CEGL::CEGL(EGLDisplay display) :
  m_display(display)
{
  eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
  eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
  glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
}

void CEGL::Configure(int fd, int fourcc, int width, int height, int offset, int stride)
{
  m_attr =
  {
    EGL_WIDTH, width,
    EGL_HEIGHT, height,
    EGL_LINUX_DRM_FOURCC_EXT, fourcc,
    EGL_DMA_BUF_PLANE0_FD_EXT, fd,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, offset,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, stride,
    EGL_NONE
  };
}

void CEGL::CreateImage()
{
  m_image = eglCreateImageKHR(m_display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, m_attr.data());
}

void CEGL::UploadTexture(GLenum textureTarget)
{
  glEGLImageTargetTexture2DOES(textureTarget, m_image);
}

void CEGL::DestroyImage()
{
  eglDestroyImageKHR(m_display, m_image);
}
