/*
 * This file is part of Obexftpc.
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
 * along with Obexftpc.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "reader.hpp"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "exception/raise.hpp"

#define RAISE_USER_ERR DMCC_RAISE_USER_ERR


namespace dmcc {
    namespace readline {

        namespace {

            // Pair to bind a signal and completion-function together.
            typedef std::pair<reader::signal_ptr_t, reader::compl_func_t> cmd_pair_t;

            // Pointer to a signal-completion-function pair.
            typedef boost::shared_ptr<cmd_pair_t > cmd_pair_ptr_t;

            // Map type used to store command-pairs.
            typedef std::map<std::string, cmd_pair_ptr_t> cmd_pairs_t;

            // Command storage.
            cmd_pairs_t cmd_pairs;


            // Cuts a command into cmd-name and arguments. Quoted arguments
            // are supported (see boost tokenizer doc).
            // Empty arguments are abandoned.
            void parse_command(const std::string& raw,
                               std::string& name_out /* cmd-name output */,
                               reader::arglist_t& arglist_out      /* cmd-argument output */)
            {
                boost::escaped_list_separator<char> sep_func("\\", " ", "\"'");
                reader::tokenizer_t tok(raw, sep_func);

                // Set to true after the command (first token) was encountered.
                bool found_cmd = false;

                reader::tokenizer_t::iterator iter = tok.begin();
                for(; iter != tok.end(); ++iter) {
                    if(iter->empty())
                        continue;

                    if(!found_cmd) {
                        // Command-name encountered -> set output reference.
                        name_out = *iter;
                        found_cmd = true;
                    }
                    else
                        arglist_out.push_back(*iter);
                }
            }


            // Readline callback proxy.
            // Calls the bound completion functions and generates command
            // completion strings.
            // Readline calls it multiple times to generate completions.
            char* compl_proxy(const char* text, int state)
            {
                static bool do_cmd_compl;
                static std::string cmd_name;
                static cmd_pairs_t::const_iterator iterator;

                std::string ret;

                if(state == 0) {
                    // Initialize the stuff and detect
                    // the completion situation.

                    boost::cmatch res;
                    boost::regex reg("^[ \t]*([a-z0-9A-Z_]+)[ \t]+.*$");

                    if(boost::regex_match(rl_line_buffer, res, reg)) {
                        do_cmd_compl = false;

                        if(res.size() >= 1)
                            cmd_name = res[1];
                    }
                    else
                        do_cmd_compl = true;

                    iterator = cmd_pairs.begin();
                }


                if(!do_cmd_compl) {
                    // Command was typed completely and we need to complete
                    // an argument. So we call the installed callback.

                    cmd_pairs_t::iterator it = cmd_pairs.find(cmd_name);

                    if(it != cmd_pairs.end()) {
                        // Suitable command found -> let's call the
                        // completion function.

                        reader::compl_func_t f = it->second->second;

                        if(!f.empty())
                            // Call the bound completion function
                            return f(text, state);
                        else
                            // If there is no callback function, do simple
                            // filename completion.
                            return rl_filename_completion_function(text, state);
                    }
                }
                else {
                    // Do command completion.

                    size_t match_len = strlen(text);

                    // Loop through all available commands and try to find a match.
                    for(; iterator != cmd_pairs.end(); ++iterator) {
                        if(strncmp(text, iterator->first.c_str(), match_len) == 0) {
                            // Matching command found.

                            ret = iterator->first;

                            ++iterator;
                            break;
                        }
                    }
                }

                if(ret.empty())
                    return 0;
                else {
                    // Do the dirty malloc stuff for readline.

                    // Malloced string that will be freed by readline.
                    char* alloc = static_cast<char*>(malloc(ret.size() + 1));
                    strcpy(alloc, ret.c_str());

                    return alloc;
                }
            }
        }


        reader::reader(const std::string& history_file, int history_size,
                       const std::string& prompt)
            : m_prompt(prompt), m_exit(false)
        {
            rl_completion_entry_function = compl_proxy;
        }


        void reader::run_mainloop()
        {
            typedef boost::shared_ptr<str_arglist_sig_t> signal_ptr_t;

            while(!m_exit) {
                arglist_t args;
                std::string cmd;
        
                std::string input_str = this->readline();

                parse_command(input_str, cmd, args);

                if(cmd == "exit" || cmd == "quit")
                    // Eat exit or quit requests directly and break the loop.
                    break;
                else if(!cmd.empty()) {
                    // Emit signal to connectors.
                    cmd_pair_ptr_t cmd_pair_ptr = cmd_pairs[cmd];

                    if(cmd_pair_ptr) {
                        signal_ptr_t sig = cmd_pair_ptr->first;

                        if(sig)
                            m_exit = (*sig)(cmd, args);
                        else
                            RAISE_USER_ERR("command not found: " + cmd);
                    }
                }
            }
        }

        std::string reader::readline()
        {
            m_exit = true;

            char* l = ::readline(m_prompt.c_str());

            if(!l)
                return std::string();

            m_exit = false;

            std::string input(l);
            free(l);

            boost::algorithm::trim(input);
    
            if ( !input.empty() ) {
                if ((history_length == 0) ||
                    (input != history_list()[ history_length - 1 ]->line))
                {
                    add_history( input.c_str() );
                    // if ( history_length >= static_cast< int >( HistoryLimit ) )
                    // {
                    //     stifle_history( HistoryLimit );
                    // }
                }
            }

            return input;
        }

        reader& reader::operator<<(const simple_command_t& cmd)
        {
            signal_ptr_t sig = signal_ptr_t(new str_arglist_sig_t());
            sig->connect(cmd.get<1>());

            cmd_pairs[cmd.get<0>()] = cmd_pair_ptr_t(
                new cmd_pair_t(sig, reader::compl_func_t()));

            return *this;
        }

        reader& reader::operator<<(const command_t& cmd)
        {
            signal_ptr_t sig = signal_ptr_t(new str_arglist_sig_t);
            sig->connect(cmd.get<1>());
    
            cmd_pairs[cmd.get<0>()] = cmd_pair_ptr_t(new cmd_pair_t(sig, cmd.get<2>()));

            return *this;
        }

        reader::signal_ptr_t reader::add(const std::string& cmd,
                                         const compl_func_t& completion_cb)
        {
            signal_ptr_t sig = signal_ptr_t(new str_arglist_sig_t);
    
            cmd_pairs[cmd] = cmd_pair_ptr_t(new cmd_pair_t(sig, completion_cb));

            return sig;
        }


        boost::escaped_list_separator<char> reader::separator =
                                  boost::escaped_list_separator<char>("", "", "");
    }
}
