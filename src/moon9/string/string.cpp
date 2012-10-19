#include <iostream>

#include "string.hpp"

#ifdef _WIN32  // start of Peter Kankowski's expression evaluator

	// (c) Peter Kankowski, 2007. http://smallcode.weblogs.us mailto:kankowski@narod.ru

	// The character type used in the evaluator
	typedef char EVAL_CHAR;

	// Error codes
	enum EXPR_EVAL_ERR {
		EEE_NO_ERROR = 0,
		EEE_PARENTHESIS = 1,
		EEE_WRONG_CHAR = 2,
		EEE_DIVIDE_BY_ZERO = 3,
		EEE_UNDEF_VAR = 4,
		EEE_TOO_MUCH_VARS = 5
	};

	struct EVAL_CODE
	{
		EXPR_EVAL_ERR error;
		const EVAL_CHAR *error_position;
		double result;
	};

	#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
	#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
	#endif
	#include <assert.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <ctype.h>
	#include <windows.h>

	#ifdef _DEBUG
	//#define DUMP_RPN
	#endif

	// ================================
	//   Code
	// ================================
	enum OPERATION_CODE {
		OP_ADD = 0xC1DE, // FADDP instruction
		OP_SUB = 0xE9DE,
		OP_MUL = 0xC9DE,
		OP_DIV = 0xF9DE,
		OP_CHANGE_SIGN = 0xE0D9
	};

	class ExprCode {
	private:
		short*  _code;
		short*  _cur_code;
		short*  _code_end;

		double* _data;
		double* _cur_data;
		double* _data_end;

		void PrintErrorAndExit() {
			puts("Out of memory");
			exit(1);
		}
		const char* GetOpName(OPERATION_CODE opcode) {
			switch(opcode) {
				case OP_ADD: return "FADDP";
				case OP_SUB: return "FSUBP";
				case OP_MUL: return "FMULP";
				case OP_DIV: return "FDIVP";
				case OP_CHANGE_SIGN: return "FCHS";
				default: return "Invalid opcode";
			}
		}
		static const short OP_FLD  = 0x05DDu;
		static const short OP_FSTP = 0x1DDDu;
		static const short OP_FST = 0x15DDu;
		static const short OP_RET = 0x90C3u; // Emit RET and NOP
	public:
		ExprCode() {
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			DWORD region_size = si.dwAllocationGranularity;
			//region_size = UINT_MAX; // Test the error-handling code
			// Allocate two regions: one for code and one for data
			_code = (short*)VirtualAlloc(NULL, region_size * 2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if(!_code)
				PrintErrorAndExit();
			_code_end = (short*) ((char*)_code + region_size);
			_data = (double*)_code_end;
			_data_end = (double*)((char*)_data + region_size);
			// Initialize current pointers
			_cur_code = _code;
			_cur_data = _data;
		}

		~ExprCode() {
			VirtualFree(_code, 0, MEM_RELEASE);
		}

		void ClearCode() {
			// Reset pointer to the beginning of the region
			_cur_code = _code;
		}

		void EmitOperation(OPERATION_CODE opcode) {
			if(_cur_code >= _code_end)
				PrintErrorAndExit();
			*_cur_code = (short)opcode;
			_cur_code++;
	#ifdef DUMP_RPN
			printf("%s\n", GetOpName(opcode));
	#endif
		}

	private:
		void EmitFldFstp(short opcode, double* ptr) {
			if((char*)_cur_code + sizeof(int) >= (char*)_code_end)
				PrintErrorAndExit();
			*_cur_code++ = opcode;
	#if defined(_WIN64)
			// Under x86-64, the address is relative to the address of the next instruction
			*(int*)_cur_code = (int)(INT_PTR)((char*)ptr - ((char*)_cur_code + sizeof(int)));
	#elif defined(_WIN32)
			// Under x86, flat 32-bit address is used
			*(int*)_cur_code = (int)(INT_PTR)ptr;
	#else
	#error Unsupported platform
	#endif
			_cur_code = (short*)((char*)_cur_code + sizeof(int));
		}

	public:
		void EmitLoadVariable(double* var) {
			EmitFldFstp(OP_FLD, var);
	#ifdef DUMP_RPN
			printf("FLD  [%p]\n", var);
	#endif
		}

		void EmitLoadNumber(double number) {
			if(_cur_data >= _data_end)
				PrintErrorAndExit();
			// Write the number to data section
			*_cur_data = number;
			// Emit FLD [_cur_data] instruction and advance the pointer
			EmitFldFstp(OP_FLD, _cur_data);
			_cur_data++;
	#ifdef DUMP_RPN
			printf("FLD %g\n", number);
	#endif
		}

		enum POP_STORED {P_POP, P_LEAVE};

		double* EmitStoreNumber(POP_STORED pop) {
			if(_cur_data >= _data_end)
				PrintErrorAndExit();
			// Emit FSTP [_cur_data]
			double* num = _cur_data;
			EmitFldFstp(pop == P_POP ? OP_FSTP : OP_FST, num);
	#ifdef DUMP_RPN
			printf(pop == P_POP ? "FSTP [%p]\n" : "FST  [%p]\n", num);
	#endif
			_cur_data++;
			return num;
		}

		void EmitEnd() {
			if(_cur_code >= _code_end)
				PrintErrorAndExit();
			*_cur_code = OP_RET;
			_cur_code++;
	#ifdef DUMP_RPN
			printf("RET\n\n");
	#endif
		}

		void Execute() {
			// Cast _code to the pointer to a function returning void
			void (*p)() = (void (*)())_code;
			p();
		}
	};

	#ifdef _DEBUG
	void TestExprCode() {
		ExprCode code;
		// -3.5 + 1.2 = -2.3
		code.EmitLoadNumber(-3.5);
		code.EmitLoadNumber(1.2);
		code.EmitOperation(OP_ADD);
		double *res1 = code.EmitStoreNumber(ExprCode::P_POP);

		// (1.5 - 0.5 + 0.2) / 5 = 0.24
		code.EmitLoadNumber(1.5);
		code.EmitLoadNumber(0.5);
		code.EmitOperation(OP_SUB);
		code.EmitLoadNumber(0.2);
		code.EmitOperation(OP_ADD);
		code.EmitLoadNumber(5);
		code.EmitOperation(OP_DIV);
		double *res2 = code.EmitStoreNumber(ExprCode::P_POP);

		// (1 - 10^6 / 10^5) * 2.5 = -22.5
		code.EmitLoadNumber(1);
		code.EmitLoadNumber(10e6);
		code.EmitLoadNumber(10e5);
		code.EmitOperation(OP_DIV);
		code.EmitOperation(OP_SUB);
		code.EmitLoadNumber(2.5);
		code.EmitOperation(OP_MUL);
		double *res3 = code.EmitStoreNumber(ExprCode::P_POP);

		// Load and store one number
		code.EmitLoadNumber(-1.23e5);
		double *res4 = code.EmitStoreNumber(ExprCode::P_POP);

		// Load a variable (0.24 + 0.26 = 0.5)
		code.EmitLoadVariable(res2);
		code.EmitLoadNumber(0.26);
		code.EmitOperation(OP_ADD);
		double *res5 = code.EmitStoreNumber(ExprCode::P_POP);

		code.EmitEnd();
		code.Execute();
		assert(*res1 == -2.3);
		assert(*res2 == 0.24);
		assert(*res3 == -22.5);
		assert(*res4 == -1.23e5);
		assert(*res5 == 0.5);
	}
	#endif

	bool InfinifyOrNan(double num) {
		unsigned second_byte = *((unsigned*)&num + 1);
		const unsigned EXPONENT_MASK = 0x7FF00000;
		return (second_byte & EXPONENT_MASK) == EXPONENT_MASK;
	}


	// ================================
	// Fixed-size hash table
	// ================================

	// TRUE, if the first string is a prefix of the second
	BOOL prefixcmp(const EVAL_CHAR* prefix, const EVAL_CHAR* str) {
		while(*prefix != '\0') {
			if(*prefix++ != *str++)
				return FALSE;
		}
		return TRUE;
	}

	template <class VAL, UINT TSIZE, VAL default_value> class StrHashTable {
	private:
		UINT HashFunction(const EVAL_CHAR* key, const EVAL_CHAR* end) {
			UINT h = 0;
			for(; key < end; key++)
				h = (31 * h + (*key - 'a'));
			h ^= (h >> 16);
			return h % TSIZE;
		}
		VAL  _vals[TSIZE];
		EVAL_CHAR* _keys[TSIZE];
		UINT _count;
	public:
		StrHashTable() {
			memset(_keys, 0, sizeof(_keys));
			_count = 0;
		}
		~StrHashTable() {
			for(UINT i = 0; i < TSIZE; i++)
				free(_keys[i]);
		}
		VAL* Lookup(const EVAL_CHAR* key, const EVAL_CHAR* end) {
			UINT hash = HashFunction(key, end);
			// Look up the value in the table
			while(_keys[hash] != 0) {
				if( prefixcmp(_keys[hash], key) )
					return _vals + hash;
				hash = (hash + 1) % TSIZE;
			}
			if(_count >= TSIZE - 1)
				return NULL;
			// If it's not here, add it
			_vals[hash] = default_value;
			SIZE_T len = end - key;
			_keys[hash] = (EVAL_CHAR*)malloc( (len + 1) * sizeof(EVAL_CHAR) );
			memcpy( _keys[hash], key, len * sizeof(EVAL_CHAR) );
			_keys[hash][len] = '\0';
			_count++;
			return _vals + hash;
		}
	};



	// ================================
	//   Simple expression evaluator
	// ================================

	class ExprEval {
	private:
		EXPR_EVAL_ERR _err;
		EVAL_CHAR _cur_token;
		double _number;
		double** _var;
		ExprCode _code;
		EVAL_CHAR* _cur_char;
		EVAL_CHAR* _err_pos;
		EVAL_CHAR* _op_start;
		StrHashTable<double*, 1024, 0> _variables;
		static const EVAL_CHAR TOKEN_NUM = '0';
		static const EVAL_CHAR TOKEN_VAR = 'a';

		// === Error handling ===
	public:
		EXPR_EVAL_ERR GetErr() {
			return _err;
		}
		EVAL_CHAR* GetErrPos() {
			return _err_pos;
		}
	private:
		int inline ReportError(EXPR_EVAL_ERR err) {
			_err = err;
			_err_pos = _op_start;
			return 0;
		}

		// ==== Lexer ====
		static BOOL IsVarFirstChar(EVAL_CHAR ch) {
			return isalpha(ch) || ch == '_';
		}
		static BOOL IsVarNextChar(EVAL_CHAR ch) {
			return isalnum(ch) || ch == '_';
		}
		EVAL_CHAR NextToken() {
			// Skip spaces
			while(*_cur_char == ' ' || *_cur_char == '\t')
				_cur_char++;

			_op_start = _cur_char;
			// Operator
			if( strchr("+-*/=()", *_cur_char) )
				return _cur_token = *_cur_char++;

			// Number
			if( isdigit(*_cur_char) || *_cur_char == '.' ) {
				char* end;
				_number = strtod(_cur_char, &end);
				if(end == _cur_char) {
					_cur_char++; // "0+." and similar expressions
					return _cur_token = (EVAL_CHAR)ReportError(EEE_WRONG_CHAR);
				}
				_cur_char = end;
				return _cur_token = TOKEN_NUM;
			}

			// Variable name
			if(IsVarFirstChar(*_cur_char)) {
				EVAL_CHAR* var_name = _cur_char++;
				while( IsVarNextChar(*_cur_char) )
					_cur_char++;
				_var = _variables.Lookup( var_name, _cur_char );
				if(!_var)
					return _cur_token = (EVAL_CHAR)ReportError(EEE_TOO_MUCH_VARS);
				return _cur_token = TOKEN_VAR;
			}
			return _cur_token = (EVAL_CHAR)ReportError(EEE_WRONG_CHAR);
		}

		// ==== Parser ====

		// Parse a number or an expression in parenthesis
		void ParseAtom() {
			bool negative = false; // Unary minus or plus
			if(_cur_token == '-' || _cur_token == '+') {
				negative = _cur_token == '-';
				NextToken();
			}

			if(_cur_token == TOKEN_NUM) {
				_code.EmitLoadNumber(negative ? -_number : _number);
				NextToken();
			} else if(_cur_token == '(') {
				NextToken();
				ParseSummands();
				if(_cur_token != ')') // Unmatched opening parenthesis
					ReportError(EEE_PARENTHESIS);
				NextToken();
				if(negative)
					_code.EmitOperation(OP_CHANGE_SIGN);
			} else if(_cur_token == TOKEN_VAR) {
				EVAL_CHAR* var_name = _op_start;
				if( NextToken() == '=') {
					double** stored_to = _var;
					NextToken();
					ParseSummands();
					*stored_to = _code.EmitStoreNumber(ExprCode::P_LEAVE);
				} else {
					if(NULL == *_var) {
						_op_start = var_name;
						ReportError(EEE_UNDEF_VAR);
					}
					_code.EmitLoadVariable(*_var);
				}
				if(negative)
					_code.EmitOperation(OP_CHANGE_SIGN);
			} else {
				if(_err == EEE_NO_ERROR)
					ReportError(EEE_WRONG_CHAR);
			}
		}

		// Parse multiplication and division
		void ParseFactors() {
			ParseAtom();
			EVAL_CHAR op;
			while( (op = _cur_token) == '/' || op == '*' ) {
				NextToken();
				ParseAtom();
				// Emit the saved operation
				_code.EmitOperation(op == '*' ? OP_MUL : OP_DIV);
			}
		}

		// Parse addition and subtraction
		void ParseSummands() {
			ParseFactors();
			EVAL_CHAR op;
			while( (op = _cur_token) == '+' || op == '-' ) {
				NextToken();
				ParseFactors();
				_code.EmitOperation(op == '+' ? OP_ADD : OP_SUB);
			}
		}

	public:
		double Eval(EVAL_CHAR* expr) {
			_err = EEE_NO_ERROR;
			_cur_char = expr;
			_code.ClearCode();
			NextToken();
			ParseSummands();
			if(_err != EEE_NO_ERROR)
				return 0;
			// Now, expr should point to '\0'
			if(_cur_token == ')')
				return ReportError(EEE_PARENTHESIS);
			if(_cur_token != '\0')
				return ReportError(EEE_WRONG_CHAR);
			// Finish the generated code
			double* res = _code.EmitStoreNumber(ExprCode::P_POP);
			_code.EmitEnd();
			// and execute it
			_code.Execute();
			if(InfinifyOrNan(*res))
				return ReportError(EEE_DIVIDE_BY_ZERO);
			return *res;
		}
	};

	// =======
	//  Tests
	// =======

	#ifdef _DEBUG
	void TestExprEval() {
		ExprEval eval;
		// Some simple expressions
		assert(eval.Eval("1234") == 1234 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("1+2*3") == 7 && eval.GetErr() == EEE_NO_ERROR);

		// Parenthesis
		assert(eval.Eval("5*(4+4+1)") == 45 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("5*(2*(1+3)+1)") == 45 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("5*((1+3)*2+1)") == 45 && eval.GetErr() == EEE_NO_ERROR);

		// Spaces
		assert(eval.Eval("5 * ((1 + 3) * 2 + 1)") == 45 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("5 - 2 * ( 3 )") == -1 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("5 - 2 * ( ( 4 )  - 1 )") == -1 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval(" 2+2 ") == 4 && eval.GetErr() == EEE_NO_ERROR);

		// Sign before parenthesis
		assert(eval.Eval("-(2+1)*4") == -12 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("-4*(2+1)") == -12 && eval.GetErr() == EEE_NO_ERROR);

		// Fractional numbers
		assert(eval.Eval("1.5/5") == 0.3 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("1/5e10") == 2e-11 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("(4-3)/(4*4)") == 0.0625 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("1/2/2") == 0.25 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("0.25 * .5 * 0.5") == 0.0625 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval(".25 / 2 * .5") == 0.0625 && eval.GetErr() == EEE_NO_ERROR);

		// Repeated operators
		assert(eval.Eval("1+-2") == -1 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("2--2") == 4 && eval.GetErr() == EEE_NO_ERROR);

		// Variables
		assert(eval.Eval("a = 2") == 2 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("a = a + 1") == 3 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("res = b*b + a*a") == 0 && eval.GetErr() == EEE_UNDEF_VAR);
		assert(eval.Eval("1 + b = 2 - 8 * 0.5") == -1 && eval.GetErr() == EEE_NO_ERROR);
		//assert(eval.Eval("res") == 0 && eval.GetErr() == EEE_UNDEF_VAR);
		assert(eval.Eval("dest") == 0 && eval.GetErr() == EEE_UNDEF_VAR);
		assert(eval.Eval("res = b*b + a*a") == 9 + 4 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("a") == 3 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("2*b") == -4 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("2/b") == -1 && eval.GetErr() == EEE_NO_ERROR);
		assert(eval.Eval("res") == 9 + 4 && eval.GetErr() == EEE_NO_ERROR);

		// === Errors ===
		// Parenthesis error
		eval.Eval("5*((1+3)*2+1");
		assert(eval.GetErr() == EEE_PARENTHESIS && strcmp(eval.GetErrPos(), "") == 0);
		eval.Eval("5*((1+3)*2)+1)");
		assert(eval.GetErr() == EEE_PARENTHESIS && strcmp(eval.GetErrPos(), ")") == 0);

		// Repeated operators (wrong)
		eval.Eval("5*/2");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "/2") == 0);

		// Wrong position of an operator
		eval.Eval("*2");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "*2") == 0);
		eval.Eval("2+");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "") == 0);
		eval.Eval("2*");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "") == 0);
		eval.Eval("2/");
		assert(eval.GetErr() == EEE_WRONG_CHAR);

		// Division by zero
		eval.Eval("2/0");
		assert(eval.GetErr() == EEE_DIVIDE_BY_ZERO);
		eval.Eval("3+1/(5-5)+4");
		assert(eval.GetErr() == EEE_DIVIDE_BY_ZERO);

		// Invalid characters
		eval.Eval("~5");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "~5") == 0);
		eval.Eval("5x");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "x") == 0);
		eval.Eval(".+0");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), ".+0") == 0);

		// Multiply errors
		eval.Eval("3+1/0+4$"); // Only one error will be detected (in this case, the last one)
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "$") == 0);
		eval.Eval("3+1/0+4");
		assert(eval.GetErr() == EEE_DIVIDE_BY_ZERO);
		eval.Eval("~+1/0)"); // ...or the first one
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "~+1/0)") == 0);
		eval.Eval("+1/0)");
		assert(eval.GetErr() == EEE_PARENTHESIS && strcmp(eval.GetErrPos(), ")") == 0);
		eval.Eval("+1/0");
		assert(eval.GetErr() == EEE_DIVIDE_BY_ZERO);

		// An emtpy string
		eval.Eval("");
		assert(eval.GetErr() == EEE_WRONG_CHAR && strcmp(eval.GetErrPos(), "") == 0);
	}
	#endif

	// ============
	// Main program
	// ============

	#if 0

	int main() {
	#ifdef _DEBUG
		TestExprEval();
		TestExprCode();
	#endif
		static const char *errors[] = {
			"no error",
			"parentheses don't match",
			"invalid character",
			"division by zero",
			"undefined variable",
			"too much variables"
		};

		puts("Enter an expression (or an empty string to exit):");
		ExprEval eval;
		for(;;) {
			// Get a string from console
			static char buff[256];
			char *expr = gets(buff);

			// If the string is empty, then exit
			if(*expr == '\0')
				return 0;

			// Evaluate the expression
			double res = eval.Eval(expr);
			if(eval.GetErr() != EEE_NO_ERROR) {
				printf("  Error: %s at %s\n", errors[eval.GetErr()], eval.GetErrPos());
			} else {
				printf("  = %g\n", res);
			}
		}
	}

	#endif

#endif // end of Peter Kankowski's expression evaluator


namespace moon9
{
	moon9::string string::eval() const
	{
#ifndef _WIN32
		return *this;
#else
		ExprEval eval;

		moon9::string result( eval.Eval( (EVAL_CHAR *)this->c_str() ) );

		if( eval.GetErr() == EEE_NO_ERROR)
			return result;
		else
			return *this;
#endif
	}
}

std::ostream &operator <<( std::ostream &os, const moon9::strings &s )
{
	os << s.str();
	return os;
}
