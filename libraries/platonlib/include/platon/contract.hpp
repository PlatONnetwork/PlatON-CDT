#pragma once

#define PLATON_ABI(NAME, MEMBER)
#define ACTION  __attribute__((used, annotate("Action")))
#define CONST  __attribute__((used, annotate("Const")))
#define CONTRACT class 

namespace platon {
    class Contract {
        public:
            Contract(){}
    };
}


