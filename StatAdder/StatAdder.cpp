// StatAdder.cpp: определяет точку входа для консольного приложения.
//input files: saR.txt, enU.txt, meN.txt 
//output files: stE.txt

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <array>

using namespace std;

//in
string const STAT_FILE_NAME = "saR.txt";
string const SCALE_FILE_NAME = "enQ.txt";
string const NAMES_FILE_NAME = "meN.txt";
//out
string const OUT_FILE_NAME = "stE.txt";

const int MAX_COLS = 40;
const int MAX_ROWS = 2000;
string scaleLetters;

class Method //method statistic with name
{
public:
	bool setCell(int & i, int & j, int & dig)
	{
		if (i >= 0 && i < MAX_ROWS && j >= 0 && j < MAX_COLS) {
			table_[i][j] = dig; return true;
		}
		return false;
	}
	void setName(string s) { name_ = s; }
	string getName() const { return name_; }
	int getCell(int & i, int & j) const { return table_[i][j]; }
	int getSizeIndex(int & size)
	{
		for (int i = 0; i < MAX_ROWS; i++)
		{
			if (size <= table_[i][0]) return i;
		}
		return MAX_ROWS - 1;
	}
	vector<double> getRow(int index)
	{
		vector<double> row;
		row.reserve(MAX_COLS - 2);
		for (int i = 0; i < MAX_COLS - 2; i++)
		{
			row.push_back(static_cast<double>(table_[index][i + 2]) / table_[index][1]);
		}
		return row;

	}
	int getSize() const { return rows_; }
private:
	int rows_ = 9;
	int cols_ = 0;
	int table_[MAX_ROWS][MAX_COLS];       //row: text length, overall texts, char errors
	string name_;
};

class MethodNames
{
public:
	MethodNames()
	{
		nums_.reserve(10);
		names_.reserve(10);
	}
	void addNum(int & num)
	{
		nums_.push_back(num);
		number_++;
	}
	void addName(string name)
	{
		if (names_.size() + 1 <= number_) {
			names_.push_back(name);
		}
	}
	string getName(int num)
	{
		return names_[num];
	}
	bool findNum(int num)
	{
		if (find(nums_.begin(), nums_.end(), num) != nums_.end())
			return true;

		return false;
	}
private:
	int number_ = 0;
	vector<int> nums_;
	vector<string> names_;

};

void skipLine(ifstream & fin)
{
	fin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void getNumbers(vector<int> & result, const string & s) { //get numbers from string
	bool found = false;
	int number = 0;

	for (string::size_type i = 0; i < s.length(); i++) {
		const char ch = s[i];
		if (ch >= '0' && ch <= '9') {
			const int digit = ch - '0';
			number = number * 10 + digit;
			found = true;
		}
		else {
			if (found) {
				result.push_back(number);

				number = 0;
				found = false;
			}
		}
	}

	if (found) {
		result.push_back(number);
	}
}

void readMethodNames(ifstream & fin, MethodNames &mn)
{
	int nM;
	fin >> nM;
	for (int iM = 0; iM < nM; iM++)
	{
		int n;
		fin >> n;
		mn.addNum(n);
	}
	for (int iM = 0; iM < nM; iM++)
	{
		string s;
		fin >> s;
		mn.addName(s);
	}
}

void readMethods(ifstream & fin, vector<Method> & methods, MethodNames & mn, int cols) //reading error statistic table about methods
{
	int nFrags;
	char c;
	int nMethods;
	fin >> nFrags >> nMethods >> c;
	
	for (int iF = 0; iF < nFrags; iF++)
	{
		skipLine(fin);
		for (int iM = 0; iM < nMethods * 2 + 1; iM++)
		{
			skipLine(fin);
		}

	}
	skipLine(fin);
	for (int iM = 0; iM < nMethods; iM++) {
		int ROWS = 9; //???????????????????????????????????????????????????????????????????????
		Method cMethod;
		string line;
		vector<int> numbers;
		getline(fin, line);
		if (mn.findNum(iM + 1))
		{
			cMethod.setName(mn.getName(iM));
		}
		
		for (int i = 0; i < 4 + ROWS; i++)
		{
			skipLine(fin);
		}


		numbers.reserve(100);
		for (int i = 0; i < ROWS; i++)
		{
			getline(fin, line);
			numbers.clear();
			getNumbers(numbers, line);
			for (int iCol = 0; iCol < cols; iCol++)
			{
				cMethod.setCell(i, iCol, numbers[iCol]);
			}
		}
		skipLine(fin);
		if (mn.findNum(iM))
		{
			methods.push_back(cMethod);
		}

	}
}

void printMethods(ofstream & fout, vector<Method> & methods)
{
	fout << methods.size() << endl;
	for (auto & method : methods)
	{
		fout << method.getName() << endl;
		int cols = scaleLetters.size();
		
		for (int i = 0; i < cols; i++)
		{
			fout << scaleLetters[i] << '\t';
		}
		fout << endl;

		for (int row = 0; row < method.getSize(); row++)
		{
			for (int col = 0; col < cols; col++)
			{
				fout << method.getCell(row, col) << '\t';
			}
			fout << endl;
		}
	}

}

string readScaleLetters(ifstream & fin)
{
	int dig;
	string s;
	fin >> dig >> dig >> dig >> dig;
	s.reserve(dig);
	for (int i = 0; i < dig; i++)
	{
		char c;
		fin >> c;
		s += c;

	}
	return s;
}

int main()
{
	setlocale(LC_ALL, "rus");

	ifstream fin;
	ofstream fout;

	fin.open(SCALE_FILE_NAME, ios_base::in);
	if (!fin.is_open())
		return 1;
	scaleLetters = "NK";
	scaleLetters += readScaleLetters(fin);
	fin.close(); //enQ.txt reading complete

	fin.open(NAMES_FILE_NAME, ios_base::in);
	if (!fin.is_open())
		return 1;
	MethodNames mn;
	readMethodNames(fin, mn);
	fin.close(); //meN.txt reading comlete

	fin.open(STAT_FILE_NAME, ios_base::in);
	if (!fin.is_open())
		return 1;
	vector<Method> methods;
	methods.reserve(10);
	readMethods(fin, methods, mn, scaleLetters.size());
	fin.close();

	fout.open(OUT_FILE_NAME, ios_base::out);
	printMethods(fout, methods);
	fout.close();


    return 0;
}

