#include <algorithm>
#include <cctype>
#include <functional>

namespace FastCG
{
    void StringUtils::Split(const std::string &rString, const std::string &rDelimiter,
                            std::vector<std::string> &rTokens)
    {
        size_t currentPosition = 0;
        size_t nextPosition = std::string::npos;
        while (currentPosition != std::string::npos)
        {
            nextPosition = rString.find_first_of(rDelimiter, currentPosition);

            if (nextPosition != currentPosition)
            {
                rTokens.emplace_back(rString.substr(currentPosition, nextPosition - currentPosition));
            }

            currentPosition = rString.find_first_not_of(rDelimiter, nextPosition);
        }
    }

    std::string StringUtils::Join(const std::vector<std::string> &rTokens, const std::string &rDelimiter /* = "" */)
    {
        if (rTokens.empty())
        {
            return {};
        }

        auto joined = rTokens.front();
        for (size_t i = 1; i < rTokens.size(); ++i)
        {
            joined += rDelimiter + rTokens[i];
        }
        return joined;
    }

    void StringUtils::LeftTrim(std::string &rString)
    {
        rString.erase(rString.begin(),
                      std::find_if(rString.begin(), rString.end(), [](char c) { return !std::isspace(c); }));
    }

    void StringUtils::RightTrim(std::string &rString)
    {
        rString.erase(std::find_if(rString.rbegin(), rString.rend(), [](char c) { return !std::isspace(c); }).base(),
                      rString.end());
    }

    void StringUtils::Trim(std::string &rString)
    {
        RightTrim(rString);
        LeftTrim(rString);
    }

    void StringUtils::Replace(std::string &rString, const std::string &rFrom, const std::string &rTo)
    {
        size_t startPosition;
        while ((startPosition = rString.find(rFrom)) != std::string::npos)
        {
            rString.replace(startPosition, rFrom.length(), rTo);
        }
    }

    bool StringUtils::StartsWith(const std::string &rString1, const std::string &rString2)
    {
        auto size = rString2.size();
        if (rString1.size() < size)
        {
            return false;
        }
        return rString1.substr(0, size) == rString2;
    }

    bool StringUtils::EndsWith(const std::string &rString1, const std::string &rString2)
    {
        auto size1 = rString1.size();
        auto size2 = rString2.size();
        if (size1 < size2)
        {
            return false;
        }
        return rString1.substr(size1 - size2, size2) == rString2;
    }

}