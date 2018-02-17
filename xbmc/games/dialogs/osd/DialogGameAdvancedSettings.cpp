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

#include "DialogGameAdvancedSettings.h"
#include "addons/Addon.h"
#include "addons/AddonManager.h"
#include "guilib/WindowIDs.h"
#include "utils/JobManager.h"
#include "Application.h"
#include "ApplicationPlayer.h"
#include "ServiceBroker.h"

using namespace KODI;
using namespace GAME;

CDialogGameAdvancedSettings::CDialogGameAdvancedSettings()
{
  // Initialize CGUIWindow via CGUIDialog
  SetID(WINDOW_DIALOG_GAME_ADVANCED_SETTINGS);
  m_loadType = KEEP_IN_MEMORY;
}

void CDialogGameAdvancedSettings::OnInitWindow()
{
  CGUIDialogAddonSettings::OnInitWindow();

  //! @todo Allow setting this via window param
  std::string gameClientId = GetGameClientID();

  // Initialize CGUIDialogAddonSettings
  m_addon.reset();
  CServiceBroker::GetAddonMgr().GetAddon(gameClientId, m_addon, ADDON::ADDON_GAMEDLL);
  m_saveToDisk = true;
}

void CDialogGameAdvancedSettings::OnDeinitWindow(int nextWindowID)
{
  CGUIDialogAddonSettings::OnDeinitWindow(nextWindowID);

  ADDON::AddonPtr addon = m_addon;
  CJobManager::GetInstance().Submit([addon]()
    {
      addon->SaveSettings();
    });
}

std::string CDialogGameAdvancedSettings::GetGameClientID()
{
  return g_application.GetAppPlayer().GetGameAddon();
}
