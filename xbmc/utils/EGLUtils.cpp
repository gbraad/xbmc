/*
 *      Copyright (C) 2017 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "EGLUtils.h"
#include "log.h"

std::set<std::string> CEGLUtils::GetClientExtensions()
{
  const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
  if (!extensions)
  {
    throw std::runtime_error("Could not query EGL for client extensions, maybe EGL_EXT_client_extensions is not supported?");
  }
  std::set<std::string> result;
  StringUtils::SplitTo(std::inserter(result, result.begin()), extensions, " ");
  return result;
}

void CEGLUtils::LogError(const std::string& what)
{
  CLog::Log(LOGERROR, "%s (EGL error %d)", what.c_str(), eglGetError());
}

/* --- CEGLImage -------------------------------------------*/

CEGLImage::CEGLImage(EGLDisplay display) :
  m_display(display)
{
  eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
  eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
  glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
}

bool CEGLImage::CreateImage(eglAttrs imageAttrs)
{
  std::vector<EGLint> attr =
  {
    EGL_WIDTH, imageAttrs.width,
    EGL_HEIGHT, imageAttrs.height,
    EGL_LINUX_DRM_FOURCC_EXT, imageAttrs.fourcc,
  };

  for (int i = 0; i < imageAttrs.num_planes; i++)
  {
    attr.emplace_back(egl_dmabuf_plane_fd_attr[i]);
    attr.emplace_back(imageAttrs.fd[i]);
    attr.emplace_back(egl_dmabuf_plane_offset_attr[i]);
    attr.emplace_back(imageAttrs.offset[i]);
    attr.emplace_back(egl_dmabuf_plane_pitch_attr[i]);
    attr.emplace_back(imageAttrs.stride[i]);
  }

  attr.emplace_back(EGL_NONE);

  m_image = eglCreateImageKHR(m_display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attr.data());

  if(m_image)
    return true;

  return false;
}

void CEGLImage::UploadImage(GLenum textureTarget)
{
  GLint format;
  glEGLImageTargetTexture2DOES(textureTarget, m_image);
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
  CLog::Log(LOGERROR, "GL Format %d", format);
}

void CEGLImage::DestroyImage()
{
  eglDestroyImageKHR(m_display, m_image);
}
