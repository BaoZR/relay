#include<Windows.h>
#include<iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<thread>
#include<map>
#include "usb_relay_device.h"
#include <cstdlib>

#include "main.h"



void parseLine(std::string& line, std::map<std::string, int>& itemMap)
{
	auto pos = line.find('=');
	if (pos != std::string::npos)
	{
		std::string key = line.substr(0, pos);
		std::string numberStr = line.substr(pos + 1, 7);
		int value = std::stoi(numberStr);
		if (value < 0)
		{
			value = -value;
		}
		itemMap.insert(std::make_pair(key, value));
	}
}

bool getConfig(int& onTime, int& offTime,int& randomPercentage,char* path)
{
	std::string rootPath = std::string(path);

	auto pos = rootPath.rfind('\\');
	if (pos == std::string::npos)
	{
		return false;
	}
	std::string configPath = rootPath.substr(0, pos) + "\\" +"config.ini";
	std::ifstream configInput(configPath);
	if (!configInput)
	{
		std::cerr << "error opening config file" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(configInput,line)) {
		std::map<std::string, int> itemMap;
		parseLine(line, itemMap);

		auto it = itemMap.begin();
		if (it != itemMap.end())
		{
			if ((it->first) == std::string("onTime"))
			{
				onTime = it->second;
			}
			else if (it->first == std::string("offTime"))
			{
				offTime = it->second;
			}
			else if (it->first == std::string("randomPercentage"))
			{
				if(it->second > 90)
				{
					it->second = 90;
				}
				else if (it -> second < 0)
				{
					it->second = 0;
				}

				randomPercentage = it->second;
			}
		}
	}
	return true;
}

int plusOrMinusSign() {
	int num = (rand() % 2);
	if (num == 1)
	{
		return 1;
	}
	return -1;
}

int getElapsedTime(int time,int percentage)
{
	int calcTime = 0;
	if (percentage == 0)
	{
		calcTime = time;
	}
	else
	{
		calcTime = time + ((time * plusOrMinusSign() * (rand() % percentage)) / 100);
	}
	return calcTime;
}

int main(int argc,char* argv[])
{
	srand(time(NULL));
	int onTime = 2000;
	int offTime = 1000;
	int randomPercentage = 0;
	struct usb_relay_device_info* pDevice = nullptr;

	if(!getConfig(onTime, offTime, randomPercentage,argv[0]))
	{
		system("pause");
		return 0;
	}

	std::cout << "onTime:" << onTime << std::endl;
	std::cout << "offTime:" << offTime << std::endl;
	std::cout << "randomPercentage:" << randomPercentage << std::endl;

	if (usb_relay_init() != 0)
	{
		system("pause");
		return 0;
	}

	pDevice = usb_relay_device_enumerate();

	if (pDevice == nullptr)
	{
		std::cout << "no device" << std::endl;
		usb_relay_exit();
		system("pause");
		return 0;
	}
	
	int handle = usb_relay_device_open(pDevice);
	
	if (handle == NULL)
	{
		std::cout << "open fail" << std::endl;
		usb_relay_exit();
		system("pause");
		return 0;
	}
	int times = 0;
	int elapsedTime = 0;
	while (1)
	{
		times++;
		std::cout << times << " time" << std::endl;
		usb_relay_device_open_all_relay_channel(handle);
		//std::this_thread::sleep_for(std::chrono::seconds(15 - rand() % 10));

		//int tttt = rand() / 10;

		elapsedTime = getElapsedTime(onTime, randomPercentage);
		std::cout << "real onTime:" << elapsedTime << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(elapsedTime));

		//nrand = rand();
		//while (nrand >6000)
		//{
			//nrand = rand();
			//std::cout << nrand << std::endl;
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		//}


		//std::this_thread::sleep_for(std::chrono::milliseconds(1000-rand()%1000));
		usb_relay_device_close_all_relay_channel(handle);
		elapsedTime = getElapsedTime(offTime, randomPercentage);
		std::cout << "real offTime:" << elapsedTime << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(elapsedTime));
	}

	

	usb_relay_device_close(handle);
	usb_relay_exit();
	
	return 0;
}