/*
 *      Copyright (C) 2018 Team Kodi
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

#include <stddef.h>
#include <stdint.h>

/// @name Audio stream
///{
typedef enum GAME_PCM_FORMAT
{
  GAME_PCM_FORMAT_UNKNOWN,
  GAME_PCM_FORMAT_S16NE,
} GAME_PCM_FORMAT;

typedef enum GAME_AUDIO_CHANNEL
{
  GAME_CH_NULL, // Channel list terminator
  GAME_CH_FL,
  GAME_CH_FR,
  GAME_CH_FC,
  GAME_CH_LFE,
  GAME_CH_BL,
  GAME_CH_BR,
  GAME_CH_FLOC,
  GAME_CH_FROC,
  GAME_CH_BC,
  GAME_CH_SL,
  GAME_CH_SR,
  GAME_CH_TFL,
  GAME_CH_TFR,
  GAME_CH_TFC,
  GAME_CH_TC,
  GAME_CH_TBL,
  GAME_CH_TBR,
  GAME_CH_TBC,
  GAME_CH_BLOC,
  GAME_CH_BROC,
} GAME_AUDIO_CHANNEL;

typedef struct game_stream_audio_properties
{
  GAME_PCM_FORMAT format;
  const GAME_AUDIO_CHANNEL* channel_map;
} game_stream_audio_properties;

typedef struct game_stream_audio_packet
{
  const uint8_t *data;
  size_t size;
} game_stream_audio_packet;
///}

/// @name Video stream
///{
typedef enum GAME_PIXEL_FORMAT
{
  GAME_PIXEL_FORMAT_UNKNOWN,
  GAME_PIXEL_FORMAT_0RGB8888,
  GAME_PIXEL_FORMAT_RGB565,
  GAME_PIXEL_FORMAT_0RGB1555,
} GAME_PIXEL_FORMAT;

typedef enum GAME_VIDEO_ROTATION
{
  GAME_VIDEO_ROTATION_0,
  GAME_VIDEO_ROTATION_90_CCW,
  GAME_VIDEO_ROTATION_180_CCW,
  GAME_VIDEO_ROTATION_270_CCW,
} GAME_VIDEO_ROTATION;

typedef struct game_stream_video_properties
{
  GAME_PIXEL_FORMAT format;
  unsigned int nominal_width;
  unsigned int nominal_height;
  unsigned int max_width;
  unsigned int max_height;
  float aspect_ratio; // If aspect_ratio is <= 0.0, an aspect ratio of nominal_width / nominal_height is assumed
} game_stream_video_properties;

typedef struct game_stream_video_packet
{
  unsigned int width;
  unsigned int height;
  GAME_VIDEO_ROTATION rotation;
  const uint8_t *data;
  size_t size;
} game_stream_video_packet;
///}

/// @name Hardware framebuffer stream
///{
typedef enum GAME_HW_CONTEXT_TYPE
{
  GAME_HW_CONTEXT_NONE,

  // OpenGL 2.x. Driver can choose to use latest compatibility context
  GAME_HW_CONTEXT_OPENGL,

  // OpenGL ES 2.0
  GAME_HW_CONTEXT_OPENGLES2,

  // Modern desktop core GL context. Use major/minor fields to set GL version
  GAME_HW_CONTEXT_OPENGL_CORE,

  // OpenGL ES 3.0
  GAME_HW_CONTEXT_OPENGLES3,

  // OpenGL ES 3.1+. Set major/minor fields.
  GAME_HW_CONTEXT_OPENGLES_VERSION,

  // Vulkan
  GAME_HW_CONTEXT_VULKAN
} GAME_HW_CONTEXT_TYPE;

typedef struct game_stream_hw_framebuffer_properties
{
  /*!
   * The API to use.
   */
  GAME_HW_CONTEXT_TYPE context_type;

  /*!
   * Set if render buffers should have depth component attached.
   *
   * TODO: Obsolete
   */
  bool depth;

  /*!
   * Set if stencil buffers should be attached. If depth and stencil are true,
   * a packed 24/8 buffer will be added. Only attaching stencil is invalid and
   * will be ignored.
   *
   * TODO: Obsolete.
   */
  bool stencil;

  /*!
   * Use conventional bottom-left origin convention. If false, standard top-left
   * origin semantics are used.
   *
   * TODO: Move to GL specific interface
   */
  bool bottom_left_origin;

  /*!
   * Major version number for core GL context or GLES 3.1+.
   */
  unsigned int version_major;

  /*!
   * Minor version number for core GL context or GLES 3.1+.
   */
  unsigned int version_minor;

  /*!
   * If this is true, the frontend will go very far to avoid resetting context
   * in scenarios like toggling fullscreen, etc.
   *
   * TODO: Obsolete? Maybe frontend should just always assume this...
   *
   * The reset callback might still be called in extreme situations such as if
   * the context is lost beyond recovery.
   *
   * For optimal stability, set this to false, and allow context to be reset at
   * any time.
   */
  bool cache_context;

  /*!
   * Creates a debug context.
   */
  bool debug_context;
} game_stream_hw_framebuffer_properties;

typedef struct game_stream_hw_framebuffer_buffer
{
  uintptr_t framebuffer;
} game_stream_hw_framebuffer_buffer;

typedef struct game_stream_hw_framebuffer_packet
{
  uintptr_t framebuffer;
} game_stream_hw_framebuffer_packet;

typedef void (*game_proc_address_t)(void);
///}

/// @name Software framebuffer stream
///{
typedef game_stream_video_properties game_stream_sw_framebuffer_properties;

typedef struct game_stream_sw_framebuffer_buffer
{
  GAME_PIXEL_FORMAT format;
  uint8_t *data;
  size_t size;
} game_stream_sw_framebuffer_buffer;

typedef game_stream_video_packet game_stream_sw_framebuffer_packet;
///}

/// @name Stream types
///{
typedef enum GAME_STREAM_TYPE
{
  GAME_STREAM_UNKNOWN,
  GAME_STREAM_AUDIO,
  GAME_STREAM_VIDEO,
  GAME_STREAM_HW_FRAMEBUFFER,
  GAME_STREAM_SW_FRAMEBUFFER,
} GAME_STREAM_TYPE;

/*!
 * \brief Immutable stream metadata
 *
 * This metadata is provided when the stream is opened. If any stream
 * properties change, a new stream must be opened.
 */
typedef struct game_stream_properties
{
  GAME_STREAM_TYPE type;
  union
  {
    game_stream_audio_properties audio;
    game_stream_video_properties video;
    game_stream_hw_framebuffer_properties hw_framebuffer;
    game_stream_sw_framebuffer_properties sw_framebuffer;
  };
} game_stream_properties;

/*!
 * \brief Stream buffers for hardware rendering and zero-copy support
 */
typedef struct game_stream_buffer
{
  GAME_STREAM_TYPE type;
  union
  {
    game_stream_hw_framebuffer_buffer hw_framebuffer;
    game_stream_sw_framebuffer_buffer sw_framebuffer;
  };
} game_stream_buffer;

/*!
 * \brief Stream packet and ephemeral metadata
 *
 * This packet contains stream data and accompanying metadata. The metadata
 * is ephemeral, meaning it only applies to the current packet and can change
 * from packet to packet in the same stream.
 */
typedef struct game_stream_packet
{
  GAME_STREAM_TYPE type;
  union
  {
    game_stream_audio_packet audio;
    game_stream_video_packet video;
    game_stream_hw_framebuffer_packet hw_framebuffer;
    game_stream_sw_framebuffer_packet sw_framebuffer;
  };
} game_stream_packet;
///}
