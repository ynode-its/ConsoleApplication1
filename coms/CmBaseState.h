/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmBaseState.cpp
 *	�T�v		�F ��Ԋ��N���X
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

#ifndef _CMBASESTATE_H_
#define _CMBASESTATE_H_

class CCmBaseStateManager;
class CCmBaseState;

typedef CCmBaseState *LPCMBASESTATE;
typedef LPCMBASESTATE *LPPCMBASESTATE;

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmBaseState
 *  �p���N���X  �F CCmBase
 *	�T�v		�F ��ԋL���N���X
 *  �쐬��      �F ITS Y.Node
 *  �쐬��      �F 2015.06.15
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmBaseState : public CCmBase
{
protected:
	CCmBaseStateManager* m_pStateMgr;

public:
	CCmBaseState( CCmBaseStateManager* pMgr );
	virtual ~CCmBaseState();

	virtual int GetStateId() = 0;
	virtual LPCSTR GetName() = 0;
};

#endif	// End of _CMBASESTATE_H_

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmBaseState.h)�̏I���
 *--------------------------------------------------------------------------*/
