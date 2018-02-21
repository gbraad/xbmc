set(SOURCES Demands.cpp
            Predicates.cpp
            Supplies.cpp
            TaskQueues.cpp
            Tasks.cpp
            ThreadPools.cpp)

set(HEADERS Demands.h
            Predicates.h
            Supplies.h
            TaskQueues.h
            Tasks.h
            ThreadPools.h
            Types.h)

core_add_library(retroplayer_engine)
