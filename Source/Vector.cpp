// Vector.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////

#include "Vector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// wspolrzedne zerowane

CVector::CVector()
{
	m_dXPosition = 0.0;
	m_dYPosition = 0.0;
	m_dZPosition = 0.0;

}

CVector::~CVector()
{

}

//=== iloczyn skalarny

double CVector::operator*(CVector const &VTemp)
{
	return (m_dXPosition*VTemp.m_dXPosition+
		    m_dYPosition*VTemp.m_dYPosition+
			m_dZPosition*VTemp.m_dZPosition);
}

//=== przypisanie wektorow

void CVector::operator =(CVector const &VTemp)
{
	m_dXPosition = VTemp.m_dXPosition;
	m_dYPosition = VTemp.m_dYPosition;
	m_dZPosition = VTemp.m_dZPosition;
}

//=== dodawanie wektorów

CVector CVector::operator +(CVector const &VTemp)
{
	CVector VReturn;
	
	VReturn.m_dXPosition = m_dXPosition + VTemp.m_dXPosition;
	VReturn.m_dYPosition = m_dYPosition + VTemp.m_dYPosition;
	VReturn.m_dZPosition = m_dZPosition + VTemp.m_dZPosition;

	return VReturn;
}

//=== odejmowanie wektorów

CVector CVector::operator -(CVector const &VTemp)
{
	CVector VReturn;

	VReturn.m_dXPosition = m_dXPosition - VTemp.m_dXPosition;
	VReturn.m_dYPosition = m_dYPosition - VTemp.m_dYPosition;
	VReturn.m_dZPosition = m_dZPosition - VTemp.m_dZPosition;

	return VReturn;
}
	
//=== skala * wektor

CVector CVector::operator *(double dValue)
{
	CVector VReturn;

	VReturn.m_dXPosition = m_dXPosition * dValue;
	VReturn.m_dYPosition = m_dYPosition * dValue;
	VReturn.m_dZPosition = m_dZPosition * dValue;

	return VReturn;
}

//=== zwiekszenie o jeden 

CVector::operator ++(void)
{
	m_dXPosition = m_dXPosition + 1.0f;
	m_dYPosition = m_dYPosition + 1.0f;
	m_dZPosition = m_dZPosition + 1.0f;
}

//=== zmniejszenie o jeden

CVector::operator --(void)
{
	m_dXPosition = m_dXPosition - 1.0f;
	m_dYPosition = m_dYPosition - 1.0f;
	m_dZPosition = m_dZPosition - 1.0f;
}

//=== normalizuj wektor

double CVector::Normalize(void)
{
	return sqrt( (m_dXPosition*m_dXPosition) + 
			     (m_dYPosition*m_dYPosition) +
				 (m_dZPosition*m_dZPosition)
				 );

}

//=== pobierz wspolrzedna x dla wektora

double CVector::GetXPosition(void)
{
	return m_dXPosition;
}

//=== ustaw wsporzedna x dla wektora

void CVector::SetXPosition(double dValue)
{
	m_dXPosition = dValue;
}

//=== pobierz wspolrzedna y dla wektora

double CVector::GetYPosition(void)
{
	return m_dYPosition;
}

//=== ustaw wsporzedna y dla wektora

void CVector::SetYPosition(double dValue)
{
	m_dYPosition = dValue;
}

//=== pobierz wspolrzedna z dla wektora

double CVector::GetZPosition(void)
{
	return m_dZPosition;
}

//=== ustaw wsporzedna x dla wektora

void CVector::SetZPosition(double dValue)
{
	m_dZPosition = dValue;
}

