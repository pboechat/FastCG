#ifndef FASTCG_STRING_UTILS_H
#define FASTCG_STRING_UTILS_H

#include <vector>
#include <string>

namespace FastCG
{
	class StringUtils
	{
	public:
		inline static void Tokenize(const std::string &rString, const std::string &rDelimiter, std::vector<std::string> &rTokens);
		inline static void LeftTrim(std::string &rString);
		inline static void RightTrim(std::string &rString);
		inline static void Trim(std::string &rString);
		inline static void Replace(std::string &rString, const std::string &rFrom, const std::string &rTo);
		inline static bool StartsWith(const std::string &rString1, const std::string &rString2);

	private:
		StringUtils() = delete;
		~StringUtils() = delete;
	};

}

#include <FastCG/StringUtils.inc>

#endif
