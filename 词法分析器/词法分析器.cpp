#include "pch.h"
#include <iostream>
#include<string>
#include <windows.h>
#define KEYWORD_LENGTH 33

using namespace std;

HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);

struct Error_Item{
	int type ;
	int error_line;
	string token;
};
typedef Error_Item error_item;

struct Table_Item
{
	string value;
	int total;
};
typedef Table_Item table_item;

table_item keyword[KEYWORD_LENGTH];//关键字表
table_item identifier[1000];//标识符表
error_item error_table[1000];//错误记录表
int error_length = 0;//错误记录表长
int identifier_length=0;//标识符表长
char buf[21];//缓冲区
int forward_ptr = 0;//向前指针,开始指针
char character;//当前读取字符
string word;//当前识别单词
int line = 1;//记录程序行数
int character_total = 0;//字符总数
int state = 0;//状态标志
int blank_flag = 0;//不跳过空格标志
int keyword_num = 0;//关键字个数
int identifier_num = 0;//标识符个数
int operator_num=0;//操作符个数
int constant_num=0;//常量个数
int delimiter_num = 0;//界符个数

void init_keyword()//关键字表初始化
{
	keyword[1].value = "auto";
	keyword[2].value = "break";
	keyword[3].value = "case";
	keyword[4].value = "char";
	keyword[5].value = "const";
	keyword[6].value = "continue";
	keyword[7].value = "default";
	keyword[8].value = "do";
	keyword[9].value = "double";
	keyword[10].value = "else";
	keyword[11].value = "enum";
	keyword[12].value = "extern";
	keyword[13].value = "float";
	keyword[14].value = "for";
	keyword[15].value = "goto";
	keyword[16].value = "if";
	keyword[17].value = "int";
	keyword[18].value = "long";
	keyword[19].value = "register";
	keyword[20].value = "return";
	keyword[21].value = "short";
	keyword[22].value = "signed";
	keyword[23].value = "sizeof";
	keyword[24].value = "static";
	keyword[25].value = "struct";
	keyword[26].value = "switch";
	keyword[27].value = "typedef";
	keyword[28].value = "union";
	keyword[29].value = "unsigned";
	keyword[30].value = "void";
	keyword[31].value = "volatile";
	keyword[32].value = "while";
	for (int i = 0;i < KEYWORD_LENGTH;i++)
	{
		keyword[i].total = 0;
	}
}

void init_identifier()//标识符表与次数表初始化
{
	for (int i = 0;i < 1000;i++)
	{
		identifier[i].value = "";
		identifier[i].total = 0;
	}
}

void init_error()//标识符表与次数表初始化
{
	for (int i = 0;i < 1000;i++)
	{
		error_table[i].type= 0;
		error_table[i].error_line = 0;
		error_table[i].token = "";
	}
}

void init_buf() //缓冲区初始化
{
	for (int i = 0;i < 21;i++)
	{
		buf[i] = { 0 };
	}	
}

FILE *init_program(char *addr) //打开目标程序文件
{
	FILE *fp;
	if (fp = fopen(addr, "r"))
		cout << "打开文件成功"<<endl<<line<<": ";
	else
	{
		cout << "打开文件失败"<<endl;
		exit(0);
	}
	return fp;
}

void read_program(FILE *fp)//读入缓冲区
{
	if (forward_ptr == 10) 
	{
		for (int i = 11; i <= 20; i++)
		{
			buf[i] = fgetc(fp);
		}
		forward_ptr++;
	}
	else if(forward_ptr==20)
	{
		for (int i = 1; i <= 10; i++)
		{
			buf[i] = fgetc(fp);
		}
		forward_ptr = 1;
	}
	else if (forward_ptr == 0)
	{
		for (int i = 1; i <= 10; i++)
		{
			buf[i] = fgetc(fp);
		}
		forward_ptr++;
	}
	else
	{
		forward_ptr++;
	}
}

void return_two_tuples(const string mark,const string value)//打印二元组函数并将字符总数加1
{
	character_total++;
	cout <<"<"<< mark << "," << value << ">";
}

bool is_letter(char c)//判断字母
{
	if ((c <= 'Z'&&c >= 'A') || (c <= 'z'&&c >= 'a'))
		return true;
	else
		return false;
}

bool is_number(char c)//判断数字
{
	if (c <= '9'&&c >= '0') 
		return true;
	else
		return false;
}

int find_identifier(string s)//搜索标识符表
{
	for (int i = 1;i <= identifier_length;i++)
	{
		if (s == identifier[i].value)//找到标识符
		{
			identifier[i].total++;
			return i;
		}		
	}
	if (identifier_length + 1 == 1000)//标识符表溢出
	{
		cout << "标识符表溢出";
		exit(0);
	}
	else//无对应标识符，创建新表项
	{
		identifier[identifier_length + 1] .value= s;
		identifier[identifier_length + 1].total++;
		identifier_length++;
		return identifier_length;
	}
}

void print_identifier()//打印标识符表
{
	cout << "===========================标识符表=========================="<<endl;
	cout<< "标识符			表中位置		出现次数"<<endl;
	for (int i = 1; i < identifier_length; i++)
	{
		cout.width(25);
		cout <<left<<identifier[i].value;
		cout << i<<"			"<< identifier[i].total<<endl;
	}
}

bool is_keyword(string s)   //判断关键字
{
	for(int i=1;i<= KEYWORD_LENGTH;i++)
		if (s == keyword[i].value)
		{
			keyword[i].total++;
			return true;
		}
	return false;
}

void print_keyword()//打印标识符表
{
	cout << "===========================关键字表==========================" << endl;
	cout << "关键字			表中位置		出现次数" << endl;
	for (int i = 1; i < KEYWORD_LENGTH; i++)
	{
		cout.width(25);
		cout << left << keyword[i].value;
		cout<< i << "			" << keyword[i].total << endl;
	}
}

void print_error()//打印错误表
{
	cout << "===========================错误表==========================" << endl;
	cout << "错误类型		出现行数		错误字符流" << endl;
	for (int i = 0; i < error_length; i++)
	{
		cout.width(25);
		string type_name[3] = { "意料之外的字符","常数错误","单双引号未闭合" };
		cout << left << type_name[error_table[i].type-1];
		cout << error_table[i].error_line << "			" << error_table[i].token << endl;
	}
}

void error(int type)//报错
{
	error_table[error_length].type = type;
	error_table[error_length].error_line = line;
	error_table[error_length].token = word;
	error_length++;
}

void lex_analyze(FILE *fp)//词法分析
{
	switch (state)
	{
	case 0://新字符初步分类判断
	{
		word = "";
		character = buf[forward_ptr];
		if (character == '{')
			int o = 0;
		if (character == -1)
		{
			SetConsoleTextAttribute(handle, 1 | 2 | 4 | 8);
			cout << endl << "词法分析结束" << endl;
			print_keyword();
			print_identifier();
			print_error();
			cout << endl << "语句行数：" << line << "		字符总数：" << character_total << endl ;
			cout << "各类单词个数：" << endl;
			cout << "关键字：" << keyword_num << " 标识符：" << identifier_num << " 运算符："<<operator_num<<" 常量："<<constant_num<<" 界符：" << delimiter_num << endl;
			exit(0);
		}
		if (is_letter(character))//标识符，关键字判断
			state = 1;
		else if (is_number(character))//常数判断
			state = 2;
		else
		{
			switch (character)
			{
			case '<':// < <= <>
			{
				state = 8;
				break;
			}
			case '>'://> >=
			{
				state = 9;
				break;
			}
			case '*':
			case '%':
			case '^':
			case '!':// 后缀‘=’ 判断
			{
				state = 11;
				break;
			}
			case '=':
			case '+':
			case '|':
			case '&'://重复后缀与‘=’后缀
			{
				state = 12;
				break;
			}
			case '~':
			case '.':
			case'?':
			{
				state = 13;
				break;
			}
			case '-':
			{
				state = 14;
				break;
			}
			//单字符判断
			case '(':
			case ')':
			case ';':
			case '\\':
			case '{':
			case '}':
			case ',':
			case '[':
			case ']':
			{
				state = 15;
				break;
			}
			case '"'://字符串判断
			{
				state = 16;
				break;
			}
			case '\'':// 字符常量判断
			{
				state = 17;
				break;
			}
			case '/'://注释判断
			{
				state = 18;
				break;
			}
			default:
			{
				word += character;
				error(1);
				state = 22;
				break;
			}
			}
		}
		break;
	}
	case 1://标识符与关键字判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (is_letter(character) || is_number(character))
		{
			state = 1;
		}
		else
		{
			state = 0;
			if (is_keyword(word))
			{
				SetConsoleTextAttribute(handle, 4 | 8);
				return_two_tuples("关键字", word);
				keyword_num++;
			}
			else
			{
				int num = find_identifier(word);
				SetConsoleTextAttribute(handle, 2 | 8);
				return_two_tuples(word, to_string(num));
				identifier_num++;
			}
		}
		break;
	}
	case 2://常数判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (is_number(character))
		{
			state = 2;
		}
		else if (character == '.')
		{
			state = 3;
		}
		else if (character == 'E'||character == 'e')
		{
			state = 5;
		}
		else
		{
			SetConsoleTextAttribute(handle, 1|2| 8);
			return_two_tuples("常量", word);
			constant_num++;
			state = 0;
		}
		break;
	}
	case 3://进入小数状态
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (is_number(character))
		{
			state = 4;
		}
		else
		{
			error(2);
			state = 22;
		}
		break;
	}
	case 4://小数状态循环
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == 'E'|| character == 'e')
		{
			state = 5;
		}
		else if (!is_number(character))
		{
			SetConsoleTextAttribute(handle, 1 | 2 | 8);
			return_two_tuples("常量", word);
			constant_num++;
			state = 0;
		}
		break;
	}
	case 5://科学计数法状态
	{
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '+' || character == '-')
		{
			state = 6;
		}
		else if (is_number(character))
		{
			state = 7;
		}
		else
		{
			error(2);
			state = 22;
		}
		break;
	}
	case 6://科学计数法循环
	{
		char character1= character;
		read_program(fp);
		character = buf[forward_ptr];
		if (is_number(character))
		{
			word += 'E';
			word+=character1;
			state = 7;
		}
		else
		{
			word += 'E';
			word += character1;
			error(2);
			state = 22;
		}
		break;
	}
	case 7://科学计数法结束状态
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (!is_number(character))
		{
			SetConsoleTextAttribute(handle, 1 | 2 | 8);
			return_two_tuples("常量", word);
			constant_num++;
			state = 0;
		}
		break;
	}
	case 8:// <= <> < <<=判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '=')
		{
			SetConsoleTextAttribute(handle,  2 |4| 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
		}
		else if (character == '>')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
		}
		else if (character == '<')
		{
			state = 10;
			break;
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
		}
		state = 0;
		break;
	}
	case 9:// >= > >>=判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '=')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
		}
		else if (character == '>')
		{
			state = 10;
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
			operator_num++;
		}
		state = 0;
		break;
	}
	case 10://<<= >>=
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '=')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
			state = 0;
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			operator_num++;
			return_two_tuples("运算符", word);
			state = 0;
		}
		break;
	}
	case 11:// X X= 判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '=')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
		}
		state = 0;
		break;
	}
	case 12:// XX X= X 判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == word[0] || character == '=')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
			state = 0;
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
			operator_num++;
			state = 0;
		}
		break;
	}
	case 13://  ~
	{
		SetConsoleTextAttribute(handle, 2 | 4 | 8);
		return_two_tuples("运算符", string(1, character));
		operator_num++;
		state = 0;
		read_program(fp);
		break;
	}
	case 14://  -  -- -= -> 判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == word[0] || character == '=' || character == '>')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word + character);
			operator_num++;
			read_program(fp);
			state = 0;
		}
		else
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
			operator_num++;
			state = 0;
		}
		break;
	}
	case 15://( ) ; \ { } , [ ] 判断
	{
		SetConsoleTextAttribute(handle, 1 | 4 | 8);
		return_two_tuples("界符", string(1, character));
		delimiter_num++;
		state = 0;
		read_program(fp);
		break;
	}
	case 16://字符串模式
	{
		word += character;
		blank_flag = 1;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '"')
		{
			word += character;
			SetConsoleTextAttribute(handle, 1 | 2 | 8);
			blank_flag = 0;
			return_two_tuples("常量", word );
			constant_num++;
			read_program(fp);
			state = 0;
		}
		else if(character=='\n')
		{
			error(3);
			state = 22;
		}
		break;
	}
	case 17:// 字符常量判断
	{
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '\'')//空或 '字符常量
		{
			read_program(fp);
			character = buf[forward_ptr];
			if (character == '\'')
			{
				SetConsoleTextAttribute(handle, 1 | 2 | 8);
				return_two_tuples("常量", "'''");
				constant_num++;
				read_program(fp);
			}
			else
			{
				SetConsoleTextAttribute(handle, 1 | 2 | 8);
				return_two_tuples("常量", "''");
				constant_num++;
			}
			state = 0;
			break;
		}
		else if (character == '\\')//转义字符
		{
			word += character;
			read_program(fp);
			character = buf[forward_ptr];
		}
		else if (character == '\n')
		{
			blank_flag = 1;
			error(3);
			state = 22;
			break;
		}
		word += character;
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '\'')//其他字符常量
		{
			word += character;
			SetConsoleTextAttribute(handle, 1 | 2 | 8);
			return_two_tuples("常量", word);
			read_program(fp);
			state = 0;
		}
		else//出错
		{
			error(3);
			state = 22;
		}
		break;
	}
	case 18://注释判断
	{
		read_program(fp);
		character = buf[forward_ptr];
		if (character == '=')
		{
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
			identifier_num++;
			state = 0;
			read_program(fp);
		}
		else if (character == '*')
		{
			blank_flag = 1;
			state = 19;
			read_program(fp);
		}
		else if (character == '/')
		{
			state = 21;
			read_program(fp);
		}
		else
		{
			state = 0;
			SetConsoleTextAttribute(handle, 2 | 4 | 8);
			return_two_tuples("运算符", word);
			identifier_num++;
		}
		break;
	}
	case 19://记录多行注释状态
	{
		character = buf[forward_ptr];
		if (character == '\n')
		{
			SetConsoleTextAttribute(handle, 1 | 2 | 4 | 8);
			cout << "=====跳过注释=====";
			cout << endl << line << ": ";
			line++;
			word += character;
			read_program(fp);
		}
		else if (character != '*')
		{
			word += character;
			read_program(fp);
		}
		else
		{
			state = 20;
			read_program(fp);
		}
		break;
	}
	case 20://多行注释结束状态
	{
		character = buf[forward_ptr];
		if (character == '/')
		{
			SetConsoleTextAttribute(handle, 1 | 2 | 4 | 8);
			cout <<"=====跳过注释=====";
			read_program(fp);
			blank_flag = 0;
			state = 0;
		}
		else
		{
			state = 19;
		}
		break;
	}
	case 21://单行注释
	{
		blank_flag = 1;
		character = buf[forward_ptr];
		if (character != '\n')
		{
			word += character;
			read_program(fp);
		}
		else
		{
			blank_flag = 0;
			SetConsoleTextAttribute(handle, 1 | 2 | 4 | 8);
			cout << "=====跳过注释=====";
			read_program(fp);
			state = 0;
			line++;
			SetConsoleTextAttribute(handle,1| 2 | 4 | 8);
			cout << endl << line << ": ";
		}
		break;
	}
	case 22://错误处理
	{
		state = 0;
		read_program(fp);
		blank_flag = 0;
		break;
	}
	default:
		break;
	}
}

void jump_blank(FILE *fp)//跳过空白，并记录行数
{
	while (1)
	{
		if (buf[forward_ptr] == '\n')
		{
			read_program(fp);
			line++;
			SetConsoleTextAttribute(handle, 1|2 | 4 | 8);
			cout << endl << line << ": ";
		}
		else if (buf[forward_ptr] == ' ' || buf[forward_ptr] == '\t')
		{
			read_program(fp);
		}
		else
			return;
	}
}

int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		cout << "错误的参数个数！" << endl;
		exit(0);
	}
	init_keyword();
	init_buf();
	FILE *fp = init_program(argv[1]);
	read_program(fp);
	while (1)
	{
		if(blank_flag==0)
			jump_blank(fp);
		lex_analyze(fp);
	}
}

