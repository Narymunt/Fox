// Procedure.h: interface for the CProcedure class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXPROCEDURE_
#define _FOXPROCEDURE_

#include <windows.h>
#include <fox.h>

// ta klasa jest wykorzystywana do reprezentacji funkcji zapisanych 
// w edytorze

#define		V_PROCEDURE_NONE		4
#define		V_PROCEDURE_STRING		3
#define		V_PROCEDURE_CONST		2
#define		V_PROCEDURE_LOCAL		1
#define		V_PROCEDURE_DEFAULT		0

// operatory

typedef enum {	
		OP_EQUAL,				/* =  */
		OP_SUBTRACT_EQUAL,		/* -= */
		OP_ADD_EQUAL,			/* += */
		OP_MULTIPLY_EQUAL,		/* *= */
		OP_DIVIDE_EQUAL,		/* /= */
		OP_MODULO_EQUAL,		/* %= */
		OP_UPPER_EQUAL,			/* &= */
		OP_LOWER_EQUAL,			/* $= */
		OP_POWER,				/* ^  */
		OP_ADD_ADD,				/* ++ */
		OP_SUBTRACT_SUBTRACT,	/* -- */
		OP_ADD,					/* +  */
		OP_SUBTRACT,			/* -  */
		OP_MULTIPLY,			/* *  */
		OP_DIVIDE,				/* /  */
		OP_RANDOM,				/* losowanie */
		OP_EQUIVALENT,			/* == */
		OP_SMALL,				/* <  */
		OP_SMALL_EQUAL,			/* <= */
		OP_LARGER,				/* >  */
		OP_LARGER_EQUAL,		/* >= */
		OP_NONE
		} _operator;


class __declspec(dllexport) CProcedure  
{

public:
	CProcedure();
	virtual ~CProcedure();

private:

	int		m_iOpsCount;	// liczba operatorow i operandow
	
	char	m_cName[256];	// nazwa operandu [V_PROCEDURE_STRING]

	double	m_dValue;		// wartosc [V_PROCEDURE_CONST]

	int		m_iLocalValue;	// zmienna lokalna [V_PROCEDURE_LOCAL]

	int		m_iAddValue;	// zmienna w epizodzie >=0 [?]

	int		m_iOperator;	// operator	

};

#endif 
