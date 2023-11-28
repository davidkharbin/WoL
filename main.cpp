#include <iostream>
using namespace std;

void printInitPrompt();
void sendRequest(string);

string getTargetMAC();
string createMagicPacket(string);

int main()
{
	string macAddress;
	string magicPacket;

	printInitPrompt();

	macAddress = getTargetMAC();
	magicPacket = createMagicPacket(macAddress);
	sendRequest(magicPacket);

	return 0;
}

void printInitPrompt()
{
	cout << "Wake On LAN" << endl;
	cout << "-----------" << endl;

	cout << "Enter the MAC address for the target machine: " << endl;
}

string getTargetMAC()
{
	string target;
	cin >> target;
	return target;
}

/**
 * A network frame (102 bytes total) to be sent as an ethernet broadcast message.
 * Payload to be six bytes of all 255 (FF:FF:FF:FF:FF:FF is the broadcast address of an ethernet network)
 * and sixteen repeating MAC addresses (the target's MAC address).
 */
string createMagicPacket(string mac)
{
	string payload;
	// take the MAC address and iterate 16 times, appending the broadcast address to it
	return payload;
}

void sendRequest(string payload)
{
	// Broadcast a network request with the "magic packet"
}