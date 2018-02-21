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

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <stddef.h>

namespace KODI
{
namespace RETRO
{
  struct Requirement;
  using RequirementCallback = std::function<void(std::unique_ptr<Requirement>, bool)>;

  using Requirements = std::deque<std::unique_ptr<Requirement>>;
  using RequirementsMap = std::map<Requirement*, size_t>;

  struct Args;
  using Runnable = std::function<bool(std::unique_ptr<Args>)>;
  using RunnableCallback = std::function<void(Runnable, bool)>;

  struct Task;
  using TaskCallback = std::function<void(std::shared_ptr<Task>, bool)>;
}
}
