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

#include <memory>
#include <utility> //! @todo Move to .cpp

namespace KODI
{
namespace RETRO
{
  struct Subject;
  struct Predicate;
  struct Object;
  struct Class;

  /*!
   * \brief Base class for what a task may demand
   */
  struct Demand
  {
    virtual ~Demand() = default;

    virtual bool Matches(const Subject &subject) = 0;
  };

  /*!
   * \brief Base class for demands which matches against predicates and
   *        possibly objects
   *
   */
  struct PredicateObjectDemand : public Demand
  {
    PredicateObjectDemand(std::unique_ptr<Predicate> predicate, std::unique_ptr<Object> object) :
      predicate(std::move(predicate)),
      object(std::move(object))
    {
    }

    ~PredicateObjectDemand() override = default;

    bool Matches(const Subject &subject) override
    {
      auto obj = subject. [predicate];

      if (obj && object)
        return obj == object;
      else
        return obj != nullptr;

      return false;
    }

    std::unique_ptr<Predicate> predicate;
    std::unique_ptr<Object> object;
  };

  /*!
   * \brief Optional predicate, object keypair
   *
   * The task will be put on hold if any task may provide the pair but will
   * run if it is not provided.
   * */
  struct Optional : public PredicateObjectDemand
  {
    //! @todo NOT IMPLEMENTED YET
  };

  /*!
   * \brief A hard dependency
   *
   * The task will never run if its not provided. The task will wait until
   * on any task which might provide or replace the key-value-pair.
   * */
  struct Required : public PredicateObjectDemand
  {
  };

  /*!
   * \brief  A key-value-pair which may NOT exist on subject for this task to
   *         run
   *
   * The task will be scheduled after any task which might provide or replace
   * the key-value-pair and will be removed from queue if one emits the
   * key-value-pair.
   */
  struct None : public PredicateObjectDemand
  {
    bool Matches(const Subject &subject) override
    {
      return !PredicateObjectDemand::Matches(subject);
    }
  };

  /*!
   * \brief A demand on the class of a subject
   */
  struct RequiredClass : public Demand
  {
    const std::unique_ptr<Class> clazz;
    const bool allowExtended;

    RequiredClass(std::unique_ptr<Class> clazz, bool allowExtended) :
      clazz(std::move(clazz)),
      allowExtended(allowExtended)
    {
    }

    ~RequiredClass() override = default;

    //! @todo
    bool Matches(const Subject &subject) override
    {
      /* @todo
      // Input and sought class are same
      if (clazz == subject.clazz)
        return true;

      // Input class may extend towards sought class, but it has not reached
      // there yet
      else if (subject.clazz == clazz)
        return false;
      */

      return false;
    }
  };
}
}
