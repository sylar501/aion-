#include <shared/network/Packet.h>

namespace shared
{
	namespace network
	{
		Packet::Packet()
		{
			Write<uint16_t>(0); // Placeholder for size.
		}

		Packet::~Packet()
		{

		}

		uint8_t* Packet::GetDataPtr()
		{
			return m_vData.data();
		}

		uint32_t Packet::GetSize()
		{
			return (uint32_t) m_vData.size();
		}

		void Packet::Reserve(uint32_t u32Size)
		{
			m_vData.reserve(u32Size);
		}

		void Packet::Resize(uint32_t u32Size)
		{
			m_vData.resize(u32Size);
		}

		void Packet::HexDump()
		{
			uint8_t* aData = m_vData.data();

			int i, j;
			for (i = 0; i < m_vData.size(); i += 16)
			{
				printf("%06x: ", i);
				for (j = 0; j < 16; j++)
					if (i + j < m_vData.size())
						printf("%02x ", aData[i + j]);
					else
						printf("   ");
				printf(" ");
				for (j = 0; j < 16; j++)
					if (i + j < m_vData.size())
						printf("%c", isprint(aData[i + j]) ? aData[i + j] : '.');
				printf("\n");
			}
		}

		uint32_t Packet::GetPosition()
		{
			return m_u32Position;
		}

		void Packet::SetPosition(uint32_t u32Position)
		{
			m_u32Position = u32Position;
		}

		std::string Packet::ReadString()
		{
			uint32_t u32StringLength = Read<uint32_t>();

			if (u32StringLength == 0) 
				return std::string();

			// Check whether the string defined by specified size is actually available in the packet.
			// If not, read what's available.
			if (m_u32Position + u32StringLength > m_vData.size())
				u32StringLength = (uint32_t) m_vData.size() - m_u32Position;

			std::string strValue = std::string((char*)&m_vData[m_u32Position], u32StringLength);

			m_u32Position += u32StringLength;

			return strValue;
		}

		void Packet::WriteBytes(uint8_t* pBytes, uint32_t u32Length)
		{
			size_t u64Size = m_vData.size();

			if (m_u32Position + u32Length > u64Size)
				m_vData.resize(u64Size + u32Length);

			memcpy(&m_vData[m_u32Position], pBytes, u32Length);

			m_u32Position += u32Length;
		}

		void Packet::WriteString(const std::string& strValue)
		{
			uint32_t u32StringLength = (uint32_t) strValue.length();

			Write<uint32_t>(u32StringLength);
			WriteBytes((uint8_t*)strValue.c_str(), u32StringLength);

			m_u32Position += u32StringLength;
		}

		void Packet::WriteZeroes(uint32_t u32Count)
		{
			size_t u64Size = m_vData.size();

			if (m_u32Position + u32Count > u64Size)
				m_vData.resize(u64Size + u32Count);

			memset(&m_vData[m_u32Position], 0, u32Count);

			m_u32Position += u32Count;
		}

		TCPConnection* Packet::GetConnection()
		{
			return m_pConnection;
		}

		void Packet::SetConnection(TCPConnection* pConnection)
		{
			m_pConnection = pConnection;
		}
	}
}