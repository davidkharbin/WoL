#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
void sendPacket(string);

int main()
{

	string targetMAC = "d4:5d:64:b4:19:ee"; // Replace with the MAC address of the target machine

	// hard-coding the MAC for testing
	// cin >> targetMAC;

	sendPacket(targetMAC);
	return 0;
}

// Take MAC address and send Wake-on-LAN packet
void sendPacket(string macAddress)
{
	// Validate MAC address format: "XX:XX:XX:XX:XX:XX"
	unsigned char mac[6];
	if (sscanf(macAddress.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
						 &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6)
	{
		cerr << "Invalid MAC address format" << endl;
		return;
	}

	// Create magic packet: 6 bytes of 0xFF followed by 16 repetitions of the target MAC address
	unsigned char magicPacket[102];
	memset(magicPacket, 0xFF, 6);
	for (int i = 0; i < 16; ++i)
	{
		memcpy(&magicPacket[6 + i * 6], mac, 6);
	}

	// Create UDP socket
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		cerr << "Error opening socket" << endl;
		return;
	}

	// Set socket options to allow broadcast
	int broadcastEnable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
	{
		cerr << "Error setting socket options" << endl;
		close(sockfd);
		return;
	}

	// Set destination address and port for the broadcast
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9);														 // WoL (should) uses port 9
	addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Broadcast IP address

	// Send the magic packet
	if (sendto(sockfd, magicPacket, sizeof(magicPacket), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		cerr << "Error sending Wake-on-LAN packet" << endl;
		close(sockfd);
		return;
	}

	close(sockfd);
	cout << "Wake-on-LAN packet sent successfully to MAC address: " << macAddress << endl;
}
