/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include "utils/IBufferObject.h"

#include <gbm.h>

class CGBMUtils
{
public:
  CGBMUtils() = default;
  ~CGBMUtils() = default;
  bool CreateDevice(int fd);
  void DestroyDevice();
  bool CreateSurface(int width, int height);
  void DestroySurface();
  struct gbm_bo *LockFrontBuffer();
  void ReleaseBuffer();

  struct gbm_device *m_device = nullptr;
  struct gbm_surface *m_surface = nullptr;

protected:
  struct gbm_bo *m_bo = nullptr;
  struct gbm_bo *m_next_bo = nullptr;
};

class CGBMBufferObject : public IBufferObject
{
public:
  CGBMBufferObject(int format);
  virtual ~CGBMBufferObject() override;

  bool CreateBufferObject(int width, int height) override;
  void DestroyBufferObject() override;
  uint8_t* GetMemory() override;
  void ReleaseMemory() override;
  int GetFd() override;
  int GetStride() override;

private:
  gbm_device *m_device = nullptr;

  int m_format = 0;
  int m_fd = -1;
  uint32_t m_stride = 0;
  uint8_t *m_map = nullptr;
  void *m_map_data = nullptr;
  gbm_bo *m_bo = nullptr;
};
