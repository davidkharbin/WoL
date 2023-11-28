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

// Frame ( 102 bytes total ) to be sent as an ethernet broadcast message -
// Payload to be six bytes of all 255 and sixteen repeating MAC address
string createMagicPacket(string mac)
{
	string payload;

	return payload;
}

void sendRequest(string payload)
{
	// Broadcast a network request with the "magic packet"
}