/*
 *      Copyright (C) 2014-2017 Team Kodi
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

#include "libXBMC_addon.h"
#include "kodi_game_types.h"

#include <string>
#include <stdio.h>

#if defined(ANDROID)
  #include <sys/stat.h>
#endif

class CHelper_libKODI_game
{
public:
  CHelper_libKODI_game(void) :
    m_handle(nullptr),
    m_callbacks(nullptr)
  {
  }

  ~CHelper_libKODI_game(void)
  {
  }

  /*!
    * @brief Resolve all callback methods
    * @param handle Pointer to the add-on
    * @return True when all methods were resolved, false otherwise.
    */
  bool RegisterMe(void* handle)
  {
    m_handle = static_cast<AddonCB*>(handle);
    if (m_handle)
      m_callbacks = (AddonInstance_Game*)m_handle->GameLib_RegisterMe(m_handle->addonData);
    if (!m_callbacks)
      fprintf(stderr, "libKODI_game-ERROR: GameLib_RegisterMe can't get callback table from Kodi !!!\n");

    return m_callbacks != nullptr;
  }

  // --- Game callbacks --------------------------------------------------------

  /*!
   * \brief Requests the frontend to stop the current game
   */
  void CloseGame(void)
  {
    m_callbacks->toKodi.CloseGame(m_callbacks->toKodi.kodiInstance);
  }

  void* OpenStream(const game_stream_properties &properties)
  {
    return m_callbacks->toKodi.OpenStream(m_callbacks->toKodi.kodiInstance, &properties);
  }

  bool GetStreamBuffer(void *stream, unsigned int width, unsigned int height, game_stream_buffer &buffer)
  {
    return m_callbacks->toKodi.GetStreamBuffer(m_callbacks->toKodi.kodiInstance, stream, width, height, &buffer);
  }

  void AddStreamData(void *stream, const game_stream_packet &packet)
  {
    m_callbacks->toKodi.AddStreamData(m_callbacks->toKodi.kodiInstance, stream, &packet);
  }

  void ReleaseStreamBuffer(void *stream, game_stream_buffer &buffer)
  {
    m_callbacks->toKodi.ReleaseStreamBuffer(m_callbacks->toKodi.kodiInstance, stream, &buffer);
  }

  void CloseStream(void *stream)
  {
    m_callbacks->toKodi.CloseStream(m_callbacks->toKodi.kodiInstance, stream);
  }

  // -- Hardware rendering callbacks -------------------------------------------

  /*!
   * \brief Get a symbol from the hardware context
   *
   * \param symbol The symbol's name
   *
   * \return A function pointer for the specified symbol
   */
  game_proc_address_t HwGetProcAddress(const char* sym)
  {
    return m_callbacks->toKodi.HwGetProcAddress(m_callbacks->toKodi.kodiInstance, sym);
  }

  // --- Input callbacks -------------------------------------------------------

  /*!
  * \brief Notify the port of an input event
  *
  * \param event The input event
  *
  * Input events can arrive for the following sources:
  *   - GAME_INPUT_EVENT_MOTOR
  *
  * \return true if the event was handled, false otherwise
  */
  bool InputEvent(const game_input_event& event)
  {
    return m_callbacks->toKodi.InputEvent(m_callbacks->toKodi.kodiInstance, &event);
  }

private:
  AddonCB* m_handle;
  AddonInstance_Game* m_callbacks;
};
