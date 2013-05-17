#include <StringUtils.h>

#include <algorithm>
#include <cctype>

StringUtils::StringUtils()
{
}

StringUtils::~StringUtils()
{
}

void StringUtils::Tokenize(const std::string& rString, const std::string& rDelimiter, std::vector<std::string>& rTokens)
{
	unsigned int currentPosition = 0;
	unsigned int nextPosition = std::string::npos;

	while (currentPosition != std::string::npos)
	{
		nextPosition = rString.find_first_of(rDelimiter, currentPosition);

		if (nextPosition != currentPosition)
		{
			std::string token = rString.substr(currentPosition, nextPosition - currentPosition);
			rTokens.push_back(token);
		}

		currentPosition = rString.find_first_not_of(rDelimiter, nextPosition);
	}
}

void StringUtils::LeftTrim(std::string& rString)
{
	rString.erase(rString.begin(), std::find_if(rString.begin(), rString.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void StringUtils::RightTrim(std::string& rString)
{
	rString.erase(std::find_if(rString.rbegin(), rString.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), rString.end());
}

void StringUtils::Trim(std::string& rString)
{
	RightTrim(rString);
	LeftTrim(rString);
}

void StringUtils::Replace(std::string& rString, const std::string& rFrom, const std::string& rTo)
{
	unsigned int startPosition;

	while ((startPosition = rString.find(rFrom)) != std::string::npos)
	{
		rString.replace(startPosition, rFrom.length(), rTo);
	}
}

bool StringUtils::StartsWith(const std::string& rString1, const std::string& rString2)
{
	unsigned int size = rString2.size();
	if (rString1.size() < size)
	{
		return false;
	}

	std::string start = rString1.substr(0, size);

	return start == rString2;
}