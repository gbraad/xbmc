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

#include "Demands.h"
#include "TaskQueues.h"
#include "Tasks.h"
#include "Types.h"
#include "Supplies.h"
#include "threads/CriticalSection.h"
#include "threads/Event.h"
#include "threads/SingleLock.h"

#include <algorithm>
#include <map>
#include <string>

namespace KODI
{
namespace RETRO
{
  using Class = std::string;
  using Predicate = std::string;
  using Object = std::deque<std::string>;
  using Metadata = std::map<Predicate, Object>;

  std::string IsolateIfSingle(Object object)
  {
    if (object.empty())
      return "";
    else if (object.size() == 1)
      return *object.begin();
    else
      return object; //! @todo
  }

  struct Subject
  {
    Subject(Class clazz = "", Metadata metadata) :
      clazz(std::move(clazz)),
      metadata(std::move(metadata))
    {
    }

    std::map<std::string, std::string> ToDict()
    {
      std::map<std::string, std::string> s;

      for (auto &it : metadata)
      {
        std::string value = IsolateIfSingle(it.second);
        if (!value.empty())
          s[it.first] = std::move(value);
      }

      return s;
    }

    std::string GetItem(const std::string &name)
    {
      return IsolateIfSingle(metadata[name]));
    }

    void Emit(std::string predicate, std::string object)
    {
      if (!object.empty())
        metadata[predicate].emplace_back(std::move(object));
    }

    void Replace(std::string predicate, std::string object)
    {
      auto it = metadata.find(predicate);
      if (it != metadata.end())
        metadata.erase(it);

      if (!object.empty())
        metadata
    }

    Class clazz;
    Metadata metadata;
  };

  struct Callback;

  struct SubjectTaskDispatcher
  {
    SubjectTaskDispatcher(std::unique_ptr<Subject> subject, std::deque<std::shared_ptr<Task>> subjectTasks, std::shared_ptr<TaskQueue> taskQueue, TaskCallback callback) :
      subject(std::move(subject)),
      availableTasks(std::move(subjectTasks)),
      taskQueue(std::move(taskQueue)),
      callback(callback)
    {
    }

    void ScheduleNonConflictingTasks()
    {
      std::vector<std::shared_ptr<Task>> possibleTasks = PurgeImpossibleTasks(subject, availableTasks);
      std::vector<std::shared_ptr<Task>> doableTasks = FindDoableTasks(subject, std::move(possibleTasks));
      doableTasks = PurgeConflictingTasks(std::move(doableTasks));

      if (!doableTasks.empty())
      {
        for (std::shared_ptr<Task> &task : doableTasks)
        {
          auto createdTask = task->Run(subject);
          runningTasks.emplace_back(createdTask);

          taskQueue->AddTask(std::move(createdTask),
            [this](std::shared_ptr<Task> task, int error, std::unique_ptr<TaskResult> result)
            {
              return OnDone(std::move(task), std::move(result));
            });
          }

        taskPath.emplace_back(std::move(doableTasks));
      }
      else
      {
        //! @todo Debug log: Final scheduling order became <taskPath>
        callback(subject);
      }
    }

    void OnDone(std::shared_ptr<Task> task, int error, std::unique_ptr<TaskResult> result)
    {
      if (error != 0)
        callback(error, nullptr);
      else
      {
        CSingleLock lock(m_mutex);

        auto it = std::find(runningTasks.begin(), runningTasks.end(), task);
        if (it != runningTasks.end())
          runningTasks.erase(it);

        if (runningTasks.empty())
          ScheduleNonConflictingTasks();
      }
    }

    std::unique_ptr<Subject> subject;
    TaskCallback callback;
    std::deque<std::shared_ptr<Task>> runningTasks;
    std::deque<std::shared_ptr<Task>> availableTasks;
    std::shared_ptr<TaskQueue> taskQueue;
    std::deque<std::shared_ptr<Task>> taskPath; // For debugging purposes
    CCriticalSection m_mutex;
    CEvent m_event;
  };

  struct Engine
  {
    void RegisterModule(std::vector<std::shared_ptr<Task>> module)
    {
      for (auto &task : module)
      {
        if (dynamic_cast<SubjectTask*>(task.get()) != nullptr)
          registeredTasks.emplace_back(std::move(task));
      }
    }

    std::unique_ptr<SubjectTaskDispatcher> Get(std::unique_ptr<Subject> subject, std::unique_ptr<Callback> &callback)
    {
      std::unique_ptr<SubjectTaskDispatcher> dispatcher;

      dispatcher.reset(new SubjectTaskDispatcher{std::move(subject), std::move(registeredTasks), new TaskQueue{threadPool}, std::move(callback)});

      //! @todo Should not return, should just keep a reference so it can be paused
      return dispatcher;
    }

    std::deque<Task> registeredTasks;
    std::shared_ptr<ThreadPool> threadPool;
  };
}
}
