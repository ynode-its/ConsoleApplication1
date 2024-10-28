/*--------------------------------------------------------------------------*
 *	ファイル名	： CmBaseStateManager.h
 *	概要		： 状態基底ヘッダ
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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

/* 道路属性定義(0,1以外は一般道として処理する) */
#define	D_ATR_ROAD_EXP			0										//	高速道路
#define	D_ATR_ROAD_DRV			1										//	自動車専用道
#define	D_ATR_ROAD_NAT			2										//	国道
#define	D_ATR_ROAD_LOC			3										//	主要地方道
#define	D_ATR_ROAD_PRF			4										//	都道府県道
#define	D_ATR_ROAD_BAS			5										//	基本道
#define	D_ATR_ROAD_ETC			6										//	その他
#define	D_ATR_ROAD_FRY			7										//	フェリー航路
#define	D_ATR_ROAD_NON			8										//	属性無し
#define	D_ATR_ROAD_THN			9										//	細道路

/*--------------------------------------------------------------------------*
 *  クラス名    ： CCmBaseStateManager
 *  継承クラス  ： CCmBase
 *	概要		： 状態遷移を行うクラス
 *  作成者      ： ITS Y.Node
 *  作成日      ： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/
class CCmBaseStateManager : public CCmBase
{
	CCmBaseControl* m_pCtl;
public:
	CCmBaseStateManager( CCmBaseControl* pCtl );
	virtual ~CCmBaseStateManager();

	// 状態遷移
	virtual LPCSTR StateChange( int stateId );
	virtual long RecvProcessMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;

	virtual int GetStateId() = 0;
	virtual int GetModuleId() = 0;

protected:
	// ステータスエントリポインタ取得
	virtual LPPCMBASESTATE GetStateEntries() = 0;
	virtual void SetState( void* pState ) = 0;
};

#endif	// End of _CMBASESTATEMANAGER_H_

/*--------------------------------------------------------------------------*
 *						ファイル(CmBaseStateManager.h)の終わり
 *--------------------------------------------------------------------------*/
