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

#include "inotify.hpp"

#include <iostream>
#include <boost/filesystem/convenience.hpp>

#include <cerrno>

#include "exception/raise.hpp"

#define INOTIFY_EVENT_SIZE (sizeof(struct inotify_event))
#define INOTIFY_BUFLEN ((INOTIFY_EVENT_SIZE + 16) * 1024)

namespace fs = boost::filesystem;

using fs::is_directory;
using fs::directory_iterator;
using boost::regex;

using boost::system::system_category;


namespace dmcc {
    namespace inotify {
        inotify::inotify()
            : m_descr(inotify_init())
        {
            // Check inotify initialization.
            if(m_descr <= 0)
                DMCC_RAISE_LINUX_SYS_ERR("unable to initialize inotify");
        }

        void inotify::add_watch(const fs::path& path, uint32_t mask)
        {
            // Add watch to underlaying inotify-descriptor.
            int wd = inotify_add_watch(m_descr, path.string().c_str(), mask);

            // Inform user about failure.
            if(wd <= 0)
                DMCC_RAISE_LINUX_SYS_ERR("unable to add watch for `" + path.string() + "'");

            m_wd_map.insert(std::pair<int,fs::path>(wd, path));
        }

        void inotify::connect_slot(const event_sig_t::slot_type& slot)
        {
            m_signal.connect(slot);
        }

        void inotify::listen()
        {
            bool break_out = false;

            while(!break_out) {
                // Buffer to store event stream chunks.
                unsigned char buf[INOTIFY_BUFLEN];

                ssize_t len = 0;

                do {
                    // Read waiting events.
                    len = read(m_descr, buf, INOTIFY_BUFLEN);
            
                } while (len == -1);

                if (len == -1 && !(errno == EWOULDBLOCK || errno == EINTR))
                    DMCC_RAISE_LINUX_SYS_ERR("reading events failed");

                if (len == -1)
                    return;

                ssize_t i = 0;

                // Parse events.
                while (i < len) {

                    // Is destroyed as early as possible.
                    event ev;

                    // Construct event from next chunk.
                    ev.m_event = (inotify_event*)(&buf[i]);

                    const fs::path& p = m_wd_map[ev.wd()];
            
                    ev.m_path = p / ev.name();

                    // Emit signal.
                    if((break_out = m_signal(*this, ev)) == true)
                        break;

                    i += INOTIFY_EVENT_SIZE + (ssize_t) ev.m_event->len;
                }
            }
        }


        event::event()
            : m_event(static_cast<inotify_event*>(0))
        {
        }

        int event::wd() const
        {
            DMCC_ASSERT(m_event);
            return m_event->wd;
        }

        uint32_t event::mask() const
        {
            DMCC_ASSERT(m_event);
            return m_event->mask;
        }

        uint32_t event::cookie() const
        {
            DMCC_ASSERT(m_event);
            return m_event->cookie;
        }

        std::string event::name() const
        {
            DMCC_ASSERT(m_event);

            if(m_event->len <= 0)
                return std::string();

            return m_event->name;
        }

        const fs::path& event::path() const
        {
            DMCC_ASSERT(m_event);
            return m_path;
        }
    }
}
