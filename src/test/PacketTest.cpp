#include <shared/network/Packet.h>

int main(int argc, char* argv[])
{
	shared::network::Packet pkt;

	pkt.Write<uint8_t>(0x01);
	pkt.Write<uint16_t>(0x02);
	pkt.Write<uint32_t>(0x04);
	pkt.Write<uint64_t>(0x02);
	pkt.Write<float>(2.0f);
	pkt.Write<double>(3.5f);
	pkt.WriteString("#Test#");

	pkt.HexDump();

	pkt.SetPosition(0);

	printf("%u\n", pkt.Read<uint8_t>());
	printf("%u\n", pkt.Read<uint16_t>());
	printf("%u\n", pkt.Read<uint32_t>());
	printf("%llu\n", pkt.Read<uint64_t>());
	printf("%f\n", pkt.Read<float>());
	printf("%f\n", pkt.Read<double>());
	printf("%s\n", pkt.ReadString().c_str());

	return 0;
}