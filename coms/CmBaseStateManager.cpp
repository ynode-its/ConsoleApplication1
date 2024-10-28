/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmBaseStateManager.cpp
 *	�T�v		�F ��ԊǗ����N���X
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * �C���N���[�h
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "CmBase.h"
#include "CmBaseStateManager.h"
#include "CmBaseState.h"

/*--------------------------------------------------------------------------*
 * �֐���   �F CCmBaseStateManager
 * �@�\     �F �R���X�g���N�^
 * �߂�l   �F �Ȃ�
 * ����1    �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseStateManager::CCmBaseStateManager(  CCmBaseControl* pCtl )
{
	assert( pCtl );

	m_pCtl = pCtl;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F ~CCmBaseStateManager
 * �@�\     �F �f�X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseStateManager::~CCmBaseStateManager()
{
}

/*--------------------------------------------------------------------------*
 * �֐���   �F StateChange
 * �@�\     �F ��ԑJ��
 * ����		�F �X�e�[�^�XID
 * �߂�l   �F ��Ԗ���
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
LPCSTR CCmBaseStateManager::StateChange( int stateId )
{
	int idx = 0;
	CCmBaseState** pStateEntries = GetStateEntries();

	if ( NULL != pStateEntries )
	{
		while ( NULL != pStateEntries[ idx ] )
		{
			if ( stateId == pStateEntries[ idx ]->GetStateId() )
			{
				SetState( pStateEntries[ idx ] );
				m_pCtl->OnStateChange( this->GetModuleId(), pStateEntries[ idx ]->GetStateId() );
				return pStateEntries[ idx ]->GetName();
			}
			idx++;
		}
	}
	return NULL;
}

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmBaseStateManager.cpp)�̏I���
 *--------------------------------------------------------------------------*/
