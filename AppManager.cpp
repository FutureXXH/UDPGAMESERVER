#include "AppManager.h"
#include <time.h>
#include <mutex>
#include <future>
mutex mtx,mtx2;
APPManager::APPManager()
{
	int head20001 = 20001;
	std::memset(gamesyncdata, 0, sizeof(gamesyncdata));
	memcpy(gamesyncdata, (char*)&head20001, 4);

}

void gamesyncfuture(SOCKET serversock, APPManager* appmanager,int frame)
{
	
	/*char* buf = new char[gamesyncdata.size() * sizeof(DATA) + 9];
	memcpy(buf, "20001", 5);*/
	//itoa(frame, gamesyncdata + 4,10);

	clock_t first, second;
	double dur;

	mtx.lock();

	memcpy(appmanager->gamesyncdata + 4, (char*)&frame, 4);
	memcpy(appmanager->gamesyncdata + 8, (char*)&appmanager->gamesyncdatasize, 4);
	memcpy(appmanager->everyframedata[frame], appmanager->gamesyncdata, DATASIZE);
	std::memset(appmanager->gamesyncdata + 4, 0, sizeof(appmanager->gamesyncdata) - 4);
	appmanager->gamesyncdatasize = 12;
	mtx.unlock();
	
	mtx2.lock();

	first = clock();
	for (auto iter = appmanager->players.begin(); iter != appmanager->players.end(); iter++)
	{
		if (iter->second.frame < frame) 
		{
			
			int size = (frame - iter->second.frame) * DATASIZE;
			int sendsize = 8; // 20002+大小
			char* framepack = new char[size + 8];
			int head20002 = OPERATIONSYNCPACK;
			memcpy(framepack, (char*)&head20002, 4);
			memcpy(framepack + 4, (char*)&size, 4);

			//循环加载没有的帧
			for (int i = 0; i < (frame - iter->second.frame); i++)
			{
				memcpy(framepack + 8 + i * DATASIZE, appmanager->everyframedata[frame - (frame - iter->second.frame) + i], DATASIZE);
				sendsize += DATASIZE;
			}
			//发送没有的帧
			SERVERPRINT_INFO << "缺帧发送:" << "|" << sendsize << "|" << size << endl;

			sendto(serversock, framepack, sendsize, 0, (SOCKADDR*)&iter->second.Addr, sizeof(iter->second.Addr));


			delete[] framepack;
		}
		


	}
	second = clock();
	dur = (double)(second - first);
	cout << dur << endl;
	mtx2.unlock();
	

}

// 20001+大小+数据
void APPManager::gamesync(SOCKET serversock)
{
	DWORD  start;
	start = GetTickCount();
	SERVERPRINT_INFO << "帧同步线程已开启" << endl;
	while (true)
	{

		if (GetTickCount() - start > 100 )
		{
			string aa = std::to_string(frame);
			aa += "当前玩家:";
			aa += std::to_string(players.size());
			system(("title " + aa).c_str());
		
				auto accumulate_future = std::async(std::launch::async, gamesyncfuture, serversock, this, frame);
				
			
			
			start = GetTickCount();
			frame++;
		}
		
	     
		
	}
	
}


void APPManager::heartcheck()
{

	SERVERPRINT_INFO << "心跳同步线程已开启" << endl;
	while (true)
	{
		SERVERPRINT_INFO << "当前玩家数量" << players.size() << endl;
		for (auto iter = players.begin(); iter != players.end(); iter++)
		{
			iter->second.endtime = clock() / CLOCKS_PER_SEC;
			if (iter->second.endtime - iter->second.starttime > 5)
			{
				SERVERPRINT_INFO << "连接超时 正在删除玩家" << inet_ntoa(iter->second.Addr.sin_addr) << endl;
				mtx2.lock();
				players.erase(iter->first);
				mtx2.unlock();
				break;
			}

		}




		Sleep(1000);
	}

}

void APPManager::head10002(int &size, char* data, SOCKADDR_IN &playeraddr, SOCKET &serversock)
{

	int id = -1;

	memcpy((char*)&id, data +8, 4);

	if (players.find(id) != players.end())
	{
		players[id].starttime = clock() / CLOCKS_PER_SEC;
		
		sendto(serversock, data, size, 0, (SOCKADDR*)&playeraddr, sizeof(playeraddr));
	}

}

int len = -2;
void APPManager::response(int size, char* data, SOCKADDR_IN &playeraddr,SOCKET &serversock)  // 
{
	SERVERPRINT_INFO << "发现数据" << size << endl;
	int head = 0;
	memcpy((char*)&head, data, 4);
	int id = -1;
	int datasize = -1;
	memcpy((char*)&id, data + 8, 4);
	
	switch (head)
	{
	case CONNECT: // 10001+ 大小 + 玩家id
		SERVERPRINT_INFO << "发现连接" << inet_ntoa(playeraddr.sin_addr) << endl;
	
		memcpy((char*)&datasize, data + 4, 4);
		
		if (id == -1)
		{
			return;
		}
		playerdata tempdata;
		tempdata.Addr = playeraddr;
		tempdata.playerid = id;
		tempdata.starttime = clock() / CLOCKS_PER_SEC;
		tempdata.endtime = clock() / CLOCKS_PER_SEC;
		tempdata.frame = 0;
		players[id] = tempdata;
		{
		int o = 1;
		mtx.lock();
		memcpy(tempchar, (char*)&id, 4);
		memcpy(tempchar + 4, (char*)&o, 4);
	
		memcpy(gamesyncdata + gamesyncdatasize, tempchar, 12);
		gamesyncdatasize += 8;
		
		mtx.unlock();
		}

	     len = sendto(serversock, data, size, 0, (SOCKADDR*)&playeraddr, sizeof(playeraddr));
		SERVERPRINT_INFO << "发送数据" << len << endl;
		break;
	case HEART: // 10002+ 大小 + 玩家id+帧

		
		
		{
		int tempframe = 0;
		memcpy((char*)&tempframe, data + 12, 4);
		if (tempframe > players[id].frame)
		{
			players[id].frame = tempframe;
		}
		head10002(size, data, playeraddr, serversock);
		}
		break;
	case OPERATIONSYNC: // 20001+ 大小 + 玩家id +当前帧 + 操作（int）
		//读取玩家当前帧
	{
		int playernowframe;
		
		memcpy((char*)&playernowframe, data + 12, 4);
		if (playernowframe > players[id].frame)
		{
			players[id].frame = playernowframe;
		}
		

		
	
		//SERVERPRINT_INFO << "操作帧" << playernowframe << endl;
		//保存操作数据
		mtx.lock();
		memcpy(gamesyncdata + gamesyncdatasize, data + 8, 4);
		memcpy(gamesyncdata + gamesyncdatasize+4, data + 16, 4);
		gamesyncdatasize += 8;
	
		mtx.unlock();
	}

		//gamesyncdata.push_back(data);
		break;
	default:
		SERVERPRINT_INFO << "未知数据" << len << endl;
		break;
	}
}


