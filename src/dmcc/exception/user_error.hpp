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


#ifndef DMCC_EXCEPTION_USER_ERROR_HPP
#define DMCC_EXCEPTION_USER_ERROR_HPP

#include "exception.hpp"


namespace dmcc {
    namespace exception {

        /**
         * \brief Is thrown if an error occurs that
         * could be interesting for the user.
         *
         * It is catched at the toplevel and printed.
         */
        class DMCC_RAISABLE(user_error)
        {
            DMCC_INIT_RAISABLE(user_error)
        public:
            /**
             * \brief An enumeration specifying the error-levels that
             * a user error can have.
             *
             * An error usally causes the exit of the application.
             * A warning is just printed.
             */
            enum Level {
                WARNING,
                ERROR
            };

            /**
             * \brief Constructs a new Error that can be thrown.
             * \param what The error message this object will be associated
             * with.
             * \param level The error level.
             * \param file Is passed to DebugInfo.
             * \param line Is passed to DebugInfo.
             */
            user_error(const std::string& what, Level level);

            /**
             * \brief Returns the level associated with this object.
             * \return The level.
             */
            Level level() const;

        private:
            Level m_level;
        };

    }
}

#endif  // DMCC_EXCEPTION_USER_ERROR_HPP
