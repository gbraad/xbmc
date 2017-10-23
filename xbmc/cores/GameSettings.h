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

namespace KODI
{
namespace RETRO
{

enum class SCALINGMETHOD
{
  AUTO,
  NEAREST,
  LINEAR,
};

enum class ViewMode
{
  Normal,
  Stretch4x3,
  Stretch16x9,
  Stretch16x9Nonlin,
  Original,
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

  enum ViewMode ViewMode() const { return m_viewMode; }
  void SetViewMode(enum ViewMode viewMode);

private:
  // Render settings
  SCALINGMETHOD m_scalingMethod;
  ViewMode m_viewMode;
};

class CCriticalSection;
class CGameSettingsLocked
{
public:
  CGameSettingsLocked(CGameSettings &gs, CCriticalSection &critSection);
  virtual ~CGameSettingsLocked() = default;

  CGameSettingsLocked(CGameSettingsLocked const &other) = delete;
  void operator=(CGameSettingsLocked const &rhs) = delete;

  void SetScalingMethod(SCALINGMETHOD scalingMethod);
  void SetViewMode(enum ViewMode viewMode);

protected:
  CGameSettings &m_gameSettings;
  CCriticalSection &m_critSection;
};

}
}
