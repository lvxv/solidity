/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <string>
#include <vector>

namespace dev
{
struct Exception;
}

namespace langutil
{

struct SourceReference
{
	std::string name;
	std::string line;
	int startLine;
	int startColumn;
	int interestLine;
	int interestColumn;
	int endLine;
	int endColumn;
	int locationLength;
	std::string description;
};

struct SourceLocation;

namespace SourceReferenceExtractor
{
	struct Message // Naming is a skill.
	{
		SourceReference primary;
		std::string category; // "Error", "Warning", ...
		std::vector<SourceReference> secondary;
	};

	Message extract(dev::Exception const& _exception, std::string _category);
	SourceReference extract(SourceLocation const* _location, std::string description = "");
}

}
