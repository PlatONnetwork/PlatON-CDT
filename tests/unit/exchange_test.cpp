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
  Energon microatx = 1_microATX;
  ASSERT(microatx == Energon(u128(EnergonUnit::microATX)));

  Energon milliatx = 1_milliATX;
  ASSERT(milliatx == Energon(u128(EnergonUnit::milliATX)));

  Energon one_atx = 1_ATX;
  ASSERT(one_atx.Get() == u128(EnergonUnit::ATX));

  Energon katx = 1_kATX;
  ASSERT(katx.Get() == u128(EnergonUnit::kATX));

  Energon matx = 1_mATX;
  ASSERT(matx.Get() == u128(EnergonUnit::mATX));

  Energon gatx = 1_gATX;
  ASSERT(gatx.Get() == u128(EnergonUnit::gATX));

  one_atx += 1_VON;
  ASSERT(one_atx.Get() == u128(EnergonUnit::ATX) + u128(EnergonUnit::VON));

  Energon gon = 1_VON + 1_ATX;
  ASSERT(gon.Get() == u128(EnergonUnit::VON) + u128(EnergonUnit::ATX));
#else
  Energon microatp = 1_microATP;
  ASSERT(microatp == Energon(u128(EnergonUnit::microATP)));

  Energon milliatp = 1_milliATP;
  ASSERT(milliatp == Energon(u128(EnergonUnit::milliATP)));

  Energon one_atp = 1_ATP;
  ASSERT(one_atp.Get() == u128(EnergonUnit::ATP));

  Energon katp = 1_kATP;
  ASSERT(katp.Get() == u128(EnergonUnit::kATP));

  Energon matp = 1_mATP;
  ASSERT(matp.Get() == u128(EnergonUnit::mATP));

  Energon gatp = 1_gATP;
  ASSERT(gatp.Get() == u128(EnergonUnit::gATP));

  one_atp += 1_VON;
  ASSERT(one_atp.Get() == u128(EnergonUnit::ATP) + u128(EnergonUnit::VON));

  Energon gon = 1_VON + 1_ATP;
  ASSERT(gon.Get() == u128(EnergonUnit::VON) + u128(EnergonUnit::ATP));
#endif

  ASSERT(one_von < kvon);
  ASSERT(mvon > kvon);

#ifdef TESTNET
  ASSERT(microatx > mvon);
  ASSERT(milliatx > microatx);
  ASSERT(one_atx > milliatx);
  ASSERT(katx > one_atx);
  ASSERT(matx > katx);
  ASSERT(gatx > matx);
#else
  ASSERT(microatp > mvon);
  ASSERT(milliatp > microatp);
  ASSERT(one_atp > milliatp);
  ASSERT(katp > one_atp);
  ASSERT(matp > katp);
  ASSERT(gatp > matp);
#endif
}

UNITTEST_MAIN() { RUN_TEST(exchange, energon); }
