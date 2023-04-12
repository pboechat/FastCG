#ifndef FASTCG_STRING_UTILS_H
#define FASTCG_STRING_UTILS_H

#include <vector>
#include <string>

namespace FastCG
{
	class StringUtils
	{
	public:
		inline static void Split(const std::string &rString, const std::string &rDelimiter, std::vector<std::string> &rTokens);
		inline static std::string Join(const std::vector<std::string> &rTokens, const std::string &rDelimiter = "");
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

#include <FastCG/Core/StringUtils.inc>

#endif
