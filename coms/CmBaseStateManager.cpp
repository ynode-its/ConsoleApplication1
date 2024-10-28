/*--------------------------------------------------------------------------*
 *	ファイル名	： CmBaseStateManager.cpp
 *	概要		： 状態管理基底クラス
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * インクルード
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "CmBase.h"
#include "CmBaseStateManager.h"
#include "CmBaseState.h"

/*--------------------------------------------------------------------------*
 * 関数名   ： CCmBaseStateManager
 * 機能     ： コンストラクタ
 * 戻り値   ： なし
 * 引数1    ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseStateManager::CCmBaseStateManager(  CCmBaseControl* pCtl )
{
	assert( pCtl );

	m_pCtl = pCtl;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： ~CCmBaseStateManager
 * 機能     ： デストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseStateManager::~CCmBaseStateManager()
{
}

/*--------------------------------------------------------------------------*
 * 関数名   ： StateChange
 * 機能     ： 状態遷移
 * 引数		： ステータスID
 * 戻り値   ： 状態名称
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
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
 *						ファイル(CmBaseStateManager.cpp)の終わり
 *--------------------------------------------------------------------------*/
