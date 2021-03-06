/*
 *      Copyright (C) 2012-2017 Team Kodi
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

#include "RetroPlayerVideo.h"
#include "cores/RetroPlayer/process/RPProcessInfo.h"
#include "cores/RetroPlayer/rendering/RenderTranslator.h"
#include "cores/RetroPlayer/rendering/RPRenderManager.h"
#include "utils/log.h"

using namespace KODI;
using namespace RETRO;

CRetroPlayerVideo::CRetroPlayerVideo(CRPRenderManager& renderManager, CRPProcessInfo& processInfo) :
  m_renderManager(renderManager),
  m_processInfo(processInfo)
{
  CLog::Log(LOGDEBUG, "RetroPlayer[VIDEO]: Initializing video");

  m_renderManager.Initialize();
}

CRetroPlayerVideo::~CRetroPlayerVideo()
{
  CLog::Log(LOGDEBUG, "RetroPlayer[VIDEO]: Deinitializing video");

  CloseStream();
  m_renderManager.Deinitialize();
}

bool CRetroPlayerVideo::OpenStream(AVPixelFormat pixfmt, unsigned int nominalWidth, unsigned int nominalHeight, unsigned int maxWidth, unsigned int maxHeight, float aspectRatio)
{
  CLog::Log(LOGDEBUG, "RetroPlayer[VIDEO]: Creating video stream - format %s, nominal %ux%u, max %ux%u",
      CRenderTranslator::TranslatePixelFormat(pixfmt),
      nominalWidth,
      nominalHeight,
      maxWidth,
      maxHeight);

  m_processInfo.SetVideoPixelFormat(pixfmt);
  m_processInfo.SetVideoDimensions(nominalWidth, nominalHeight); // Report nominal height for now

  return m_renderManager.Configure(pixfmt, nominalWidth, nominalHeight, maxWidth, maxHeight);
}

void CRetroPlayerVideo::AddData(const uint8_t* data, unsigned int size, unsigned int width, unsigned int height, unsigned int orientationDegCCW)
{
  m_renderManager.AddFrame(data, size, width, height, orientationDegCCW);
}

void CRetroPlayerVideo::CloseStream()
{
  CLog::Log(LOGDEBUG, "RetroPlayer[VIDEO]: Closing video stream");

  m_renderManager.Flush();
}
