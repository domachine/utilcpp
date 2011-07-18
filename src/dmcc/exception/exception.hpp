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


#ifndef DMCC_EXCEPTION_EXCEPTION_HPP
#define DMCC_EXCEPTION_EXCEPTION_HPP

#include <stdexcept>

#define DMCC_RAISABLE(class_name) class_name : public dmcc::exception::raisable

#define DMCC_INIT_RAISABLE(classname) \
    public: classname& set_file(const char* file) {m_file = file; return *this;} \
    classname& set_line(int line) {m_line = line; return *this;} private:


namespace dmcc {
    namespace exception {
        /**
         * @brief Adds debug info to an exception.
         */
        class debug_info
        {
        public:
            /**
             * @brief Constructs new object saving a path to the file in which the
             * The exception was thrown.
             * @param file The file in which the object is thrown
             * @param line The line in which the object is thrown.
             */
            debug_info(const std::string& what = std::string(),
                      const char* file = "<unknown>", int line = -1);

            ~debug_info() throw();

            void set_what_str(const std::string& what);

            /**
             * @brief Returns the error stored in the object.
             * @return The error message associated.
             */
            const char* debug_str() const;

            const std::string& file() const;

            int line() const;

            /**
             * @brief Makes this printable to an output-stream.
             * @return The given output-stream.
             */
            friend std::ostream& operator<<(std::ostream&, const debug_info&);

        protected:
            std::string m_file;
            int m_line;

        private:
            std::string m_what;
        };

        // Global operator to make debug_info printable.
        std::ostream& operator<<(std::ostream& os, const debug_info& dbg_info);


        /**
           @brief Base class for all exceptions that implement the
           raise protocol.
        */
        class raisable : public std::exception,
                          public debug_info
        {
            DMCC_INIT_RAISABLE(raisable)

        public:
            raisable(const std::string& what = std::string(),
                      const char* file = "", int line = -1);

            const char* what() const throw();
        };
    }
}

#endif  // DMCC_EXCEPTION_EXCEPTION_HPP
