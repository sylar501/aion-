#ifndef AION_SHARED_NETWORK_PACKET_H
#define AION_SHARED_NETWORK_PACKET_H

#include <vector>
#include <stdint.h>
#include <string>
#include <memory>

namespace shared
{
	namespace network
	{
		class TCPConnection;

		class Packet
		{
		public:
			Packet();
			virtual ~Packet();

			uint8_t*						GetDataPtr();
			uint32_t						GetSize();

			// Increases the total number of bytes that the packet can hold without requiring reallocation.
			void							Reserve(uint32_t u32Size);

			// Forcefully resize the packet storage to the specified size.
			void							Resize(uint32_t u32Size);

			// Writes a string representation of this packet's contents to the standard output.
			void							HexDump();

			// Sets the read/write position within this packet.
			uint32_t						GetPosition();
			// Returns the read/write position within this packet.
			void							SetPosition(uint32_t u32Position);

			template<typename T>
			T								Read()
			{
				// Check that we have enough data to read the specified type.
				if (m_u32Position + sizeof(T) > m_vData.size())
					return T();
				T val = *((T*)&m_vData[m_u32Position]);
				m_u32Position += sizeof(T);
				return val;
			}

			// Reads a string from the packet in AION format.
			std::string						ReadString();

			template<typename T>
			void							Write(T val)
			{
				WriteBytes((uint8_t*) &val, sizeof(T));
			}

			// Writes the specified bytes into the packet, reallocating memory if required.
			void							WriteBytes(uint8_t* pBytes, uint32_t u32Length);

			// Writes the specified string into the packet in AION format.
			void							WriteString(const std::string& strValue);

			// Writes the specified amount of zeroes into the packet.
			void							WriteZeroes(uint32_t u32Count);

			std::shared_ptr<TCPConnection>	GetConnection();
			void							SetConnection(std::shared_ptr<TCPConnection> spConnection);

		protected:
			std::vector<uint8_t>			m_vData;
			uint32_t						m_u32Position = 0;
			std::shared_ptr<TCPConnection>	m_spConnection = nullptr;
		};
	}
}

#endif