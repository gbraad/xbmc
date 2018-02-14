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

#include "system_gl.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <vector>

class CEGL
{
public:
  CEGL(EGLDisplay display);
  ~CEGL() = default;

  void Configure(int fd, int fourcc, int width, int height, int offset, int stride);
  void CreateImage();
  void UploadTexture(GLenum textureTarget);
  void DestroyImage();

private:
  EGLDisplay m_display;
  EGLImage m_image;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;

  std::vector<EGLint> m_attr;
};
