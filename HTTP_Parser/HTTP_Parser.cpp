#include <iostream>
#include <optional>
#include <map>
#include <algorithm>

class HTTP
{
public:
    HTTP(const std::string& sRequest);
    virtual ~HTTP() {};

protected:
    enum class HTTP_TYPES
    {
        GET = 0,
        POST,
        CONNECT,
        PUT,
        HEAD
    };

    enum class HTTP_HEADERS
    {
        Host = 0,
        UserAgent,
        Accept,
        Connection
    };

    std::optional<HTTP_TYPES> GetType() const { return m_Type; }
    std::optional<std::string> GetURL() const { return m_URL; }

    std::optional<std::string> GetHeaderValue(std::string& sHeader) const;
    std::optional<std::string> GetHeaderValue(HTTP_HEADERS header) const;

private:
    size_t FindCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);

private:
    std::optional<HTTP_TYPES> m_Type;
    std::optional<std::string> m_URL;

    std::map<std::string, HTTP_TYPES> m_Types;
    std::map<std::string, HTTP_HEADERS> m_Headers;
    
    std::map<HTTP_HEADERS, std::string> m_HeadersValue;
};

HTTP::HTTP(const std::string& sRequest)
{
    m_Types =
    {
        { "GET",        HTTP_TYPES::GET     },
        { "POST",       HTTP_TYPES::POST    },
        { "CONNECT",    HTTP_TYPES::CONNECT },
        { "PUT",        HTTP_TYPES::PUT     },
        { "HEAD",       HTTP_TYPES::HEAD    }
    };

    m_Headers = 
    {
        { "host",           HTTP_HEADERS::Host          },
        { "user-agent",     HTTP_HEADERS::UserAgent     },
        { "accept",         HTTP_HEADERS::Accept        },
        { "connection",     HTTP_HEADERS::Connection    },

        /*added more*/
    };

    bool bTypeUrlExist = false;
    std::string_view str_view;

    // parse
    size_t startPos = 0;
    auto endPos = sRequest.find_first_of('\n');

    while (endPos != std::string::npos && startPos < endPos)
    {
		str_view = std::string_view(sRequest).substr(startPos, endPos - startPos);

        if (!bTypeUrlExist)
        {
            // find Type and URL
            for (const auto& type : m_Types)
            {
                auto typePos = FindCaseInsensitive(std::string{ str_view }, type.first);
                if (typePos == std::string::npos)
                    continue;

                m_Type = type.second;
                // else Error

                size_t charsOffset = type.first.length();
                // need to check exception
                m_URL = str_view.substr(charsOffset + 1, str_view.length() - charsOffset);

                bTypeUrlExist = true;
                break;
            }
        }
        else
        {
            for (const auto& header : m_Headers)
            {

                // maybe need copy string from start line to ':' and find this string in map
                auto headerPos = FindCaseInsensitive(std::string{ str_view }, header.first);
                if (headerPos == std::string::npos)
                    continue;

                size_t charsOffset = header.first.length() + 1; // + ':'
                m_HeadersValue[header.second] = str_view.substr(charsOffset + 1, str_view.length() - charsOffset);

                break;
            }
        }

        str_view = std::string_view(sRequest).substr(endPos + 1, sRequest.length() - endPos);
        startPos = endPos + 1;

        if (auto nextEndPos = str_view.find_first_of('\n'); nextEndPos != std::string::npos)
            endPos += nextEndPos + 1;
        else
            endPos = str_view.empty() ? std::string::npos : sRequest.size() - 1;
    }
}

std::optional<std::string> HTTP::GetHeaderValue(std::string& sHeader) const
{
    if (sHeader.empty())
        return {};

    std::transform(sHeader.begin(), sHeader.end(), sHeader.begin(), tolower);

    auto it = m_Headers.find(sHeader);
    if (it == m_Headers.end())
        return {};

    return GetHeaderValue(it->second);
}

std::optional<std::string> HTTP::GetHeaderValue(HTTP_HEADERS header) const
{
    auto it = m_HeadersValue.find(header);
    if (it == m_HeadersValue.end())
        return {};

    return { it->second }; // TODO: check return it->second without {}
}

size_t HTTP::FindCaseInsensitive(std::string data, std::string toSearch, size_t pos)
{
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);

    return data.find(toSearch, pos);
}

int main()
{
    std::string sRequest =
        "GET /wiki/http HTTP/1.1\n"
        "Host: ru.wikipedia.org\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\n"
        "Accept: text/html\n"
        "Connection: close";

    HTTP httpRequest(sRequest);
}

