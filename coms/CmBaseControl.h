/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmBaseControl.h
 *	�T�v		�F �R���g���[�����w�b�_
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

#ifndef _CMBASECONTROL_H_
#define _CMBASECONTROL_H_

class CCmBaseControl;

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmBaseControl
 *  �p���N���X  �F CCmBase
 *	�T�v		�F �A�v���P�[�V��������N���X
 *  �쐬��      �F ITS Y.Node
 *  �쐬��      �F 2015.06.15
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmBaseControl : public CCmBase
{
public:
	CCmBaseControl();
	virtual ~CCmBaseControl();

	virtual void OnStateChange( int moduleid, int state );
};

#endif	// End of _CMBASECONTROL_H_

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmBaseControl.h)�̏I���
 *--------------------------------------------------------------------------*/
