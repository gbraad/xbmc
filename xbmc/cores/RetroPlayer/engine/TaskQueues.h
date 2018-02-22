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

#include "Types.h"
#include "Tasks.h" //! @todo Move to .cpp
#include "threads/CriticalSection.h"
#include "threads/Event.h"
#include "threads/SingleLock.h" //! @todo Move to .cpp

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string> //! @todo Check if needed
#include <utility>

namespace KODI
{
namespace RETRO
{
  struct TaskResult;
  struct ThreadPool;
  struct DeferredRun;

  //! @todo
  struct Requirement { };

  /*!
   * \brief A placeholder for a tasks requirement which is yet to be filled
   *
   * Internal use only.
   */
  struct NotFilled : public Requirement
  {
  };

  /*!
   * \brief Structure for necessary data to manage a task
   *
   * Internal use only.
   */
  struct TaskData
  {
    TaskData(std::shared_ptr<Task> task, TaskCallback callback) :
      task(std::move(task)),
      callback(callback)
    {
    }

    const std::shared_ptr<Task> task;
    const TaskCallback callback;
    RequirementsMap requirementsMap;
    Requirements requirements;
    Runnable runnable;
  };

  /*!
   * \brief A queue which will manage tasks, and schedule them out on the
   *        thread pool
   *
   * The queue will also manage any requirements of given tasks.
   */
  struct TaskQueue
  {
    TaskQueue(std::unique_ptr<ThreadPool> threadPool) :
      m_threadPool(std::move(threadPool))
    {
    }

    /*!
     * \brief Method to add a task to the queue
     *
     * When it has run, the result or error will be sent to the callback
     * method.
     */
    void AddTask(std::shared_ptr<Task> task, TaskCallback callback)
    {
      CSingleLock lock(m_mutex);

      std::unique_ptr<TaskData> taskData(new TaskData(task, callback));
      //m_taskDataMap.emplace(TaskDataPair{task, std::move(taskData)}); //! @todo

      std::unique_ptr<DeferredRun> rr = task->PreFlight();

      AddRunnable(std::move(task), rr->runnable, std::move(rr->requirements));
    }

  private:
    /*!
     * \brief Method which schedules a runnable and its requirement
     *
     * Internal use only.
     */
    void AddRunnable(std::shared_ptr<Task> owner, Runnable runnable, Requirements requirements)
    {
      CSingleLock lock(m_mutex);

      m_runnableOwnerMap[runnable] = owner;

      auto it = m_taskDataMap.find(owner);
      if (it != m_taskDataMap.end())
      {
        TaskData &taskData = *it->second;

        taskData.runnable = runnable;
        taskData.requirementsMap.clear();
        taskData.requirements.clear();

        for (std::unique_ptr<Requirement> &requirement : requirements)
        {
          taskData.requirementsMap[requirement.get()] = taskData.requirements.size();
          taskData.requirements.emplace_back(new NotFilled);
          m_requirementsOwnerMap[requirement.get()] = owner;

          RequirementCallback callback = [this](std::unique_ptr<Requirement> requirement, std::unique_ptr<TaskResult> result)
            {
              OnRequirementDone(std::move(requirement), std::move(result));
            };

          // AddTask(std::move(requirement), callback); //! @todo
        }
      }
    }

    /*!
     * \brief Will be called when a runnable is done
     *
     * Will finish the task if it has a result or nothing. If the runnable's
     * result is a deferedrun object its requirements and runnable will be
     * scheduled out.
     *
     * Internal use only.
     */
    void OnRunnableDone(Runnable runnable, std::unique_ptr<TaskResult> result)
    {
      CSingleLock lock(m_mutex);

      auto it = m_runnableOwnerMap.find(runnable);
      if (it != m_runnableOwnerMap.end())
      {
        std::shared_ptr<Task> &owner = it->second;

        auto it2 = m_taskDataMap.find(owner);
        if (it2 != m_taskDataMap.end())
        {
          std::unique_ptr<TaskData> &taskData = it2->second;

          const DeferredRun* deferredRun = dynamic_cast<const DeferredRun*>(result.get());

          if (deferredRun != nullptr)
            AddRunnable(owner, deferredRun->runnable, std::move(deferredRun->requirements));
          else
            OnTaskDone(taskData->task, std::move(result));
        }
      }
    }

    /*!
     * \brief Will be called when a task is done
     *
     * Internal use only.
     */
    void OnTaskDone(std::shared_ptr<Task> task, std::unique_ptr<TaskResult> result)
    {
      CSingleLock lock(m_mutex);

      auto it = m_taskDataMap.find(task);
      if (it != m_taskDataMap.end())
      {
        std::unique_ptr<TaskData> &taskData = it->second;

        taskData->callback(std::move(task), std::move(result));

        m_taskDataMap.erase(it);
      }
    }

    /*!
     * \brief Will be called when finishing task is a requirement to another
     *        task
     *
     * If this task was the last needed requirement the requiring task will be
     * scheduled.
     *
     * Internal use only.
     */
    void OnRequirementDone(std::unique_ptr<Requirement> requirement, std::unique_ptr<TaskResult> result)
    {
      CSingleLock lock(m_mutex);

      auto it = m_requirementsOwnerMap.find(requirement.get());
      if (it != m_requirementsOwnerMap.end())
      {
        std::shared_ptr<Task> &owner = it->second;

        auto it2 = m_taskDataMap.find(owner);
        if (it2 != m_taskDataMap.end())
        {
          std::unique_ptr<TaskData> &taskData = it2->second;

          auto it3 = taskData->requirementsMap.find(requirement.get());
          if (it3 != taskData->requirementsMap.end())
          {
            size_t i = it3->second;

            Requirements &requirements = taskData->requirements;

            std::unique_ptr<Requirement> &requirement = requirements[i];
            requirement = std::move(result);

            bool x = false;
            if (x)
            {
              RunnableCallback callback = [this](Runnable runnable, std::unique_ptr<TaskResult> result)
                {
                  OnRunnableDone(runnable, std::move(result));
                };

              std::unique_ptr<Args> args; //! @todo Should be requirements
              m_threadPool->Append(taskData->runnable, callback, requirements.size(), std::move(args));
            }
          }
        }
      }

    }

    using RunnableOwnerMap = std::map<Runnable, std::shared_ptr<Task>>;
    using RequirementsOwnerMap = std::map<Requirement*, std::shared_ptr<Task>>;
    using TaskDataMap = std::map<std::shared_ptr<Task>, std::unique_ptr<TaskData>>;
    using TaskDataPair = std::pair<TaskDataMap::key_type, TaskDataMap::value_type>;

    const std::unique_ptr<ThreadPool> m_threadPool;
    RunnableOwnerMap m_runnableOwnerMap;
    RequirementsOwnerMap m_requirementsOwnerMap;
    TaskDataMap m_taskDataMap;
    CCriticalSection m_mutex;
    CEvent m_event;
  };
}
}
