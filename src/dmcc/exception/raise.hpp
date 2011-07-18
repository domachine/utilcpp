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


#ifndef DMCC_EXCEPTION_RAISE_HPP
#define DMCC_EXCEPTION_RAISE_HPP

#include <cerrno>

#include "user_error.hpp"
#include "system_error.hpp"


/**
 * @brief Raises a compatible exception and passes file
 * and line information with it.
 */
#define DMCC_RAISE(exc) (throw exc.set_file(__FILE__).set_line(__LINE__))

/**
 * @brief Raises a critical error that usally indicates a memory
 * error or a bug and causes the application to exit.
 *
 * It is caught at the highest level and is displayed to the user.
 */
#define DMCC_RAISE_CRITICAL(msg) DMCC_RAISE(dmcc::exception::raisable(msg))

#define DMCC_RAISE_LINUX_SYS_ERR(msg) \
    DMCC_RAISE(dmcc::exception::system_error(errno, boost::system::system_category(), msg))

/**
 * @brief Raises an UserError with level ``ERROR``.
 */
#define DMCC_RAISE_USER_ERR(msg) \
    DMCC_RAISE(dmcc::exception::user_error(msg,                         \
                                          dmcc::exception::user_error::ERROR))

#define DMCC_RAISE_USER_WARN(msg) \
    DMCC_RAISE(dmcc::exception::user_error(msg, dmcc::exception::user_error::WARNING))

/**
 * @brief The assertion, that doesn't blindly abort, but raises a critical error.
 */
#define DMCC_ASSERT(test) if(!(test)) DMCC_RAISE_CRITICAL("Assertion failed: " #test)

#endif  // DMCC_EXCEPTION_RAISE_HPP
