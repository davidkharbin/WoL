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
void removeTargetMAC(vector<string> &targetMACs, int index, const string &filename);

int main()
{
	vector<string> savedMACs;
	string filename = "targetMACs.txt"; // File to store target MACs

	// Load target MACs from file
	savedMACs = loadTargetMAC(filename);

	cout << "Choose an option:" << endl;
	cout << "1. Select a target MAC address" << endl;
	cout << "2. Add a new MAC address" << endl;
	cout << "3. Remove a saved MAC address" << endl;
	cout << "Enter your choice (1, 2, or 3): ";

	int option;
	cin >> option;
	cin.ignore(); // Clear newline character from input buffer

	if (option == 1)
	{
		cout << "Choose a target MAC address:" << endl;
		for (size_t i = 0; i < savedMACs.size(); ++i)
		{
			cout << i + 1 << ". " << savedMACs[i] << endl;
		}
		cout << "Enter the index of the MAC address: ";
		int choice;
		cin >> choice;
		cin.ignore(); // Clear newline character from input buffer

		if (choice > 0 && choice <= savedMACs.size())
		{
			string selectedMAC = savedMACs[choice - 1];
			sendPacket(selectedMAC);
		}
		else
		{
			cerr << "Invalid choice." << endl;
		}
	}
	else if (option == 2)
	{
		cout << "Enter a new MAC address (format: XX:XX:XX:XX:XX:XX): ";
		string newMAC;
		cin >> newMAC;
		cin.ignore(); // Clear newline character from input buffer
		savedMACs.push_back(newMAC);
		saveTargetMAC(savedMACs, filename);
		cout << "New MAC address added successfully." << endl;
	}
	else if (option == 3)
	{
		cout << "Select a MAC address to remove:" << endl;
		for (size_t i = 0; i < savedMACs.size(); ++i)
		{
			cout << i + 1 << ". " << savedMACs[i] << endl;
		}
		cout << "Enter the index of the MAC address to remove: ";
		int index;
		cin >> index;
		cin.ignore();																		 // Clear newline character from input buffer
		removeTargetMAC(savedMACs, index - 1, filename); // Adjust index to match vector index
	}
	else
	{
		cerr << "Invalid choice." << endl;
	}

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

// Function to remove a target MAC address from the list
void removeTargetMAC(vector<string> &targetMACs, int index, const string &filename)
{
	if (index >= 0 && index < targetMACs.size())
	{
		targetMACs.erase(targetMACs.begin() + index);
		saveTargetMAC(targetMACs, filename);
		cout << "MAC address removed successfully." << endl;
	}
	else
	{
		cerr << "Invalid index." << endl;
	}
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
