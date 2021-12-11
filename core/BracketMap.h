#pragma once

#include <boost/bimap.hpp>

struct TagBracketOpen {};
struct TagBracketClose {};

using BracketMap = boost::bimaps::bimap<
    boost::bimaps::tagged<size_t, TagBracketOpen>,
    boost::bimaps::tagged<size_t, TagBracketClose>
>;

using BracketMapElement = BracketMap::value_type;
