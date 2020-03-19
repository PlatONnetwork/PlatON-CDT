#include "platon/exchange.hpp"
#include "unit_test.hpp"

using namespace platon;

TEST_CASE(exchange, energon) {
  Energon one_von = 1_VON;
  ASSERT(one_von == Energon(u128(EnergonUnit::VON)));

  Energon kvon = 1_kVON;
  ASSERT(kvon == Energon(u128(EnergonUnit::kVON)));

  Energon mvon = 1_mVON;
  ASSERT(mvon == Energon(u128(EnergonUnit::mVON)));

  Energon one_gvon = 1_gVON;
  ASSERT(one_gvon.Get() == u128(EnergonUnit::gVON));

  Energon microlat = 1_microLAT;
  ASSERT(microlat == Energon(u128(EnergonUnit::microLAT)));

  Energon millilat = 1_milliLAT;
  ASSERT(millilat == Energon(u128(EnergonUnit::milliLAT)));

  Energon one_lat = 1_LAT;
  ASSERT(one_lat.Get() == u128(EnergonUnit::LAT));

  Energon klat = 1_kLAT;
  ASSERT(klat.Get() == u128(EnergonUnit::kLAT));

  Energon mlat = 1_mLAT;
  ASSERT(mlat.Get() == u128(EnergonUnit::mLAT));

  Energon glat = 1_gLAT;
  ASSERT(glat.Get() == u128(EnergonUnit::gLAT));

  one_lat += 1_VON;
  ASSERT(one_lat.Get() ==
         u128(EnergonUnit::LAT) + u128(EnergonUnit::VON));

  Energon gon = 1_VON + 1_LAT;
  ASSERT(gon.Get() ==
         u128(EnergonUnit::VON) + u128(EnergonUnit::LAT));

  ASSERT(one_von < kvon);
  ASSERT(mvon > kvon);
  ASSERT(microlat > mvon);
  ASSERT(millilat > microlat);
  ASSERT(one_lat > millilat);
  ASSERT(klat > one_lat);
  ASSERT(mlat > klat);
  ASSERT(glat > mlat);
}

UNITTEST_MAIN() { RUN_TEST(exchange, energon); }
