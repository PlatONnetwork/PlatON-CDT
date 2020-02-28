#include "platon/exchange.hpp"
#include "unit_test.hpp"

using namespace platon;

TEST_CASE(exchange, energon) {
  Energon one_von = 1_VON;
  ASSERT(one_von == Energon(VON));

  Energon kvon = 1_kVON;
  ASSERT(kvon == Energon(kVON));

  Energon mvon = 1_mVON;
  ASSERT(mvon == Energon(mVON));

  Energon one_gvon = 1_gVON;
  ASSERT(one_gvon.Get() == u128(gVON));

  Energon ulat = 1_uLAT;
  ASSERT(ulat == Energon(uLAT));

  Energon mlat = 1_mLAT;
  ASSERT(mlat == Energon(mLAT));

  Energon one_lat = 1_LAT;
  ASSERT(one_lat.Get() == u128(LAT));

  one_lat += 1_VON;
  ASSERT(one_lat.Get() == u128(LAT + VON));

  Energon gon = 1_VON + 1_LAT;
  ASSERT(gon.Get() == u128(VON + LAT));

  ASSERT(one_von < kvon);
  ASSERT(mvon > kvon);
  ASSERT(ulat > mvon);
  ASSERT(mlat > ulat);
  ASSERT(one_lat > mlat);
}

UNITTEST_MAIN() { RUN_TEST(exchange, energon); }
