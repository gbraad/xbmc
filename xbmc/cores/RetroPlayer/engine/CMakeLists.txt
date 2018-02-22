set(SOURCES Core.cpp
            Demands.cpp
            Predicates.cpp
            Supplies.cpp
            TaskQueues.cpp
            Tasks.cpp
            ThreadPools.cpp)

set(HEADERS Core.h
            Demands.h
            Predicates.h
            Supplies.h
            TaskQueues.h
            Tasks.h
            ThreadPools.h
            Types.h)

core_add_library(retroplayer_engine)
