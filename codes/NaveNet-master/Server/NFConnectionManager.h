/** 
 *  @file  		NFConnectionManager.h
 *  @brief 		Connection�� �����ϴ� ������ü
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

//#include <Nave/NFSync.h>
#include <mutex>
#include <vector>

#include "NFPacket.h"


namespace NaveNetLib {

	class NFConnection;

	/** 
	 *  @class        NFConnectionManager
	 *  @brief        Ư���� Ŀ�ؼ� ����Ʈ�� �����ϱ� ���� �޴��� ��ü ���ÿ� �������� ����� �ִ�.
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-08-28
	 */
	class NFConnectionManager
	{
	public:
		NFConnectionManager(void);
		~NFConnectionManager(void);

	public:
		/**
		 * @brief	�޴��� �ʱ�ȭ
		 * @param iMaxCount ���� ������ �ִ� Ŀ�ؼ� ����
		 */
		void Init(int iMaxCount);

		/**
		 * @brief	������ ��Ͻ�Ų��.
		 * @param pUser ������ ���ᰴü ������
		 */
		void Join(NFConnection* pUser);

		/**
		 * @brief	�ش� �ε����� ��ü�� �����Ѵ�.
		 * @param iIndex ������ �ε��� 
		 */
		void Levae(int iIndex);
		
		/**
		 * @brief	���� ��ϵ� ���ᰴü ���� ���
		 * @return  Ŀ�ؼ� ����
		 */
		int	GetCount() { return m_iCount; }

		/**
		 * @brief	�����ϴ� ���ᰴü �ִ� ����
		 * @return  �ִ� ���ᰴü ����
		 */
		int GetMaxCount() { return m_iMaxCount; }

		/**
		 * @brief	��ϵ� ��� ���ᰴü�� ��Ŷ�� �����Ѵ�.
		 * @param pPackte ���� ��Ŷ ����
		 * @param Len ���� ��Ŷ ������
		 */
		virtual void		SendPostAll( char* pPackte, int Len );

		/**
		 * @brief	��ϵ� ��� ���ᰴü�� ��Ŷ�� �����Ѵ�.
		 * @param Packet ���� ��Ŷ ����ü
		 */
		virtual void		SendPostAll( NFPacket&	Packet);

		//void Draw();

	private:
		/// ���ᰴü ���� ����
		std::vector<NFConnection*>						m_vecConn;
		typedef std::vector<NFConnection*>::iterator	iterConn;

		/// ���� ��ϵ� ���ᰴü�� ����
		int												m_iCount;
		/// ������ ������ �ִ� ���ᰴü�� ����
		int												m_iMaxCount;

		/// Sync ��ü
		std::mutex									m_Lock;
	};

}