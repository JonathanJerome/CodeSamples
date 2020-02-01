#pragma once

#include <chrono>
#include <thread>

#include "MathUtility.h"
#include "Random.h"

//Disable warning for GUID
#pragma warning( push, 0 )


class GUIDGenerator
{
public:
	static unsigned long long Generate()
	{
		const u64 Sequence = GetSequence16();
		const u64 Time = GetTime16();
		const u64 Random = GetRandom16();
		const u64 UniqueTag = GetUniqueTag();
		return (Sequence << (16 * 0))
			| (Time << (16 * 1))
			| (Random << (16 * 2))
			| (UniqueTag << (16 * 3));
	}

private:
	static unsigned short GetSequence16()
	{
		if (m_Sequence == 0xffff)
		{
			unsigned short Time;
			while (m_LastResetTime == (Time = GetTime16()))
			{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
			m_LastResetTime = Time;
		}
		return (unsigned short)m_Sequence++;
	}

	static unsigned short GetTime16()
	{
		using namespace std::chrono;
		system_clock::time_point now = system_clock::now();
		time_t t = system_clock::to_time_t(now);

		unsigned short seed;
		seed = (t >> (16 * 0)) & 0xFFFF;
		seed ^= (t >> (16 * 1)) & 0xFFFF;
		seed ^= (t >> (16 * 2)) & 0xFFFF;
		seed ^= (t >> (16 * 3)) & 0xFFFF;

		return seed;
	}

	static unsigned short GetRandom16()
	{
		if ((m_Rand_m += 7) >= 9973)
			m_Rand_m -= 9871;

		if ((m_Rand_ia += 1907) >= 99991)
			m_Rand_ia -= 89989;

		if ((m_Rand_ib += 73939) >= 224729)
			m_Rand_ib -= 96233;

		m_Rand_irand = (m_Rand_irand * m_Rand_m) + m_Rand_ia + m_Rand_ib;

		return (m_Rand_irand >> 16) ^ (m_Rand_irand & 0x3FFF);
	}

	static unsigned short GetUniqueTag()
	{
		const unsigned short A = []
		{
			unsigned short A;
			const char*    pA = reinterpret_cast<const char*>(&A);
			long           llA = reinterpret_cast<long>(pA);
			llA ^= (llA >> 16);
			return static_cast<unsigned short>(llA);
		}();

		const unsigned short B = []
		{
			const unsigned ThreadHash32 = std::hash<std::thread::id>()(std::this_thread::get_id());
			return static_cast<unsigned short>(((ThreadHash32 >> 16) ^ ThreadHash32));
		}();

		return m_LastResetTime ^ A^B;
	}

	static unsigned m_Rand_m;
	static unsigned m_Rand_ia;
	static unsigned m_Rand_ib;
	static unsigned m_Rand_irand;
	static unsigned m_Sequence;
	static unsigned m_LastResetTime;
};

//Enable warnings again
#pragma warning( pop )
