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

#include <memory>
#include <stdint.h>

namespace KODI
{
namespace GAME
{
  class CGameClient;
}

namespace RETRO
{
  /*!
   * \brief Video, audio, memory and haptics for a timeframe
   */
  class CState;

  /*!
   * \brief Combined state of all controllers and input for a timeframe
   */
  class CAction;

  /*!
   * \brief Directed goal set by an observer to allow NLP control of an agent
   */
  class CGoal;

  /*!
   * \brief Scores and achievements
   */
  class CReward;


  /*!
   * \brief class Environment transitions
   */
  class CEnvironment
  {
  public:
    static void UpdateState(        CState& state, const CReward& reward, const CGoal& goal, const CAction& action );
    static void UpdateReward( const CState& state,       CReward& reward, const CGoal& goal, const CAction& action );
    static void UpdateGoal(   const CState& state, const CReward& reward,       CGoal& goal, const CAction& action );
    static void UpdateAction( const CState& state, const CReward& reward, const CGoal& goal,       CAction& action );

    // Environment variables
    std::unique_ptr<CState> m_state;
    std::unique_ptr<CReward> m_reward;
    std::unique_ptr<CGoal> m_goal;
    std::unique_ptr<CAction> m_action;
  };
}
}
