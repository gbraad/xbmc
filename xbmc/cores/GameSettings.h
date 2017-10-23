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

class CCriticalSection;

namespace KODI
{
namespace RETRO
{

// NOTE: Only append
enum class SCALINGMETHOD
{
  AUTO = 0,
  NEAREST = 1,
  LINEAR = 2,
  MAX = LINEAR
};

// NOTE: Only append
enum class VIEWMODE
{
  Normal = 0,
  Stretch4x3 = 1,
  Stretch16x9 = 2,
  Stretch16x9Nonlin = 3,
  Original = 4,
  Max = Original
};

class CGameSettings
{
public:
  CGameSettings() { Reset(); }
  ~CGameSettings() = default;

  CGameSettings &operator=(const CGameSettings &rhs);

  // Restore game settings to default
  void Reset();

  bool operator==(const CGameSettings &rhs) const;
  bool operator!=(const CGameSettings &rhs) const { return !(*this == rhs); }

  SCALINGMETHOD ScalingMethod() const { return m_scalingMethod; }
  void SetScalingMethod(SCALINGMETHOD scalingMethod);

  VIEWMODE ViewMode() const { return m_viewMode; }
  void SetViewMode(VIEWMODE viewMode);

private:
  // Render settings
  SCALINGMETHOD m_scalingMethod;
  VIEWMODE m_viewMode;
};

class CGameSettingsLocked
{
public:
  CGameSettingsLocked(CGameSettings &gs, CCriticalSection &critSection);
  virtual ~CGameSettingsLocked() = default;

  CGameSettingsLocked(CGameSettingsLocked const &other) = delete;
  void operator=(CGameSettingsLocked const &rhs) = delete;

  void SetScalingMethod(SCALINGMETHOD scalingMethod);
  void SetViewMode(VIEWMODE viewMode);

protected:
  CGameSettings &m_gameSettings;
  CCriticalSection &m_critSection;
};

}
}
