#include <iostream>
#include <thread>

#include "ServerCtrl.h"
#include "Config.h"

int main()
{
	ServerCtrl Server;
		
	Config config;
	config.Load();
		
	Server.SetConfig(&config);

	Server.StartServer();
	
	std::cout << "Press any key to continue..." << std::endl;
	getchar();
	
	Server.EndServer();
	// �ֿܼ��� exit ������ �������������Ѵ�
	/*while (true)
	{
		std::this_thread::yield();
	}*/

	return 0;
}