#include <iostream>
using namespace std;

void sendRequest();
void printInitPrompt();

string getTargetMAC();

int main()
{

	printInitPrompt();

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

void sendRequest()
{
}