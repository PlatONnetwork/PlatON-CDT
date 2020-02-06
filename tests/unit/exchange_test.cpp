#include "platon/exchange.hpp"
#include "unit_test.hpp"

using namespace platon;

TEST_CASE(exchange, energon) {
    Energon one_von = 1_Von;
    ASSERT(one_von.Get() == u256(VonUnit));

    Energon one_gvon = 1_GVon;
    ASSERT(one_gvon.Get() == u256(GVonUnit));

    Energon one_lat = 1_LAT;
    ASSERT(one_lat.Get() == u256(LATUnit));

    one_lat += 1_Von;
    ASSERT(one_lat.Get() == u256(LATUnit + VonUnit));

    Energon gon = 1_Von + 1_LAT;
    ASSERT(gon.Get() == u256(VonUnit+LATUnit));

    ASSERT((1.5_LAT).Get() == u256(1.5*LATUnit));
    ASSERT((0.0000000122_GVon).Get() == u256(0.0000000122*GVonUnit));
}

UNITTEST_MAIN() {
    RUN_TEST(exchange, energon);
}
