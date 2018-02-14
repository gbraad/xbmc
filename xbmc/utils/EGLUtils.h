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
#pragma once

#include <set>
#include <string>
#include <stdexcept>
#include <vector>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "system_gl.h"

#include "StringUtils.h"

class CEGLUtils
{
public:
  static std::set<std::string> GetClientExtensions();
  static void LogError(std::string const & what);
  template<typename T>
  static T GetRequiredProcAddress(const char * procname)
  {
    T p = reinterpret_cast<T>(eglGetProcAddress(procname));
    if (!p)
    {
      throw std::runtime_error(std::string("Could not get EGL function \"") + procname + "\" - maybe a required extension is not supported?");
    }
    return p;
  }

private:
  CEGLUtils();
};

/* --- CEGLImage ---------------------------------------------*/

#define MAX_NUM_PLANES 3

static const EGLint egl_dmabuf_plane_fd_attr[MAX_NUM_PLANES] =
{
  EGL_DMA_BUF_PLANE0_FD_EXT,
  EGL_DMA_BUF_PLANE1_FD_EXT,
  EGL_DMA_BUF_PLANE2_FD_EXT,
};

static const EGLint egl_dmabuf_plane_offset_attr[MAX_NUM_PLANES] =
{
  EGL_DMA_BUF_PLANE0_OFFSET_EXT,
  EGL_DMA_BUF_PLANE1_OFFSET_EXT,
  EGL_DMA_BUF_PLANE2_OFFSET_EXT,
};

static const EGLint egl_dmabuf_plane_pitch_attr[MAX_NUM_PLANES] =
{
  EGL_DMA_BUF_PLANE0_PITCH_EXT,
  EGL_DMA_BUF_PLANE1_PITCH_EXT,
  EGL_DMA_BUF_PLANE2_PITCH_EXT,
};

struct eglAttrs
{
  int width;
  int height;
  int fourcc;
  int num_planes;
  int fd[MAX_NUM_PLANES];
  int offset[MAX_NUM_PLANES];
  int stride[MAX_NUM_PLANES];
};

class CEGLImage
{
public:
  CEGLImage(EGLDisplay display);
  ~CEGLImage() = default;

  //void Configure(int fd, int fourcc, int width, int height, int offset, int stride);
  bool CreateImage(eglAttrs imageAttrs);
  void UploadImage(GLenum textureTarget);
  void DestroyImage();

private:
  EGLDisplay m_display = nullptr;
  EGLImage m_image = nullptr;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = nullptr;
};
