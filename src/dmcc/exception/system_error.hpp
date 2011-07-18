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


#ifndef DMCC_EXCEPTION_SYSTEM_ERROR_HPP
#define DMCC_EXCEPTION_SYSTEM_ERROR_HPP

#include <boost/system/system_error.hpp>

#include "exception.hpp"


namespace dmcc {
    namespace exception {
        
        /**
           \brief Makes the boost system_error compatible with the
           raise protocol.
        */
        class DMCC_RAISABLE(system_error),
                           public boost::system::system_error
        {
            DMCC_INIT_RAISABLE(system_error)

        public:
            system_error(boost::system::error_code err);

            system_error(boost::system::error_code err, const std::string& what);

            system_error(int ev,
                        const boost::system::error_category& ecat, const std::string& what);

            system_error(int ev, const boost::system::error_category& ecat);

            const char* what() const throw();
        };
    }
}

#endif  // DMCC_EXCEPTION_SYSTEM_ERROR_HPP
