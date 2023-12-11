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
#include <fstream>

using namespace std;
void sendPacket(string);
void saveTargetMAC(const vector<string> &targetMACs, const string &filename);
vector<string> loadTargetMAC(const string &filename);

int main()
{
	vector<string> savedMACs;
	string filename = "targetMACs.txt"; // File to store target MACs

	// Load target MACs from file
	savedMACs = loadTargetMAC(filename);

	// Offer options to the user
	cout << "Choose a target MAC address:" << endl;
	for (size_t i = 0; i < savedMACs.size(); ++i)
	{
		cout << i + 1 << ". " << savedMACs[i] << endl;
	}
	cout << "Enter your choice (number) or enter a new MAC address: ";

	int choice;
	cin >> choice;
	cin.ignore(); // Clear newline character from input buffer

	string selectedMAC;
	if (choice > 0 && choice <= savedMACs.size())
	{
		selectedMAC = savedMACs[choice - 1];
	}
	else
	{
		cout << "Enter a new MAC address (format: XX:XX:XX:XX:XX:XX): ";
		cin >> selectedMAC;
		savedMACs.push_back(selectedMAC);		// Add new MAC to the list
		saveTargetMAC(savedMACs, filename); // Save updated list to file
	}

	sendPacket(selectedMAC);
	return 0;
}

// Function to save target MAC addresses to a file
void saveTargetMAC(const vector<string> &targetMACs, const string &filename)
{
	ofstream file(filename);
	if (file.is_open())
	{
		for (const string &mac : targetMACs)
		{
			file << mac << endl;
		}
		file.close();
		cout << "Target MAC addresses saved to " << filename << endl;
	}
	else
	{
		cerr << "Unable to open file: " << filename << endl;
	}
}

// Function to load target MAC addresses from a file
vector<string> loadTargetMAC(const string &filename)
{
	vector<string> targetMACs;
	ifstream file(filename);
	if (file.is_open())
	{
		string line;
		while (getline(file, line))
		{
			targetMACs.push_back(line);
		}
		file.close();
	}
	else
	{
		cerr << "Unable to open file: " << filename << endl;
	}
	return targetMACs;
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
