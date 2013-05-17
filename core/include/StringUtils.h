#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

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
	static bool StartsWith(const std::string& rString1, const std::string& rString2);

private:
	StringUtils();
	~StringUtils();

};

#endif
