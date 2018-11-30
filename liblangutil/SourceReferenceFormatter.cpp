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

using namespace std;
using namespace dev;
using namespace langutil;

void SourceReferenceFormatter::printSourceLocation(SourceLocation const* _location)
{
	printSourceLocation(SourceReferenceExtractor::extract(_location));
}

void SourceReferenceFormatter::printSourceLocation(SourceReference const& _ref)
{
	if (_ref.startLine < 0)
		return; // Nothing we can print here

	if (_ref.startLine == _ref.endLine)
	{
		string const& line = _ref.line;

		m_stream << line << endl;

		for_each(
			line.cbegin(),
			line.cbegin() + _ref.startColumn,
			[this](char const& ch) { m_stream << (ch == '\t' ? '\t' : ' '); }
		);
		m_stream << "^";
		if (_ref.endColumn > _ref.startColumn + 2)
			m_stream << string(_ref.endColumn - _ref.startColumn - 2, '-');
		if (_ref.endColumn > _ref.startColumn + 1)
			m_stream << "^";
		m_stream << endl;
	}
	else
		m_stream <<
			_ref.line <<
			endl <<
			string(_ref.startColumn, ' ') <<
			"^ (Relevant source part starts here and spans across multiple lines)." <<
			endl;
}

void SourceReferenceFormatter::printSourceName(SourceReference const& _ref)
{
	if (_ref.interestLine != -1)
		m_stream << _ref.name << ":" << (_ref.interestLine + 1) << ":" << (_ref.interestColumn + 1) << ": ";
}

void SourceReferenceFormatter::printExceptionInformation(dev::Exception const& _error, std::string const& _category)
{
	printExceptionInformation(SourceReferenceExtractor::extract(_error, _category));
}

void SourceReferenceFormatter::printExceptionInformation(SourceReferenceExtractor::Message const& _msg)
{
	printSourceName(_msg.primary);

	m_stream << _msg.category << ": " << _msg.primary.description << endl;

	printSourceLocation(_msg.primary);

	for (auto const& ref : _msg.secondary)
	{
		printSourceName(ref);
		m_stream << ref.description << endl;
		printSourceLocation(ref);
	}
}
