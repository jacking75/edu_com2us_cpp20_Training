#pragma once

#include "../../Server/NFConnectionManager.h"
#include "../../Server/NFServerCtrl.h"
#include "../../Server/NFPacketPool.h"

#include "TestConnection.h"


class Config;

class ServerCtrl : public NaveNetLib::NFServerCtrl
{
public:
	ServerCtrl(void);
	~ServerCtrl(void);
	
public:	
	void SetConfig(Config* pConfig);

	void StartServer();

	void EndServer();

	//void						Update();									// Process ó�� �Լ�

	void						ShowServerInfo() override;							// ���۽� ȭ�鿡 ���� ���� ǥ��

	bool						CreateSession(unsigned long long sckListener);
	bool						ReleaseSession();

	//virtual void				InitializeCommand();

	// ����� ������Ʈ�� �ʱ�ȭ �մϴ�. (�����켳���� ����� �Ŀ� ȣ��)
	//virtual void				InitObject();
	// ����� ������Ʈ�� �����մϴ�. (���η����� ������(EndProcessȣ����) ȣ��)
	//virtual void				ReleaseObject();

public:
	void						UpdateInfo();
	void						SendMsg(char* strParam);


private:
	NaveNetLib::NFConnectionManager			m_NFConnectionManager;
	NaveNetLib::NFPacketPool		m_PacketPool;				// �޸� ó�� ���� 
	TestConnection*					m_pLogin;					// Client List ����

	unsigned int					m_iPrevTick;
	int								m_iMaxPacket;

	Config*							m_pConfig;
};