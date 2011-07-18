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

#ifndef DMCC_INOTIFY_INOTIFY_HPP
#define DMCC_INOTIFY_INOTIFY_HPP

#include <stdexcept>

#include <boost/signal.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

#include <sys/inotify.h>


// Forward declaration
class inotify;

struct inotify_event;


namespace dmcc {
    namespace inotify {
        class event;
        class watch;

        /**
         * @brief Wraps all this low-level inotify stuff
         * and provides a nice object-oriented interface to it.
         */
        class inotify
        {
            // Type to save path together with the associated depth.
            //typedef std::pair<boost::filesystem::path, int> watch_tuple_t;

        public:
            // Event wrapper

            typedef boost::signal<bool (inotify&, const event& event)> event_sig_t;

            /**
             * @brief Constructs a new object and initializes the
             * inotify-interface.
             */
            inotify();

            /**
             * @brief Adds a new watch for a file.
             * @param path The path to watch
             * @param mask The event-mask to listen for
             * @param depth The depth relative to the root-watch.
             */
            void add_watch(const boost::filesystem::path& path, uint32_t mask //,
                           /*int depth*/);

            void add_watch(boost::shared_ptr<watch> w, uint32_t mask);

            /**
             * @brief Connect a slot to the event-signal.
             *
             * Everytime an event is read, a signal is fired.
             * See Boost.Signal for further information.
             * @param slot The slot to connect.
             */
            void connect_slot(const event_sig_t::slot_type& slot);

            /**
             * @brief Start the listening process.
             *
             * This functions blocks until the signal-slot returns false.
             */
            void listen();

        private:

            event_sig_t m_signal;
            int m_descr;

            std::map<int,boost::shared_ptr<watch> > m_wd_map;
        };


        /**
         * @brief A wrapper for an inotify-event structure.
         */
        class event
        {
            friend class inotify;
            explicit event();

        public:
            /**
             * @brief Returns the watch descriptor.
             * @return The watch-descriptor.
             */
            int wd() const;

            /**
               @brief Returns the mask that the event carries with.
            */
            uint32_t mask() const;
            uint32_t cookie() const;

            std::string name() const;

            boost::filesystem::path path() const;

        private:
            inotify_event* m_event;
            //boost::filesystem::path m_path;
            boost::shared_ptr<watch> m_watch;
        };


        /**
           @brief Used to transfer additional information with
           an event object.
        */
        class watch
        {
        public:
            watch(const boost::filesystem::path& path);

            const boost::filesystem::path& path();

        private:
            boost::filesystem::path m_path;
        };
    }
}

#endif  // DMCC_INOTIFY_INOTIFY_HPP
