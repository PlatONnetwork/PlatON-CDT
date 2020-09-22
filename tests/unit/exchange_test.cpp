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

#ifdef TESTNET
  Energon microlax = 1_microLAX;
  ASSERT(microlax == Energon(u128(EnergonUnit::microLAX)));

  Energon millilax = 1_milliLAX;
  ASSERT(millilax == Energon(u128(EnergonUnit::milliLAX)));

  Energon one_lax = 1_LAX;
  ASSERT(one_lax.Get() == u128(EnergonUnit::LAX));

  Energon klax = 1_kLAX;
  ASSERT(klax.Get() == u128(EnergonUnit::kLAX));

  Energon mlax = 1_mLAX;
  ASSERT(mlax.Get() == u128(EnergonUnit::mLAX));

  Energon glax = 1_gLAX;
  ASSERT(glax.Get() == u128(EnergonUnit::gLAX));

  one_lax += 1_VON;
  ASSERT(one_lax.Get() == u128(EnergonUnit::LAX) + u128(EnergonUnit::VON));

  Energon gon = 1_VON + 1_LAX;
  ASSERT(gon.Get() == u128(EnergonUnit::VON) + u128(EnergonUnit::LAX));
#else
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
  ASSERT(one_lat.Get() == u128(EnergonUnit::LAT) + u128(EnergonUnit::VON));

  Energon gon = 1_VON + 1_LAT;
  ASSERT(gon.Get() == u128(EnergonUnit::VON) + u128(EnergonUnit::LAT));
#endif

  ASSERT(one_von < kvon);
  ASSERT(mvon > kvon);

#ifdef TESTNET
  ASSERT(microlax > mvon);
  ASSERT(millilax > microlax);
  ASSERT(one_lax > millilax);
  ASSERT(klax > one_lax);
  ASSERT(mlax > klax);
  ASSERT(glax > mlax);
#else
  ASSERT(microlat > mvon);
  ASSERT(millilat > microlat);
  ASSERT(one_lat > millilat);
  ASSERT(klat > one_lat);
  ASSERT(mlat > klat);
  ASSERT(glat > mlat);
#endif
}

UNITTEST_MAIN() { RUN_TEST(exchange, energon); }
