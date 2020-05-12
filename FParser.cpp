#include "stdafx.h"
#include "FParser.h"
//------------------------------------------------------------------------------------------//
bool FParser::isNumber(const char* from, const char* to) {
	if(from[0]=='.' || from[to - from - 1]=='.') return false;
	if(!isdigit(*from)) {
		if(*from != '-' && *from != '+')
		return false;
	}
	int point_count = 0;
	for(char const *iter = from + 1; iter < to; iter++) {
		if(*iter != '.' && !isdigit(*iter)) return false;

		if(*iter=='.' && (point_count++ >= 1)) return false;
	}
	return true;
}
//------------------------------------------------------------------------------------------//
double FParser::getNumber(char *&str) {
	char buffer[256] = {0};
	int i = 0;
	while(str && (isdigit(*str) || *str=='.')) buffer[i++] = *str++;
	return atof(buffer);
}
//------------------------------------------------------------------------------------------//
int FParser::getCop(char token) {
	switch(token) {
		case '^' : return POW;
		case '*' : return MUL;
		case '/' : return DIV;
		case '+' : return ADD;
		case '-' : return SUB;
		default  : return ERROR;
	}
}
//------------------------------------------------------------------------------------------//
void FParser::reverseString(char* s) {
	if (!s)  return;
	char *begin = s;
	char *end = s + strlen(s) - 1;
	char tmp;
	while (end > begin) {
		tmp = *end;
		*end-- = *begin;
		*begin++ = tmp;
	}
}
//------------------------------------------------------------------------------------------//
void FParser::MakeExpression(Token* begin, Token* end, BracketExpArray& bracks) {
	Operation op;
	BracketExp tmp;
	BracketExpArray::iterator i;

	tmp.Nat = false;
	tmp.iOpBrack = begin->index;
	tmp.iCloseBrack = end->index;
	

	for(int priority = 0; priority < 3; priority++) {
		for(Token *iter = begin; iter < end; iter++) {
			if((priority == 0 && (iter->COP == POW)) ||
				(priority == 1 && (iter->COP == MUL  || iter->COP == DIV)) || 
				(priority == 2 && (iter->COP == ADD  || iter->COP == SUB))) {
				if(iter->Nat) continue;

				bool doBrake1 = false;
				bool doBrake2 = false;
				bool takeLeftOperand = true;
				bool takeRightOperand = true;
  

				if(!m_opers.empty()) {
					for(OperArray::iterator it = m_opers.end() - 1; it >= m_opers.begin(); it--) {
						if(doBrake1 && doBrake2) break;
						Token* iter1 = iter - 1;

						while(iter1 > begin && !doBrake1 && iter1->Nat) {
							if(!iter1->isCloseBrack && !iter1->isOpenBrack && iter1->COP == ERROR) {
								iter1--;
								continue;
							}
							if(iter1->isCloseBrack) {
								bool doInsert = false;

								for(i = bracks.end() - 1; i >= bracks.begin(); i--)
									if(i->iCloseBrack == iter1->index) {
										if(!i->Nat)
										doInsert = true;
										break;
									}
								if(doInsert) {
									op.leftIndex = i->index;
									i->Nat = true;
									takeLeftOperand = false;
									doBrake1 = true;
									break;
								}
								else {
									while(iter1-- > begin && (iter1->index != i->iOpBrack));
									iter1--;
									if(iter1 == begin) {
										doBrake1 = true;
										break;
									}
									continue;
								}
							}
							else if(iter1->index==it->Index) {
								 op.leftIndex = it->Index;
								 takeLeftOperand = false;
								 doBrake1 = true;
								 break;
							}
							iter1--;
						}

						iter1 = iter + 1;

						while(iter1 < end && !doBrake2 && iter1->Nat) {
							if(!iter1->isCloseBrack && !iter1->isOpenBrack && iter1->COP == ERROR) {
								iter1++;
								continue;
							}
							if(iter1->isOpenBrack) {
								bool doInsert = false;

								for(i = bracks.end() - 1; i >= bracks.begin(); i--)
									if(i->iOpBrack == iter1->index) {
										if(!i->Nat) doInsert = true; 
										break;
									}
								if(doInsert && i>=bracks.begin()) {
									op.rightIndex = i->index;
									i->Nat = true;
									takeRightOperand = false;
									doBrake2 = true;
									break;
								}
								else {
									while(iter1++ < end && (iter1->index != i->iCloseBrack));
									iter1--;
									if(iter1 == end) {
										doBrake2 = true;
										break;
									}
									continue;
								}
							}
							else if(iter1->index==it->Index) {
								op.rightIndex = it->Index;
								takeRightOperand = false;
								doBrake2 = true;
								break;
							}
							iter1++;
						}
					}
				}
  
				tmp.index = (op.Index = iter->index);
				op.COP = iter->COP;
				op.resultNeg = iter->resultNeg;
				iter->Nat = true;

				if(takeLeftOperand) {
					op.left = (iter - 1)->argument;
					op.leftIndex = -1;
				}

				if(takeRightOperand) {
					op.right = (iter + 1)->argument;
					op.rightIndex = -1;
				}

				op.isFunc = false;
				m_opers.push_back(op);
			}
		}    
	}
	if(begin->isFunc) {
		Operation* fnc = &m_opers.back();
		fnc->isFunc = true;
		fnc->fnc = begin->fnc;
	}
	bracks.push_back(tmp);
}
//------------------------------------------------------------------------------------------//
void FParser::SetFunction(CString& str) {
	m_opers.clear();
	TokenArray::iterator it;
	fillTokens((LPCSTR)str);
	int count = 0;
	for(it = m_toks.begin(); it < m_toks.end(); it++) {
		if(it->isCloseBrack) count++;
	}
	struct brackets {
		Token *op, *cl;
	};

	brackets* t = new brackets[count];

	int i = 0;
	for(it = m_toks.begin(); it < m_toks.end(); it++) {
		if(it->isOpenBrack) {
			t[i].op = &*it;
			for(TokenArray::iterator it1 = it + 1; it1 < m_toks.end(); it1++) {
				if(it1->isCloseBrack) {
					TokenArray::iterator it2 = it + 1;
				    int op = 0;
				    int cl = 0;

					while(it2 < it1) {
						if(it2->isOpenBrack) op++;
						if(it2->isCloseBrack) cl++;
						it2++;
					}
				   if(op == cl) {
					   t[i++].cl = &*it1;
					   break;
				   }
				}
			}
		}
	}
	BracketExpArray expAr;
	for(i = count - 1; i >=0 ; i--) MakeExpression(t[i].op, t[i].cl, expAr);
	m_toks.clear();
	Initialize(m_opers);

	delete[] t;
}
//------------------------------------------------------------------------------------------//
void __fastcall FParser::ExecuteExpression(Operation* iter, OperArray& ops, double &res) {
	for (OperArray::iterator i = ops.begin(); i < ops.end(); i++) {
		if (i->pRes) *(i->pRes) = i->getResult();
		if (i == ops.end() - 1) res = i->getResult();
	}
}
//------------------------------------------------------------------------------------------//
void FParser::fillTokens(const char* str) {
	/*To avoid adding NOP (no operation) in some operation which 
	contains only one token (argument or constant e.g.: "cos(x) ...")
	I push one ADD operation with 0 left operand (e.g. cos(0 + x) ... )*/
	double arg = 0.5;
	vector<char> string;
	string.push_back('0');
	short flag = 0;

	if(*str=='-') {
		string.push_back('-');
		flag = 1;
	}
	else string.push_back('+');

	size_t len = strlen(str);

	for(const char *j = str + flag; j < str + len;  ++j) {
		string.push_back(*j);
		if(*j=='(') {
			string.push_back('0');
			if(*(j+1)=='-') {
				string.push_back('-');
				j++;
			}
			else string.push_back('+');
		}
	}
	string.push_back('\0');
	int index = 0;
	Token tmp;
	tmp.isOpenBrack = true;
	tmp.isCloseBrack = false;
	tmp.isFunc = false;
	tmp.index = index++;
	tmp.argument.isVar = false;
	tmp.resultNeg = false;
	int negResOpIndex = -1;
	m_toks.push_back(tmp);

	for(char* iter = &string.front(); iter <= &string.back(); ++iter) {
		tmp.Nat = true;
		tmp.COP = ERROR;
		tmp.isFunc = false;
		tmp.argument.isVar = false;
		tmp.resultNeg = false;
		tmp.fnc.fName = NULL;

		//read up the open bracket or function 
		if(*iter=='(') {
			tmp.index = index++;
			tmp.isOpenBrack = true;
			if(iter - 1 >= &string.front() && isalpha(*(iter - 1))) {
				char* it = iter;
				vector<char> name;
				while((--it) >= &string.front() && isalpha(*it))
					name.push_back(*it);

				name.push_back('\0');
				reverseString(&name[0]);
				bool isUnknown = true;
				int count = sizeof(fncs)/sizeof(fncs[0]);
				for(int i = 0; i < count; i++)
					if(!strcmp(fncs[i].fName, &name[0])) {
						tmp.isFunc = true;
						tmp.fnc.fName = fncs[i].fName;
						tmp.fnc.fpfn = fncs[i].fpfn;
						isUnknown = false;
						break;
					}
					if(isUnknown) {
						CString errStr = "Unknown function: ";
						errStr += &name[0];
						MessageBox(NULL, errStr, "message", MB_ICONERROR | MB_OK);
						exit(1);
					} 
			}
			m_toks.push_back(tmp);
			continue;
		}
		//read close bracket
		if(*iter==')')  {
			tmp.index = index++;
			tmp.isCloseBrack = true;
			m_toks.push_back(tmp);
			continue;
		}
		tmp.isCloseBrack = false;
		tmp.isOpenBrack = false;
		//read operand or constant
		if(isdigit(*iter) || *iter == 'x' || *iter == 'e' || 
			(iter + 1 <= &string.back() && *iter == 'p' && *(iter + 1) == 'i')) {
			tmp.index = index++;
			bool isNegative = false;
			if(((iter - 2 && *(iter - 2)=='(') || !*(iter - 2)) && *(iter - 1)=='-') {
				char *i = iter;
				isNegative = true;
				while(++i && *i!=')' && 
					*i!='+' && *i!='-' &&
					*i!='*' && *i != '/' && (isNegative = !(*i=='^')));

				if(!isNegative) negResOpIndex = index;
			}
			if(*iter!='x') {
				if(*iter == 'e') tmp.argument.operand = E;
				else if(iter + 1 <= &string.back() && *iter == 'p' && *(iter + 1) == 'i')
						tmp.argument.operand = PI;
					 else {
						 tmp.argument.operand = getNumber(iter);
						 iter--;
					}
			}
			else {
				tmp.argument.operand = arg;
				tmp.argument.isVar = true;
			}
			if(isNegative) tmp.argument.operand = -tmp.argument.operand;
			m_toks.push_back(tmp);
			continue;
		}
		//read operation
		if((iter > &string.front() && *(iter - 1)!='(') && ((tmp.COP = getCop(*iter)) != ERROR)) {
			if(negResOpIndex!=-1) {
				tmp.resultNeg = true;
				negResOpIndex = -1;
			}
			tmp.index = index++;
			tmp.Nat = false;
			m_toks.push_back(tmp);
		}
	}
	tmp.isOpenBrack = false;
	tmp.isCloseBrack = true;
	tmp.isFunc = false;
	tmp.index = index;
	m_toks.push_back(tmp);
}
//------------------------------------------------------------------------------------------//
void FParser::Execute(double arg, double &res, OperArray *op) {
	OperArray* buf = (!op ? &m_opers : op);
	if (buf->empty()) return;
	for(Operation* iter = &buf->front(); iter <= &buf->back(); ++iter) {
		if(iter->left.isVar)
			iter->left.operand = arg;
		if(iter->right.isVar)
			iter->right.operand = arg;
	}
	ExecuteExpression(&buf->back(), *buf, res);
}
//------------------------------------------------------------------------------------------//
double Operation::getResult() {
	double res;
	switch(COP) {
	case POW: res = power(left.operand, right.operand); break;
	case MUL : res = mul(left.operand, right.operand); break;
	case DIV : {
			       if(right.operand == 0) return left.operand < 0 ? -DBL_MAX : DBL_MAX;
				   res = div(left.operand, right.operand);
				   break;
			   }
	case ADD : res = add(left.operand, right.operand); break;
	case SUB : res = sub(left.operand, right.operand); break;
	//must never happen
	default: { ::MessageBox(0, TEXT("Warning!"), TEXT("Unsopported operation."), MB_ICONINFORMATION | MB_OK); return -1.0; }; 
	}
	return isFunc ? fnc.fpfn(res) : res;
}
//------------------------------------------------------------------------------------------//
double add(double a, double b) {
	return a + b;
}
//------------------------------------------------------------------------------------------//
double sub(double a, double b) {
	return a - b;
}
//------------------------------------------------------------------------------------------//
double mul(double a, double b) {
	return a*b;
}
//------------------------------------------------------------------------------------------//
double div(double a, double b) {
	double res;
	try {
		res = a / b;
	}
	catch (...) {
		::MessageBox(0, "Divide by 0 error", "Cuation", MB_OK|MB_ICONERROR);
		::ExitProcess(::GetLastError());
	}
	return res;
}
//------------------------------------------------------------------------------------------//
double power(double a, double b) {
	return pow(a, b);
}
//------------------------------------------------------------------------------------------//
void FParser::Initialize(OperArray& opers) {
	for(OperArray::iterator iter1 = opers.begin(); iter1 < opers.end(); iter1++) {
		for(OperArray::iterator iter2 = iter1 + 1; iter2 < opers.end(); iter2++) {
			if(iter2->leftIndex==iter1->Index) {
				iter1->pRes = &(iter2->left.operand);
				break;
			}

			if(iter2->rightIndex==iter1->Index) {
				iter1->pRes = &(iter2->right.operand);
				break;
			}
		} 
	}
	(opers.end()-1)->pRes = NULL;
}
//------------------------------------------------------------------------------------------//
CArchive& operator <<(CArchive& ar, funcParams params) {
	ar << params.fName;
	ar << params.to;
	ar << params.from;
	ar << params.delta;
  
	return ar;
}
//------------------------------------------------------------------------------------------//
CArchive& operator >>(CArchive& ar, funcParams& params) {
	ar >> params.fName;
	ar >> params.to;
	ar >> params.from;
	ar >> params.delta;

	return ar;
}
//------------------------------------------------------------------------------------------//
CArchive& operator <<(CArchive& ar, Function fnc) {
	ar << fnc.opCount;
	ar << fnc.params;
	ar << fnc.absMax;
	ar << fnc.max;
	ar << fnc.min;

	for(Operation* iter = &fnc.ops.front(); iter <= &fnc.ops.back(); iter++)
		ar << *iter;

	return ar;
}
//------------------------------------------------------------------------------------------//
CArchive& operator >>(CArchive& ar, Function& fnc) {
	ar >> fnc.opCount;
	ar >> fnc.params;
	ar >> fnc.absMax;
	ar >> fnc.max;
	ar >> fnc.min;

	for(int i = 0; i < fnc.opCount; i++) {
		Operation op;
		ar >> op;
		fnc.ops.push_back(op);
	}

	return ar;
}
//------------------------------------------------------------------------------------------//
CArchive& operator <<(CArchive& ar, Operation op) {
	ar << op.COP;
	ar << op.Index;

	ar << op.isFunc;
	if(op.isFunc) {
		size_t len = strlen(op.fnc.fName) + 1;
		ar << len;
		ar.Write(op.fnc.fName, len);
	}
	ar << op.left.isVar;
	ar << op.left.operand;
	ar << op.leftIndex;
	ar << op.resultNeg;
	ar << op.right.isVar;
	ar << op.right.operand;
	ar << op.rightIndex;

	return ar;
}
//------------------------------------------------------------------------------------------//
CArchive& operator >>(CArchive& ar, Operation& op) {
	ar >> op.COP;
	ar >> op.Index;
	ar >> op.isFunc;

	if(op.isFunc) {
		int len;
		ar >> len;
		char* buffer = new char[len];
		ar.Read(buffer, len);
		for(int i = 0; i < 9; i++)
			if(!strcmp(buffer, fncs[i].fName)) {
			   op.fnc.fName = fncs[i].fName;
			   op.fnc.fpfn = fncs[i].fpfn;
			   break;
			}
		delete[] buffer;
	}
	ar >> op.left.isVar;
	ar >> op.left.operand;
	ar >> op.leftIndex;
	ar >> op.resultNeg;
	ar >> op.right.isVar;
	ar >> op.right.operand;
	ar >> op.rightIndex;
 
	return ar;
}
//------------------------------------------------------------------------------------------//