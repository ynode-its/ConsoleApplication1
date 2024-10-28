/*--------------------------------------------------------------------------*
 *	ファイル名	： CmBaseState.cpp
 *	概要		： 状態基底クラス
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

#ifndef _CMBASESTATE_H_
#define _CMBASESTATE_H_

class CCmBaseStateManager;
class CCmBaseState;

typedef CCmBaseState *LPCMBASESTATE;
typedef LPCMBASESTATE *LPPCMBASESTATE;

/*--------------------------------------------------------------------------*
 *  クラス名    ： CCmBaseState
 *  継承クラス  ： CCmBase
 *	概要		： 状態記憶クラス
 *  作成者      ： ITS Y.Node
 *  作成日      ： 2015.06.15
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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
 *						ファイル(CmBaseState.h)の終わり
 *--------------------------------------------------------------------------*/
