#ifndef EAE6320_HASHEDSTRING_INL_H
#define EAE6320_HASHEDSTRING_INL_H

#include <cstring>

namespace eae6320
{
	namespace StringHandler
	{
		inline HashedString::HashedString() :
			m_Hash(Hash(""))
		{
		}

		inline HashedString::HashedString(const char * i_string) :
			m_Hash(Hash(i_string))
#ifdef DEBUG_KEEP_STRING
			, m_pString(_strdup(i_string))
#endif
		{
		}

		inline HashedString::HashedString(const HashedString & i_other) :
			m_Hash(i_other.m_Hash)
#ifdef DEBUG_KEEP_STRING
			, m_pString(_strdup(i_other.m_pString))
#endif
		{
		}

		inline HashedString::~HashedString()
		{
#ifdef DEBUG_KEEP_STRING
			if (m_pString)
			{
				delete m_pString;
			}
#endif

		}

		inline HashedString & HashedString::operator=(const HashedString & i_other)
		{
			m_Hash = i_other.m_Hash;

#ifdef DEBUG_KEEP_STRING
			if (m_pString)
			{
				delete m_pString;
			}

			m_pString = i_other.m_pString;
#endif

			return *this;
		}

		inline uint32_t HashedString::GetHash(void) const
		{
			return m_Hash;
		}

#ifdef DEBUG_KEEP_STRING
		inline const char * HashedString::GetString() const
		{
			return m_pString;
		}
#endif

		inline bool HashedString::operator==(const HashedString & i_other) const
		{
			return m_Hash == i_other.m_Hash;
		}

		inline bool HashedString::operator!=(const HashedString & i_other) const
		{
			return m_Hash != i_other.m_Hash;
		}
		inline bool HashedString::operator<(const HashedString & i_other) const
		{
			return m_Hash < i_other.m_Hash;
		}
	}
}
#endif // !EAE6320_HASHEDSTRING_INL_H

