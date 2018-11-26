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
/**
 * @author Christian <c@ethdev.com>
 * @date 2014
 * Formatting functions for errors referencing positions and locations in the source.
 */

#include <liblangutil/SourceReferenceFormatter.h>
#include <liblangutil/Scanner.h>
#include <liblangutil/Exceptions.h>
#include <libdevcore/termcolor.h>
#include <cmath>
#include <iomanip>

using namespace std;
using namespace dev;
using namespace langutil;

namespace
{
inline ostream& frameColor(ostream& os) { return os << termcolor::bold << termcolor::blue; };
inline ostream& messageColor(ostream& os) { return os << termcolor::bold << termcolor::white; };
inline ostream& errorColor(ostream& os) { return os << termcolor::bold << termcolor::red; };
inline ostream& diagColor(ostream& os) { return os << termcolor::bold << termcolor::yellow; };
inline ostream& highlightColor(ostream& os) { return os << termcolor::yellow; };
}

void SourceReferenceFormatter::printSourceLocation(SourceLocation const* _location)
{
	printSourceLocation(SourceReferenceExtractor::extract(_location));
}

void SourceReferenceFormatter::printSourceLocation(SourceReference const& _ref)
{
	if (_ref.startLine < 0)
		return; // Nothing we can print here

	int const leftpad = static_cast<int>(log10(max(_ref.startLine, 1))) + 1;

	// line 0: source name
	m_stream << string(leftpad, ' ') << frameColor << "--> " << termcolor::reset;
	m_stream << _ref.name << ":" << (_ref.interestLine + 1) << ":" << (_ref.interestColumn + 1) << ": ";

	if (!_ref.description.empty())
		m_stream << messageColor << _ref.description << termcolor::reset << '\n';
	else
		m_stream << '\n';

	if (_ref.startLine == _ref.endLine)
	{
		// line 1:
		m_stream << string(leftpad, ' ') << frameColor << " |" << termcolor::reset << '\n';

		// line 2:
		m_stream << frameColor << _ref.interestLine << " | " << termcolor::reset
				 << _ref.line.substr(0, _ref.startColumn)
				 << highlightColor << _ref.line.substr(_ref.startColumn, _ref.locationLength) << termcolor::reset
				 << _ref.line.substr(_ref.endColumn) << '\n';

		// line 3:
		m_stream << string(leftpad, ' ') << frameColor << " | " << termcolor::reset;
		for_each(
			_ref.line.cbegin(),
			_ref.line.cbegin() + _ref.startColumn,
			[this](char const& ch) { m_stream << (ch == '\t' ? '\t' : ' '); }
		);
		m_stream << diagColor << string(_ref.locationLength, '^') << termcolor::reset << '\n';
	}
	else
	{
		// line 1:
		m_stream << string(leftpad, ' ') << frameColor << " |" << termcolor::reset << '\n';

		// line 2:
		m_stream << frameColor << _ref.interestLine << " | " << termcolor::reset
				 << _ref.line.substr(0, _ref.startColumn)
				 << highlightColor << _ref.line.substr(_ref.startColumn) << termcolor::reset << '\n';

		// line 3:
		m_stream << string(leftpad, ' ') << frameColor << " | " << termcolor::reset
				 << string(_ref.startColumn, ' ')
				 << diagColor << "^ (Relevant source part starts here and spans across multiple lines)."
				 << termcolor::reset << '\n';
	}

	m_stream << '\n';
}

void SourceReferenceFormatter::printExceptionInformation(dev::Exception const& _error, std::string const& _category)
{
	auto const ref = SourceReferenceExtractor::extract(_error, _category);

	// exception header line
	m_stream << errorColor << ref.category;
	if (string const* description = boost::get_error_info<errinfo_comment>(_error))
		m_stream << messageColor << ": " << *description << endl;
	else
		m_stream << termcolor::reset << '\n';

	if (!ref.primary.startLine || ref.primary.name.empty())
		m_stream << '\n';

	printSourceLocation(ref.primary);

	for (auto const& secondary : ref.secondary)
		printSourceLocation(secondary);
}
