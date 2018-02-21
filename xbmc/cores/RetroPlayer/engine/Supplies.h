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

#include "Demands.h" //! @todo Move to .cpp

#include <memory>

namespace KODI
{
namespace RETRO
{
  struct Predicate;
  struct Object;
  struct Demand;

  /*!
   * \brief Base class for what a task may supply
   *
   */
  struct Supply
  {
    virtual ~Supply() = default;

    std::unique_ptr<Predicate> predicate;
  };

  /*!
   * \brief Base class for supplies which matches against predicates and
   *        possibly objects
   */
  struct PredicateObjectSupply : public Supply
  {
    PredicateObjectSupply(std::unique_ptr<Predicate> predicate, std::unique_ptr<Object> object) :
      object(std::move(object))
    {
      Supply::predicate = std::move(predicate);
    }

    ~PredicateObjectSupply() override = default;

    virtual bool Matches(const Demand &demand) const
    {
      if (demand.predicate)
        return predicate == demand.predicate;

      return false;
    }

    virtual bool Conflict(const Supply &supply) const
    {
      return false;
    }

    std::unique_ptr<Object> object;
  };

  /*!
   * \brief The task might supply emit predicate(s) (and object) when run
   */
  struct Emit : public PredicateObjectSupply
  {
    ~Emit() override = default;
  };

  /*!
   * \brief The task might when run replace the predicate (and object(s))
   */
  struct Replace : public PredicateObjectSupply
  {
    ~Replace() override = default;

    bool Conflict(const Supply &supply) const override
    {
      if (dynamic_cast<const Emit*>(&supply) != nullptr)
        return supply.predicate == predicate;

      return false;
    }
  };

  /*!
   * \brief Will upgrade the predicate object
   *
   * NOT IMPLEMENTED YET.
   */
  struct Upgrade : public Replace
  {
  };

  /*!
   * \brief Will upgrade the class of a subject
   *
   * NOT IMPLEMENTED YET.
   */
  struct UpgradeClass : public Supply
  {
  };
}
}
