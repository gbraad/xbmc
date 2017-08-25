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

#include "DialogGameSaves.h"
#include "guilib/GUIMessage.h"
#include "guilib/WindowIDs.h"

using namespace KODI;
using namespace GAME;

CDialogGameSaves::CDialogGameSaves(void) :
  CGUIDialog(WINDOW_DIALOG_GAME_SAVES, "DialogGameSaves.xml")
{
  // initialize CGUIWindow
  m_loadType = KEEP_IN_MEMORY;
}

bool CDialogGameSaves::OnMessage(CGUIMessage& message)
{
  return CGUIDialog::OnMessage(message);
}

void CDialogGameSaves::OnInitWindow(void)
{
  return CGUIDialog::OnInitWindow();
}

void CDialogGameSaves::OnDeinitWindow(int nextWindowID)
{
  return CGUIDialog::OnDeinitWindow(nextWindowID);
}
