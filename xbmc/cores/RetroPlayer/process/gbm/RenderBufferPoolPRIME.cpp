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

#include "RenderBufferPoolPRIME.h"
#include "RenderBufferPRIME.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererOpenGLES.h"

#include "drm_fourcc.h"

using namespace KODI;
using namespace RETRO;

CRenderBufferPoolPRIME::CRenderBufferPoolPRIME(CRenderContext &context)
  : m_context(context)
{
}

bool CRenderBufferPoolPRIME::IsCompatible(const CRenderVideoSettings &renderSettings) const
{
  if (!CRPRendererOpenGLES::SupportsScalingMethod(renderSettings.GetScalingMethod()))
    return false;

  return true;
}


IRenderBuffer *CRenderBufferPoolPRIME::CreateRenderBuffer(void *header /* = nullptr */)
{
  return new CRenderBufferPRIME(m_context,
                                m_fourcc,
                                m_bpp);
}

bool CRenderBufferPoolPRIME::ConfigureInternal()
{
  switch (m_format)
  {
    case AV_PIX_FMT_0RGB32:
    {
      m_fourcc = DRM_FORMAT_ARGB8888;
      m_bpp = sizeof(uint32_t);
      return true;
    }
    case AV_PIX_FMT_RGB555:
    {
      m_fourcc = DRM_FORMAT_RGBA5551;
      m_bpp = sizeof(uint16_t);
      return true;
    }
    case AV_PIX_FMT_RGB565:
    {
      m_fourcc = DRM_FORMAT_RGB565;
      m_bpp = sizeof(uint16_t);
      return true;
    }
    default:
      break; // we shouldn't even get this far if we are given an unsupported pixel format
  }

  return false;
}
