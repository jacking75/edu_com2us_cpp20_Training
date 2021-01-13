/** 
 *  @file  		NFServerLink.h
 *  @brief 		Server To Server �� Connect ��ü
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

namespace NaveNetLib {

	// NFServerLink�� Server�� NFServerCtrl�� Connection�� �ϱ����� ���۵� ����ü�̴�
	// NFServerLink�� ������ Create�� Server�� Connection�ϰ� �Ѵ�.

	class NFServerLink : public NFConnection
	{
	public:
		NFServerLink(void) = default;
		virtual ~NFServerLink(void) = default;

	public:
		virtual void		Clear() { return; };

		virtual bool		Close_Open(bool bForce=false );

		// �� ��ü ���� 
		virtual bool		Create(	DWORD			dwIndex,
									HANDLE			hIOCP,
									NFPacketPool*	pPacketPool,
									wchar_t*			sIP,
									int				nPort,
									int				nMaxBuf);

		virtual bool		SendOpenPacket() { return true; };

	protected:
		// Socket�� ������ ���� �� ���Ĺ��� ���� ����  
		virtual bool		Open();

		// ���� ó�� ���� �Լ� 
		virtual bool			DispatchPacket( LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager);

		// ���� ��Ŷ�� ó���ϴ� �κ��̴�.
		virtual void		DispatchPacket( NFPacket& Packet ) 
		{
			// Nave::Sync::CSSync Live(&m_Sync);
		};

	protected:
		wchar_t	m_strIP[32];
		int		m_iPort;
	};

}