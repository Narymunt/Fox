// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXVECTOR_
#define _FOXVECTOR_

#include <windows.h>
#include <math.h>

class __declspec(dllexport) CVector  
{

public:
	CVector();
	virtual ~CVector();

	// operacje na wektorach

	operator++(void);	// zwiekszenie o 1
	operator--(void);	// zmniejszenie o 1

	CVector operator+(CVector const &);	// dodawanie wektorów
	CVector operator-(CVector const &); // odejmowanie wektorów
	
	CVector operator*(double);			// skala * wektor

	double operator*(CVector const &);	// iloczyn skalarny
	

	void operator=(CVector const &);	// przypisanie

//	double GetLength();	// pobierz dlugosc
	double Normalize();	// normalizuj wektor

	// ustaw lub zwroc wspolrzedne

	double GetXPosition(void);
	void SetXPosition(double dValue);

	double GetYPosition(void);
	void SetYPosition(double dValue);

	double GetZPosition(void);
	void SetZPosition(double dValue);

private:

	// wspolrzedne w przestrzeni

	double	m_dXPosition;	
	double	m_dYPosition;	
	double	m_dZPosition;



};

#endif // !defined(AFX_VECTOR_H__FBEE467A_5C5C_486A_A1A9_1931556651F2__INCLUDED_)
