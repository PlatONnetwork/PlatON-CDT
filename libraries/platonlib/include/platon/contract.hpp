#pragma once

#define PLATON_ABI(NAME, MEMBER)
#define ACTION  __attribute__((annotate("action")))
#define CONST  __attribute__((annotate("const")))
#define CONTRACT class 

namespace platon {
    class Contract {
        public:
            Contract(){}
    };
}


