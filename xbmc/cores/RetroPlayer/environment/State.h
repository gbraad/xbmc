/*
 *      Copyright (C) 2017-2018 Team Kodi
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

#include <stdint.h>

namespace KODI
{
namespace RETRO
{
  enum class VideoFormat
  {
    FORMAT_0RGB8888,
    FORMAT_RGB565,
    FORMAT_0RGB1555,
  };

  struct VideoState
  {
    const VideoFormat format;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int stride = 0;
    unsigned int orientationDeg = 0; // Counter clockwise
  };

  class CVideoState
  {
  public:
    CVideoState();

    VideoFormat Format() const { return m_format; }
    unsigned int Width() const { return m_videoWidth; }
    unsigned int Height() const { return m_videoHeight; }
    unsigned int Stride() const { return m_videoStride; }
    const uint8_t *Data() const { return m_pixels; }
    unsigned int

    void SetFormat(VideoFormat format) { m_format = format; }
    void SetWidth(unsigned int width) { m_videoWidth = width; }
    void SetHeight(unsigned int height) { m_videoHeight = height; }

    void RegisterVideoReceiver();
    void UnregisterVideoReceiver();

  private:
    const VideoFormat m_format;
    unsigned int m_videoWidth = 0;
    unsigned int m_videoHeight = 0;
    unsigned int m_videoStride = 0;
    const uint8_t *m_pixels = nullptr;
  };

  class CState
  {
  public:
    CState();

  private:
    uint64_t m_timestamp;
    CVideoState m_video;
    //CAudioState m_audio;
    //CMemoryState m_memory;
    //CFeedbackState m_feedback;
  };
}
}
