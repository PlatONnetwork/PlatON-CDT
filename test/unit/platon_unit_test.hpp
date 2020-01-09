// Unit test assertion
#define UNIT_ASSERT(condition) {\
    bool is_right = condition;\
    if(! is_right) {\
        return 1;\
    }\
}

//Unit test start macro
#define UNIT_BEGIN  extern "C" {\
int invoke(){\

// Unit test ends macro
#define UNIT_END }}

// Unit test ends macro
#define UNIT_SUCCESS return 0;
