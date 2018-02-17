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

#include "GBMUtils.h"
#include "utils/log.h"
#include "windowing/gbm/WinSystemGbmGLESContext.h"
#include "ServiceBroker.h"

bool CGBMUtils::CreateDevice(int fd)
{
  if (m_device)
    CLog::Log(LOGWARNING, "CGBMUtils::%s - device already created", __FUNCTION__);

  m_device = gbm_create_device(fd);
  if (!m_device)
  {
    CLog::Log(LOGERROR, "CGBMUtils::%s - failed to create device", __FUNCTION__);
    return false;
  }

  return true;
}

void CGBMUtils::DestroyDevice()
{
  if (!m_device)
    CLog::Log(LOGWARNING, "CGBMUtils::%s - device already destroyed", __FUNCTION__);

  if (m_device)
  {
    gbm_device_destroy(m_device);
    m_device = nullptr;
  }
}

bool CGBMUtils::CreateSurface(int width, int height)
{
  if (m_surface)
    CLog::Log(LOGWARNING, "CGBMUtils::%s - surface already created", __FUNCTION__);

  m_surface = gbm_surface_create(m_device,
                                 width,
                                 height,
                                 GBM_FORMAT_ARGB8888,
                                 GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

  if (!m_surface)
  {
    CLog::Log(LOGERROR, "CGBMUtils::%s - failed to create surface", __FUNCTION__);
    return false;
  }

  CLog::Log(LOGDEBUG, "CGBMUtils::%s - created surface with size %dx%d", __FUNCTION__,
                                                                         width,
                                                                         height);

  return true;
}

void CGBMUtils::DestroySurface()
{
  if (!m_surface)
    CLog::Log(LOGWARNING, "CGBMUtils::%s - surface already destroyed", __FUNCTION__);

  if (m_surface)
  {
    ReleaseBuffer();

    gbm_surface_destroy(m_surface);
    m_surface = nullptr;
  }
}

struct gbm_bo *CGBMUtils::LockFrontBuffer()
{
  if (m_next_bo)
    CLog::Log(LOGWARNING, "CGBMUtils::%s - uneven surface buffer usage", __FUNCTION__);

  m_next_bo = gbm_surface_lock_front_buffer(m_surface);
  return m_next_bo;
}

void CGBMUtils::ReleaseBuffer()
{
  if (m_bo)
    gbm_surface_release_buffer(m_surface, m_bo);

  m_bo = m_next_bo;
  m_next_bo = nullptr;
}

/* --- CGBMBufferObject ----------------------------------------------------*/

CGBMBufferObject::CGBMBufferObject(int format) :
  m_format(format)
{
  m_device = dynamic_cast<CWinSystemGbmGLESContext*>(&CServiceBroker::GetWinSystem())->GetGBMDevice();
}

CGBMBufferObject::~CGBMBufferObject()
{
  ReleaseMemory();
  DestroyBufferObject();
}

bool CGBMBufferObject::CreateBufferObject(int width, int height)
{
  m_width = width;
  m_height = height;

  m_bo = gbm_bo_create(m_device, m_width, m_height, m_format, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR);

  if (!m_bo)
    return false;

  m_fd = gbm_bo_get_fd(m_bo);

  return true;
}

void CGBMBufferObject::DestroyBufferObject()
{
  if (m_bo)
    gbm_bo_destroy(m_bo);
}

uint8_t* CGBMBufferObject::GetMemory()
{
  if (m_bo)
  {
    m_map = static_cast<uint8_t*>(gbm_bo_map(m_bo, 0, 0, m_width, m_height, GBM_BO_TRANSFER_WRITE, &m_stride, &m_map_data));
    if (m_map)
      return m_map;
  }

  return nullptr;
}

void CGBMBufferObject::ReleaseMemory()
{
  if (m_bo && m_map)
  {
    gbm_bo_unmap(m_bo, m_map_data);
    m_map_data = nullptr;
    m_map = nullptr;
  }
}

int CGBMBufferObject::GetFd()
{
  return m_fd;
}

int CGBMBufferObject::GetStride()
{
  return m_stride;
}
