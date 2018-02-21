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
#include "threads/Event.h"
#include "threads/CriticalSection.h"
#include "threads/SingleLock.h" //! @todo Move to .cpp
#include "threads/Thread.h"

#include <algorithm> //! @todo Move to .cpp
#include <deque>
#include <functional>
#include <memory>
#include <utility> //! @todo Move to .cpp

namespace KODI
{
namespace RETRO
{

  struct Args
  {
  };

  struct Callback
  {
    virtual ~Callback() = default;

    virtual void OnFinish(Runnable runnable, bool result) = 0;
  };

  struct WorkItem
  {
    WorkItem(Runnable runnable, std::unique_ptr<Callback> callback, unsigned int priority, std::unique_ptr<Args> args) :
      runnable(std::move(runnable)),
      callback(std::move(callback)),
      priority(priority),
      args(std::move(args))
    {
    }

    Runnable runnable;
    std::unique_ptr<Callback> callback;
    unsigned int priority;
    std::unique_ptr<Args> args;
  };

  void SafeExecute(std::unique_ptr<WorkItem> WorkItem)
  {
    //! @todo Debug log: Running <runnable> in thread <thread ID> with args <args>

    bool result = WorkItem->runnable(std::move(WorkItem->args));

    WorkItem->callback->OnFinish(WorkItem->runnable, result);
  }

  struct ThreadedWorker;

  struct ThreadPool
  {
    virtual ~ThreadPool() = default;

    virtual void Append(Runnable runnable, RunnableCallback callback, unsigned int priority, std::unique_ptr<Args> args) = 0;

    virtual std::unique_ptr<WorkItem> GetNextWorkItem() { return std::unique_ptr<WorkItem>(); }

    virtual void OnDone(ThreadedWorker &worker) { }

    virtual void Quit() = 0;

    virtual void Join() = 0;
  };

  //struct ThreadPool;

  struct ThreadedWorker : protected CThread
  {
    ThreadedWorker(ThreadPool &owner) :
      CThread("ThreadedWorker"),
      m_owner(owner)
    {
      Create(false);
    }

  protected:
    void Process() override
    {
      std::unique_ptr<WorkItem> workItem = m_owner.GetNextWorkItem();

      while (workItem)
      {
        SafeExecute(std::move(workItem));
        workItem = m_owner.GetNextWorkItem();
      }

      m_owner.OnDone(*this);
    }

  private:
    ThreadPool &m_owner;
  };

  struct MainloopThreadPool : public ThreadPool
  {
    ~MainloopThreadPool() override = default;

    void Append(Runnable runnable, std::unique_ptr<Callback> callback, unsigned int priority, std::unique_ptr<Args> args) override
    {
      //! @todo Debug log: Append <runnable> with args <args>

      CSingleLock lock(m_mutex);

      m_queue.emplace_back(new WorkItem(std::move(runnable), std::move(callback), priority, std::move(args)));
      m_event.Set();
    }

    void Quit() override
    {
      CSingleLock lock(m_mutex);

      m_run = false;
      m_event.Set();
    }

    void Join() override
    {
      CSingleLock lock(m_mutex);

      while (m_run)
      {
        if (!m_queue.empty())
        {
          ItemQueue::iterator workItem = m_queue.end();

          for (ItemQueue::iterator pi = m_queue.begin(); pi != m_queue.end(); ++pi)
          {
            if (workItem == m_queue.end() || (*pi)->priority > (*workItem)->priority)
              workItem = pi;
          }

          if (workItem != m_queue.end())
          {
            SafeExecute(std::move(*workItem));
            m_queue.erase(workItem);
          }
        }
        else
        {
          m_event.Wait();
        }
      }
    }

  private:
    using ItemQueue = std::deque<std::unique_ptr<WorkItem>>;

    ItemQueue m_queue;
    bool m_run = true;
    CCriticalSection m_mutex;
    CEvent m_event;
  };

  struct SingleThreadedThreadPool : public ThreadPool
  {
    ~SingleThreadedThreadPool() override = default;

    void Append(Runnable runnable, std::unique_ptr<Callback> callback, unsigned int priority, std::unique_ptr<Args> args) override
    {
      CSingleLock lock(m_mutex);

      m_queue.emplace_back(new WorkItem(std::move(runnable), std::move(callback), priority, std::move(args)));

      if (!m_worker)
        m_worker.reset(new ThreadedWorker(*this));

      m_event.Set();
    }

    std::unique_ptr<WorkItem> GetNextWorkItem() override
    {
      std::unique_ptr<WorkItem> workItem;

      CSingleLock lock(m_mutex);

      ItemQueue::iterator wi = m_queue.end();

      for (ItemQueue::iterator pi = m_queue.begin(); pi != m_queue.end(); ++pi)
      {
        if (wi == m_queue.end() || (*pi)->priority > (*wi)->priority)
          wi = pi;
      }

      if (wi != m_queue.end())
      {
        workItem = std::move(*wi);
        m_queue.erase(wi);
      }

      m_event.Set();

      return workItem;
    }

    void OnDone(ThreadedWorker &worker) override
    {
      //! @todo Debug log: Removing worker from threadpool

      CSingleLock lock(m_mutex);

      m_worker.reset();
      m_event.Set();
    }

    void Quit() override
    {
      //! @todo Debug log: Quiting threadpool

      CSingleLock lock(m_mutex);

      m_run = false;
      m_event.Set();
    }

    void Join() override
    {
      CSingleLock lock(m_mutex);

      while (m_run && m_worker && !m_queue.empty())
        m_event.Wait();
    }

  private:
    using ItemQueue = std::deque<std::unique_ptr<WorkItem>>;

    ItemQueue m_queue;
    bool m_run = true;
    std::unique_ptr<ThreadedWorker> m_worker;
    CCriticalSection m_mutex;
    CEvent m_event;
  };

  struct OptimisticThreadPool : public ThreadPool
  {
    OptimisticThreadPool(unsigned int workerCount) :
      m_workerCount(workerCount)
    {
    }

    void Append(Runnable runnable, std::unique_ptr<Callback> callback, unsigned int priority, std::unique_ptr<Args> args) override
    {
      CSingleLock lock(m_mutex);

      if (m_acceptNewTasks)
      {
        m_queue.emplace_back(new WorkItem(std::move(runnable), std::move(callback), priority, std::move(args)));

        if (m_workers.size() < m_workerCount)
          m_workers.emplace_back(new ThreadedWorker(*this));

        m_event.Set();
      }
    }

    std::unique_ptr<WorkItem> GetNextWorkItem() override
    {
      std::unique_ptr<WorkItem> workItem;

      CSingleLock lock(m_mutex);

      ItemQueue::iterator wi = m_queue.end();

      for (ItemQueue::iterator pi = m_queue.begin(); pi != m_queue.end(); ++pi)
      {
        if (wi == m_queue.end() || (*pi)->priority > (*wi)->priority)
          wi = pi;
      }

      if (wi != m_queue.end())
      {
        workItem = std::move(*wi);
        m_queue.erase(wi);
      }

      m_event.Set();

      return workItem;
    }

    void OnDone(ThreadedWorker &worker) override
    {
      CSingleLock lock(m_mutex);

      /* @todo
      auto WorkerMatches = [this](ThreadedWorker &worker)
        {
          return this == &worker;
        };

      m_workers.erase(std::remove_if(m_workers.begin(), m_workers.end(), WorkerMatches), m_workers.end());
      */

      m_event.Set();
    }

    void Quit() override
    {
      //! @todo Debug log: Quiting threadpool

      CSingleLock lock(m_mutex);

      m_queue.clear();
      m_acceptNewTasks = false;
      m_event.Set();
    }

    void Join() override
    {
      CSingleLock lock(m_mutex);

      while (!m_workers.empty() && !m_queue.empty())
        m_event.Wait();
    }

  private:
    using ItemQueue = std::deque<std::unique_ptr<WorkItem>>;
    using WorkerQueue = std::deque<std::unique_ptr<ThreadedWorker>>;

    ItemQueue m_queue;
    bool m_acceptNewTasks = true;
    const unsigned int m_workerCount;
    WorkerQueue m_workers;
    CCriticalSection m_mutex;
    CEvent m_event;
  };
}
}
