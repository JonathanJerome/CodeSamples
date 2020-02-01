#include "GUID.h"

unsigned GUIDGenerator::m_Rand_m = 971;
unsigned GUIDGenerator::m_Rand_ia = Random::Rand(20, 11113);
unsigned GUIDGenerator::m_Rand_ib = Random::Rand(50, 104322);
unsigned GUIDGenerator::m_Rand_irand = 4181 + GetTime16();
unsigned GUIDGenerator::m_Sequence = 0;
unsigned GUIDGenerator::m_LastResetTime = 179426549;