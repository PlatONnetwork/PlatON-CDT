#pragma once

#define PLATON_ABI(NAME, MEMBER)
#define ACTION  __attribute__((used, annotate("Action")))
#define CONST  __attribute__((used, annotate("Const")))
#define EVENT  __attribute__((used, annotate("Event")))
#define CONTRACT class 

namespace platon {
    class Contract {
        public:
            Contract(){}
    };
}


