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

#include "GameSettings.h"
#include "threads/CriticalSection.h"
#include "threads/SingleLock.h"

using namespace KODI;
using namespace RETRO;

//------------------------------------------------------------------------------
// CGameSettings
//------------------------------------------------------------------------------

CGameSettings &CGameSettings::operator=(const CGameSettings &rhs)
{
  if (this != &rhs)
  {
    m_scalingMethod = rhs.m_scalingMethod;
    m_viewMode = rhs.m_viewMode;
  }
  return *this;
}

void CGameSettings::Reset()
{
  m_scalingMethod = SCALINGMETHOD::AUTO;
  m_viewMode = ViewMode::Normal;
}

bool CGameSettings::operator==(const CGameSettings &rhs) const
{
  return m_scalingMethod == rhs.m_scalingMethod &&
         m_viewMode == rhs.m_viewMode;
}

//------------------------------------------------------------------------------
// CGameSettingsLocked
//------------------------------------------------------------------------------

CGameSettingsLocked::CGameSettingsLocked(CGameSettings &gs, CCriticalSection &critSection) :
  m_gameSettings(gs), m_critSection(critSection)
{
}

void CGameSettingsLocked::SetScalingMethod(SCALINGMETHOD scalingMethod)
{
  CSingleLock lock(m_critSection);
  m_gameSettings.SetScalingMethod(scalingMethod);
}

void CGameSettingsLocked::SetViewMode(ViewMode viewMode)
{
  CSingleLock lock(m_critSection);
  m_gameSettings.SetViewMode(viewMode);
}
