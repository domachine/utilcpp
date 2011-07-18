

set(INOTIFY_SOURCES dmcc/inotify/inotify.cpp)
set(READLINE_SOURCES dmcc/readline/reader.cpp)
set(EXCEPTION_SOURCES dmcc/exception/exception.cpp
  dmcc/exception/system_error.cpp
  dmcc/exception/user_error.cpp)

include_directories(dmcc)

add_library(dmcc ${INOTIFY_SOURCES}
  ${READLINE_SOURCES}
  ${EXCEPTION_SOURCES})
