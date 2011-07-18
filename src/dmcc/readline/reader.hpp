/*
 * This file is part of obexftpc.
 *
 * Obexftpc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Obexftpc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with obexftpc.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef READER_HPP
#define READER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>

#include <boost/signal.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>

namespace dmcc {
    namespace readline {
        /**
           @brief Is used as proxy to design an interface between
           the dynamically allocated strings of readline and modern c++ strings.
        */
        class c_str
        {
            c_str()
                : m_value(static_cast<char*>(0))
                {
                }

        public:
            c_str(char* value)
                : m_value(value)
                {
                }

            c_str(const std::string& value)
                {
                    m_value = static_cast<char*>(
                        malloc(value.size() + sizeof(char)));
                }

            /**
               @brief Returns the string as malloced char array.
             */
            operator char*()
                {
                    return m_value;
                }

            static c_str null()
                {
                    return c_str();
                }

        private:
            char* m_value;
        };


        /**
         * @brief Reads commands from the command-line and
         * throws appropriate signals. (using boost::signal).
         */
        class reader
        {
        public:
            typedef std::vector<std::string> arglist_t;
    
            // Saves much typing work!
            typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer_t;

            // Signaltype
            typedef boost::signal<bool (const std::string&, const arglist_t&)>
            str_arglist_sig_t;

            // Shared pointer to a signal.
            typedef boost::shared_ptr<str_arglist_sig_t> signal_ptr_t;

            typedef boost::tuple<const std::string&,
                                 const str_arglist_sig_t::slot_type&> simple_command_t;

            typedef boost::function<c_str (const std::string&, int state)> compl_func_t;

            typedef boost::tuple<const std::string&,
                                 const str_arglist_sig_t::slot_type&,
                                 const  compl_func_t&> command_t;


            /**
             * @brief Constructs new reader.
             * @param history_file  Is passed to gnu readline
             * @param history_size  Same thing
             */
            reader(const std::string& history_file, int history_size,
                   const std::string& prompt = "%> ");

            /**
             * @brief Runs the main loop.
             *
             * The user is constantly asked for commands and
             * signals are emitted.
             */
            void run_mainloop();

            /**
             * @brief Reads the next line from input.
             *
             * Utilises the readline library. So readline events
             * will be triggered.
             * @return The string read from input.
             */
            std::string readline();

            reader& operator<<(const simple_command_t& cmd);

            reader& operator<<(const command_t& cmd);

            signal_ptr_t add(const std::string& cmd,
                             const compl_func_t& completion_cb = compl_func_t());

            // Fields.
            static boost::escaped_list_separator<char> separator;

        private:

            std::string m_prompt;
            bool m_exit;
        };
    }
}

#endif  // READER_HPP
