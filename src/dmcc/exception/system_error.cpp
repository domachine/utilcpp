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


#include "system_error.hpp"

using boost::system::error_code;
using boost::system::error_category;


namespace dmcc {
    namespace exception {
        system_error::system_error(error_code err)
            : boost::system::system_error(err)
        {
        }

        system_error::system_error(error_code err, const std::string& what)
            : boost::system::system_error(err, what)
        {
        }

        system_error::system_error(int ev, const error_category& ecat,
                                 const std::string& what)
            : boost::system::system_error(ev, ecat, what)
        {
        }

        system_error::system_error(int ev, const error_category& ecat)
            : boost::system::system_error(ev, ecat)
        {
        }

        const char* system_error::what() const throw()
        {
            return (std::string(raisable::what()) +
                    " " + boost::system::system_error::what()).c_str();
        }

    }
}
