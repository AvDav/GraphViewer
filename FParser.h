/* 
Execution after the parsing.
cos(x + sin(x + 0.1)*ln(x))
Parser holds the expression like a binary tree structure:																								
												----------
												|+, cos()|
												---------- 
												/	   \
											   /		\
											  /			 \
											 /			  \	 
											/			   \
										   ---			   ---
										   |x|			   |*|		
										   ---			   ---
														   / \	
														  /	  \	
														 /	   \
													    /	 	\
													   /	 	 \	
												  ----------   ---------	
												  |+, sin()|   |+, ln()|
												  ----------   ---------
												  /		\		     /  \
											     /       \          /    \
											    /         \		   / 	  \	
											   /           \      /		   \
											  /				\    /		    \
											---			  -----	---			---
											|x|			  |0.1| |x|			|0|
											---			  -----	---			---
The execution of operations begins at the bottom.
*/														

#ifndef FPARSER_H
#define FPARSER_H

#include <cmath>
#include <vector>
using namespace std;

#ifdef  ERROR
#undef  ERROR
#endif

#define E			2.71828182845904523536
#define PI			3.14159265358979323846
#define DBL_MAX     1.7976931348623158e+308

#define ERROR -1
#define POW    0
#define MUL    1
#define DIV    2
#define ADD    3
#define SUB    4

#define TokenArray vector<Token> 
#define BracketExpArray vector<BracketExp>
#define OperArray vector<Operation>
#define FuncArray vector<Function>

//declarations of supported operations
inline double power(double, double);
inline double mul(double, double);
inline double div(double, double);
inline double add(double, double);
inline double sub(double, double);


//------------------The following structures are needed for parsing------------//
struct funcParams {
	friend CArchive& operator <<(CArchive &ar, funcParams);
	friend CArchive& operator >>(CArchive &ar, funcParams&);
	CString fName;
	double  from;
	double  to;
	double  delta;
};
/*****************************************************************************/
struct Operand {
	double operand;
	short  isVar;
};
/*****************************************************************************/
struct func {
	char   *fName;
	double(*fpfn)(double);
};
/*****************************************************************************/
struct Operation {
	friend CArchive& operator <<(CArchive &ar, Operation);
	friend CArchive& operator >>(CArchive &ar, Operation&);

	int     Index;
	int     leftIndex;
	int     rightIndex;
	short   resultNeg;
	short   COP;
	double *pRes;
	Operand left;
	Operand right;
	double  getResult();
	short   isFunc;
	func    fnc;
};
/*****************************************************************************/
struct Function {
	friend  CArchive& operator <<(CArchive &, Function);
	friend  CArchive& operator >>(CArchive &, Function&);
	funcParams  params;
	int         opCount;
	OperArray   ops;
	double      absMax;
	double      max;
	double      min;
};
/*****************************************************************************/
struct BracketExp {
	int  iOpBrack;
	int  iCloseBrack;
	int  index;
	bool Nat;
};
/*****************************************************************************/
struct Token {
	Operand argument;
	short  COP;          // if it is not an operation COP==-1
	int    index;
	bool   isFunc;
	bool   Nat;
	bool   isOpenBrack;
	bool   isCloseBrack;
	bool   resultNeg;
	func   fnc;
};
/*****************************************************************************/
class FParser {
public:
	void	__fastcall Execute(double arg, double &res, OperArray * = nullptr);
	void	SetFunction(CString &);
	void	GetOpers(OperArray &ops) const { ops = m_opers; Initialize(ops); }
	static bool isNumber(const char *, const char*);
	static void Initialize(OperArray&);
protected:
	void   reverseString(char *);
	void   fillTokens(const char *);
	void   MakeExpression(Token *, Token *, BracketExpArray &);
	double getNumber(char *&);
	int    getCop(char);
	void __fastcall ExecuteExpression(Operation *, OperArray &, double &);
	OperArray  m_opers;
	TokenArray m_toks;
};
/*****************************************************************************/
//--------------------Array of available math. functions---------------------//
/*****************************************************************************/
static func fncs[] = {
	{"cos", cos}, {"sin", sin},
	{"tan", tan}, {"ln",  log},
	{"sinh", sinh}, {"cosh", cosh},
	{"tanh", tanh}
};
/*****************************************************************************/
#endif 
