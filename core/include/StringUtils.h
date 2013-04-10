#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <vector>
#include <string>

class StringUtils
{
public:
	static void Tokenize(const std::string& rString, const std::string& rDelimiter, std::vector<std::string>& rTokens);
	static void LeftTrim(std::string& rString);
	static void RightTrim(std::string& rString);
	static void Trim(std::string& rString);
	static void Replace(std::string& rString, const std::string& rFrom, const std::string& rTo);

private:
	StringUtils();
	~StringUtils();

};

#endif