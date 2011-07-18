/* This program listens on a directory for changes and applies them
 * to another location, too.
 * Copyright (C) 2010  Dominik Burgdörfer <dominik.burgdoerfer@googlemail.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "exception.hpp"

#include <sstream>
#include <boost/filesystem/convenience.hpp>

using boost::filesystem::basename;
using boost::filesystem::extension;


namespace dmcc {
    namespace exception {
        debug_info::debug_info(const std::string& what, const char* file, int line)
            : m_what(what),
              m_file(basename(file) + extension(file)),
              m_line(line)
        {
        }

        debug_info::~debug_info() throw()
        {
        }

        const char* debug_info::debug_str() const
        {
#ifdef NDEBUG
            return m_what.c_str();
#else
            std::stringstream s;
            s << "[" << m_file << ":" << m_line << "]";

            if(!m_what.empty())
                s << " " + m_what;

            return s.str().c_str();
#endif
        }

        const std::string& debug_info::file() const
        {
            return m_file;
        }

        int debug_info::line() const
        {
            return m_line;
        }

        std::ostream& operator<<(std::ostream& os, const debug_info& dbg_info)
        {
            os << dbg_info.debug_str();

            return os;
        }

        raisable::raisable(const std::string& what, const char* file, int line)
            : debug_info(what, file, line)
        {
        }

        const char* raisable::what() const throw()
        {
            return debug_str();
        }
    }
}
