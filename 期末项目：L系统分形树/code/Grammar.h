#pragma once
#include "shader.h"
#include "texture.h"
#include <vector>
#include <map>
#include <ctime>
#include <string>
using namespace std;

class Grammar {
private:
	std::string Gname;
	int level;
	std::string start;
	std::string result;
	vector<pair<char, vector<std::string> > >generations;

public:

	Grammar() {}

	void clear();

	int find(char ch);
	std::string search(char ch);

	void addGeneration(const char& ch, const string& ref);
	void iterateFor(int num);

	void setGrammarName(const string& ref);
	void setStart(const string& ref);
	void setLevel(int num);
	void setResult(string& target);

	std::string getGrammarName();
	int getLevel();
	std::string getResult();
};