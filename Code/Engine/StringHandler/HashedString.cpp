#include "HashedString.h"
#include "../Asserts/Asserts.h"

namespace eae6320
{
	namespace StringHandler
	{

		uint32_t HashedString::Hash(const char * i_string)
		{
			EAE6320_ASSERT(i_string != NULL);

			return Hash(reinterpret_cast<void *>(const_cast<char *>(i_string)), static_cast<uint32_t>(strlen(i_string)));
		}

		uint32_t HashedString::Hash(const void * i_bytes, uint32_t i_bytecount)
		{
			// FNV hash, http://isthe.com/chongo/tech/comp/fnv/

			register const unsigned char * p = static_cast<const unsigned char *>(i_bytes);
			uint32_t hash = 2166136261;

			for (uint32_t i = 0; i < i_bytecount; ++i)
				hash = 16777619 * (hash ^ p[i]);

			return hash ^ (hash >> 16);
		}
	}
}