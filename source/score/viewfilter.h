/*
  * Copyright (C) 2015 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCORE_VIEWFILTER_H
#define SCORE_VIEWFILTER_H

#include <cassert>
#include "fileversion.h"
#include <regex>
#include <string>
#include <vector>

class ActivePlayer;
class Score;

/// A rule for filtering which staves are viewable. For example, a rule might be
/// whether the staff contains a particular player, or a player with a certain
/// number of strings.
class FilterRule
{
public:
    enum Subject : int
    {
        PLAYER_NAME = 0,
        NUM_STRINGS = 1,
    };

    enum Operation : int
    {
        LESS_THAN = 0,
        LESS_THAN_EQUAL = 1,
        EQUAL = 2,
        GREATER_THAN_EQUAL = 3,
        GREATER_THAN = 4
    };

    FilterRule();
    FilterRule(Subject subject, std::string value);
    FilterRule(Subject subject, Operation op, int value);

    bool operator==(const FilterRule &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns whether the given staff is visible.
    bool accept(const Score &score, int system_index, int staff_index) const;

private:
    bool accept(const Score &score, const ActivePlayer &player) const;

    Subject mySubject;
    Operation myOperation;
    int myIntValue;
    std::string myStrValue;
    std::regex myRegex;
};

/// A filter that specifies which staves are visible.
class ViewFilter
{
public:
    ViewFilter();
    bool operator==(const ViewFilter &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Adds a new rule to the filter.
    void addRule(const FilterRule &rule);

    /// Returns whether the given staff is visible.
    bool accept(const Score &score, int system_index, int staff_index) const;

private:
    std::vector<FilterRule> myRules;
};

template <class Archive>
void FilterRule::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("subject", mySubject);
    ar("operation", myOperation);

    switch (mySubject)
    {
    case PLAYER_NAME:
        ar("value", myStrValue);
        break;
    case NUM_STRINGS:
        ar("value", myIntValue);
        break;
    default:
        assert(!"Unexpected subject for filter.");
        break;
    }
}

template <class Archive>
void ViewFilter::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("rules", myRules);
}

#endif