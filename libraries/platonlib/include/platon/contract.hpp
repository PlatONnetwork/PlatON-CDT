#pragma once

#define PLATON_ABI(NAME, MEMBER)
#define ACTION  __attribute__((used, annotate("Action")))
#define CONST  __attribute__((used, annotate("Const")))
#define EVENT  __attribute__((used, annotate("Event0")))
#define EVENT1  __attribute__((used, annotate("Event1")))
#define EVENT2  __attribute__((used, annotate("Event2")))
#define EVENT3  __attribute__((used, annotate("Event3")))
#define EVENT4  __attribute__((used, annotate("Event4")))
#define CONTRACT class 

namespace platon {
    class Contract {
        public:
            Contract(){}
    };
}


