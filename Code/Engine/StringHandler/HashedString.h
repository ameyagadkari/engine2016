#ifndef EAE6320_HASHEDSTRING_H
#define EAE6320_HASHEDSTRING_H

#ifdef _DEBUG
//#define DEBUG_KEEP_STRING
#endif // _DEBUG


#include <cstdint>
namespace eae6320
{
	namespace StringHandler
	{
		class HashedString
		{
		public:
			inline HashedString();
			inline ~HashedString();

			inline HashedString(const char * i_string);
			inline HashedString(const HashedString & i_other);
			inline HashedString & operator=(const HashedString & i_other);

			inline uint32_t GetHash() const;
#ifdef DEBUG_KEEP_STRING
			inline const char * GetString() const;
#endif

			inline bool operator==(const HashedString & i_other) const;
			inline bool operator!=(const HashedString & i_other) const;
			inline bool operator<(const HashedString & i_other) const;

			static uint32_t Hash(const char * i_string);
			static uint32_t Hash(const void * i_bytes, uint32_t i_count);
		private:
			uint32_t 	m_Hash;

#ifdef DEBUG_KEEP_STRING
			const char * 	m_pString;
#endif
		};
	}
}
#include "HashedString_inl.h"

#endif // !EAE6320_HASHEDSTRING_H