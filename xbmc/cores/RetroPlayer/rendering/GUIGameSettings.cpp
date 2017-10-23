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

#include "GUIGameSettings.h"
#include "cores/RetroPlayer/process/RPProcessInfo.h"
#include "cores/RetroPlayer/rendering/RenderContext.h"
#include "cores/GameSettings.h"
#include "threads/SingleLock.h"

using namespace KODI;
using namespace RETRO;

CGUIGameSettings::CGUIGameSettings(CRPProcessInfo &processInfo) :
  m_processInfo(processInfo)
{
  UpdateSettings();
}

CGUIGameSettings::~CGUIGameSettings() = default;

CRenderSettings CGUIGameSettings::GetSettings() const
{
  CSingleLock lock(m_mutex);

  return m_renderSettings;
}

void CGUIGameSettings::UpdateSettings()
{
  CSingleLock lock(m_mutex);

  // Get settings from GUI

  // Reset game settings
  CGameSettings guiSettings = m_processInfo.GetRenderContext().GetDefaultGameSettings();

  SCALINGMETHOD scalingMethod = guiSettings.ScalingMethod();
  VIEWMODE viewMode = guiSettings.ViewMode();

  // Save settings for renderer
  m_renderSettings.VideoSettings().SetScalingMethod(scalingMethod);
  m_renderSettings.VideoSettings().SetRenderViewMode(viewMode);
}
