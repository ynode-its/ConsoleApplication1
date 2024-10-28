/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmBaseStateManager.h
 *	�T�v		�F ��Ԋ��w�b�_
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

#ifndef _CMBASESTATEMANAGER_H_
#define _CMBASESTATEMANAGER_H_

class CCmBaseState;

#define D_EVENTINFO_BREAK		(0x00000001)
#define D_EVENTINFO_01			(0x00000002)
#define D_EVENTINFO_02			(0x00000004)
#define D_EVENTINFO_03			(0x00000008)
#define D_EVENTINFO_04			(0x00000010)
#define D_EVENTINFO_05			(0x00000020)
#define D_EVENTINFO_06			(0x00000040)
#define D_EVENTINFO_07			(0x00000080)
#define D_EVENTINFO_08			(0x00000100)
#define D_EVENTINFO_09			(0x00000200)
#define D_EVENTINFO_10			(0x00000400)
#define D_EVENTINFO_11			(0x00000800)
#define D_EVENTINFO_12			(0x00001000)
#define D_EVENTINFO_13			(0x00002000)
#define D_EVENTINFO_14			(0x00004000)
#define D_EVENTINFO_15			(0x00008000)
#define D_EVENTINFO_16			(0x00010000)
#define D_EVENTINFO_17			(0x00020000)
#define D_EVENTINFO_18			(0x00040000)
#define D_EVENTINFO_HIGHTWAY	(0x00080000)
#define D_EVENTINFO_ACTUAL		(0x00100000)
#define D_EVENTINFO_EVENTDATA	(0x0007FFFF)


#define D_EVENTINFO_HIGHTWAY_DATABIT		(19)
#define D_EVENTINFO_ACTUAL_DATABIT 			(20) 

/* ���H������`(0,1�ȊO�͈�ʓ��Ƃ��ď�������) */
#define	D_ATR_ROAD_EXP			0										//	�������H
#define	D_ATR_ROAD_DRV			1										//	�����Ԑ�p��
#define	D_ATR_ROAD_NAT			2										//	����
#define	D_ATR_ROAD_LOC			3										//	��v�n����
#define	D_ATR_ROAD_PRF			4										//	�s���{����
#define	D_ATR_ROAD_BAS			5										//	��{��
#define	D_ATR_ROAD_ETC			6										//	���̑�
#define	D_ATR_ROAD_FRY			7										//	�t�F���[�q�H
#define	D_ATR_ROAD_NON			8										//	��������
#define	D_ATR_ROAD_THN			9										//	�ד��H

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmBaseStateManager
 *  �p���N���X  �F CCmBase
 *	�T�v		�F ��ԑJ�ڂ��s���N���X
 *  �쐬��      �F ITS Y.Node
 *  �쐬��      �F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmBaseStateManager : public CCmBase
{
	CCmBaseControl* m_pCtl;
public:
	CCmBaseStateManager( CCmBaseControl* pCtl );
	virtual ~CCmBaseStateManager();

	// ��ԑJ��
	virtual LPCSTR StateChange( int stateId );
	virtual long RecvProcessMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;

	virtual int GetStateId() = 0;
	virtual int GetModuleId() = 0;

protected:
	// �X�e�[�^�X�G���g���|�C���^�擾
	virtual LPPCMBASESTATE GetStateEntries() = 0;
	virtual void SetState( void* pState ) = 0;
};

#endif	// End of _CMBASESTATEMANAGER_H_

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmBaseStateManager.h)�̏I���
 *--------------------------------------------------------------------------*/
