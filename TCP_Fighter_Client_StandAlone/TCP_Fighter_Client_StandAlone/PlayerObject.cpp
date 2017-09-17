#include "stdafx.h"
#include "PlayerObject.hpp"


CPlayerObject::CPlayerObject()
{
	m_dwActionCur = dfACTION_STAND;
	m_dwActionOld = dfACTION_STAND;
}


CPlayerObject::~CPlayerObject()
{
}

DWORD CPlayerObject::Action(DWORD dwParam)
{
	ActionProc();
	NextFrame();
	
	return false;
}

void CPlayerObject::ActionProc(void)
{
	//�ܺο����� �׼� �Է��� �ܼ� �ɹ� ������ ���ø� ���ش�.
	//�׼ǰ������� �ﰢ ������ �ؾ� �ϱ� ������ ť ����� ����� �ʿ䰡 ����.
	//Ȥ�ó� ���⿡�� ���� ��������Ʈ ����, �ִϸ��̼� ����, ��ǥ�̵� ���� ���־ �ȵǸ� �׼� ���濡 ���� �������� ó����
	//Action() �Լ������� �ϵ��� �Ѵ�.


	//��� ���۽�(����)�� ��� ���������� �ش� ���� ó���� �Ϸ��ؾ߸� �Ѵ�.
	switch (m_dwActionCur)
	{
		//���ݵ��� �Ǵ� ������ ������ �ִϸ��̼��� ���� �� ���� ���������� �ִϸ��̼��� �Ǿ�߸� �ϸ�,
		//�ִϸ��̼��� ���� �� �⺻�������� �ڵ����� ���ư��� �Ѵ�.
	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:	
	case dfACTION_ATTACK3:
		{
			if (isEndFrame())
			{
				SetActionStand();
				
				//������ �����������, �׼��� �ٲ��༭ �������� ������ �� �� �� ������ �����ϵ��� �Ѵ�.
				m_dwActionInput = dfACTION_STAND;
			}
			break;
		}
	default:
		InputActionProc();
		break;
	}
}

DWORD CPlayerObject::Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	//�׸��� ���.
	if (GetObjectID() == 5)
		g_cSprite.DrawSpriteALPHA50(eSHADOW, GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch, 1);
	else
		g_cSprite.DrawSprite(eSHADOW, GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);


	//�÷��̾� ��ü ���
	if(GetObjectID() == 1)
		g_cSprite.DrawSprite(GetSprite(), GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);

	else if(GetObjectID() == 2)
		g_cSprite.DrawSpriteRED(GetSprite(), GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);

	else if(GetObjectID() == 3)
		g_cSprite.DrawSpriteGREEN(GetSprite(), GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);

	else if (GetObjectID() == 4)
		g_cSprite.DrawSpriteBLUE(GetSprite(), GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);

	else if (GetObjectID() == 5)
		g_cSprite.DrawSpriteALPHA50(GetSprite(), GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch, 2);

	//HP������ �� ���
	g_cSprite.DrawSprite(eGUAGE_HP, GetCurX() - 35, GetCurY() + 9, bypDest, iDestWidth, iDestHeight, iDestPitch);

	return true;
}

void CPlayerObject::SetActionStand(void)
{
	if (m_dwActionInput == dfACTION_STAND)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_STAND)
			{
				SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
			}
			m_dwActionCur = dfACTION_STAND;
			m_dwActionOld = m_dwActionCur;
		}
		else if (GetDirection() == dfDIR_RIGHT)
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_STAND)
			{
				SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
			}
			m_dwActionCur = dfACTION_STAND;
			m_dwActionOld = m_dwActionCur;
		}
	}
}

void CPlayerObject::InputActionProc(void)
{
	
	if (m_dwActionInput == dfACTION_MOVE_LL) //m_dwActionInput����� dfACTION_MOVE_LL�̸�.
	{
		SetDirection(dfDIR_LEFT); //������ LEFT�� SET.
		if (m_dwActionOld != dfACTION_MOVE_LL) //�׸��� ���� �׼��� dfACTION_MOVE_LL�� �ƴϸ�
		{	
			//���� ��������Ʈ�� ��´�.
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		}
		//���� �������� ����� dfACTION_MOVE_LL�� SET�ϰ�.
		m_dwActionCur = dfACTION_MOVE_LL;
		//���� ��ɿ� ���� ����� ����. �̰� ���������Ӻ��� �����.
		m_dwActionOld = m_dwActionCur;
		
		//�����ǰ� ����.
		SetCurPosition(max(dfRANGE_MOVE_LEFT, GetCurX() - dfSPEED_PLAYER_X), GetCurY());
	}

	else if (m_dwActionInput == dfACTION_MOVE_LU)
	{
		SetDirection(dfDIR_LEFT);
		if (m_dwActionOld != dfACTION_MOVE_LU)
		{
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		}
		m_dwActionCur = dfACTION_MOVE_LU;
		m_dwActionOld = m_dwActionCur;

		SetCurPosition(max(dfRANGE_MOVE_LEFT, GetCurX() - dfSPEED_PLAYER_X), 
						max(dfRANGE_MOVE_TOP,GetCurY() - dfSPEED_PLAYER_Y));
	}

	else if (m_dwActionInput == dfACTION_MOVE_UU)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_MOVE_UU)
			{
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			}
			m_dwActionCur = dfACTION_MOVE_UU;
			m_dwActionOld = m_dwActionCur;
		}
		else
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_MOVE_UU)
			{
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
			}
			m_dwActionCur = dfACTION_MOVE_UU;
			m_dwActionOld = m_dwActionCur;
		}

		SetCurPosition(GetCurX(), max(dfRANGE_MOVE_TOP, GetCurY() - dfSPEED_PLAYER_Y));
	}

	else if (m_dwActionInput == dfACTION_MOVE_RU)
	{
		SetDirection(dfDIR_RIGHT);
		if (m_dwActionOld != dfACTION_MOVE_RU)
		{
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		m_dwActionCur = dfACTION_MOVE_RU;
		m_dwActionOld = m_dwActionCur;

		SetCurPosition(min(dfRANGE_MOVE_RIGHT, GetCurX() + dfSPEED_PLAYER_X), 
						max(dfRANGE_MOVE_TOP, GetCurY() - dfSPEED_PLAYER_Y));
	}
	
	else if (m_dwActionInput == dfACTION_MOVE_RR)
	{
		SetDirection(dfDIR_RIGHT);
		if (m_dwActionOld != dfACTION_MOVE_RR)
		{
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		m_dwActionCur = dfACTION_MOVE_RR;
		m_dwActionOld = m_dwActionCur;

		SetCurPosition(min(dfRANGE_MOVE_RIGHT, GetCurX() + dfSPEED_PLAYER_X), GetCurY());
	}
	
	else if (m_dwActionInput == dfACTION_MOVE_RD)
	{
		SetDirection(dfDIR_RIGHT);
		if (m_dwActionOld != dfACTION_MOVE_RD)
		{
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		m_dwActionCur = dfACTION_MOVE_RD;
		m_dwActionOld = m_dwActionCur;

		SetCurPosition(min(dfRANGE_MOVE_RIGHT, GetCurX() + dfSPEED_PLAYER_X), 
						min(dfRANGE_MOVE_BOTTOM,GetCurY() + dfSPEED_PLAYER_Y));
	}

	else if (m_dwActionInput == dfACTION_MOVE_DD)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_MOVE_DD)
			{
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			}
			m_dwActionCur = dfACTION_MOVE_DD;
			m_dwActionOld = m_dwActionCur;
		}
		else
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_MOVE_DD)
			{
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
			}
			m_dwActionCur = dfACTION_MOVE_DD;
			m_dwActionOld = m_dwActionCur;
		}

		SetCurPosition(GetCurX(), min(dfRANGE_MOVE_BOTTOM, GetCurY() + dfSPEED_PLAYER_Y));
	}

	else if (m_dwActionInput == dfACTION_MOVE_LD)
	{
		SetDirection(dfDIR_LEFT);
		if (m_dwActionOld != dfACTION_MOVE_LD)
		{
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		}
		m_dwActionCur = dfACTION_MOVE_LD;
		m_dwActionOld = m_dwActionCur;

		SetCurPosition(max(dfRANGE_MOVE_LEFT,GetCurX() - dfSPEED_PLAYER_X), min(dfRANGE_MOVE_BOTTOM, GetCurY() + dfSPEED_PLAYER_Y));
	}

	else if (m_dwActionInput == dfACTION_JUMP_LL)
	{
	}

	else if (m_dwActionInput == dfACTION_JUMP_RR)
	{
	}

	else if (m_dwActionInput == dfACTION_JUMP_CC)
	{
	}

	else if (m_dwActionInput == dfACTION_ATTACK1)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_ATTACK1)
			{
				SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
			}
			m_dwActionCur = dfACTION_ATTACK1;
			m_dwActionOld = m_dwActionCur;
		}
		else if(GetDirection() == dfDIR_RIGHT)
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_ATTACK1)
			{
				SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);
			}
			m_dwActionCur = dfACTION_ATTACK1;
			m_dwActionOld = m_dwActionCur;
		}
	}

	else if (m_dwActionInput == dfACTION_ATTACK2)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_ATTACK2)
			{
				SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
			}
			m_dwActionCur = dfACTION_ATTACK2;
			m_dwActionOld = m_dwActionCur;
		}
		else
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_ATTACK2)
			{
				SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);
			}
			m_dwActionCur = dfACTION_ATTACK2;
			m_dwActionOld = m_dwActionCur;
		}
	}

	else if (m_dwActionInput == dfACTION_ATTACK3)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_ATTACK3)
			{
				SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
			}
			m_dwActionCur = dfACTION_ATTACK3;
			m_dwActionOld = m_dwActionCur;
		}
		else
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_ATTACK3)
			{
				SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);
			}
			m_dwActionCur = dfACTION_ATTACK3;
			m_dwActionOld = m_dwActionCur;
		}
	}

	else if (m_dwActionInput == dfACTION_PUSH)
	{
	}
	
	if(m_dwActionInput == dfACTION_STAND)
	{
		if (GetDirection() == dfDIR_LEFT)
		{
			SetDirection(dfDIR_LEFT);
			if (m_dwActionOld != dfACTION_STAND)
			{
				SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
			}
			m_dwActionCur = dfACTION_STAND;
			m_dwActionOld = m_dwActionCur;
		}
		else if (GetDirection() == dfDIR_RIGHT)
		{
			SetDirection(dfDIR_RIGHT);
			if (m_dwActionOld != dfACTION_STAND)
			{
				SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND); //dfDELAY_STAND=5
			}
			m_dwActionCur = dfACTION_STAND;
			m_dwActionOld = m_dwActionCur;
		}
	}
}

int CPlayerObject::GetDirection(void)
{
	return m_iDirCur;
}

char CPlayerObject::GetHP(void)
{
	return m_chHP;
}


BOOL CPlayerObject::IsPlayer(void)
{
	return m_bPlayerCharacter;
}


void CPlayerObject::SetActionAttack1(DWORD Attack1)
{

}

void CPlayerObject::SetActionAttack2(DWORD Attack2)
{

}

void CPlayerObject::SetActionAttack3(DWORD Attack3)
{

}

void CPlayerObject::SetActionMove(void)
{

}

void CPlayerObject::SetDirection(int iDirCur)
{
	//Left=0, Right=1.
	m_iDirCur = iDirCur;
}

void CPlayerObject::SetHP(void)
{

}
