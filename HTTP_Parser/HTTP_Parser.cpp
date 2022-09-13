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
    bool ParseTypeAndURL(std::string_view str_view);
    bool ParseHeadersValues(std::string_view str_view);

    bool CompareCaseInsensitive(std::string str1, std::string str2);
    size_t FindCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);

private:
    using TypeName = std::string;
    using HeaderName = std::string;
    using HeaderValue = std::string;

    std::optional<HTTP_TYPES> m_Type;
    std::optional<std::string> m_URL;

    std::map<TypeName, HTTP_TYPES> m_Types;
    std::map<HeaderName, HTTP_HEADERS> m_Headers;
    
    std::map<HTTP_HEADERS, HeaderValue> m_HeadersValue;
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

    size_t startPos = 0;
    auto endPos = sRequest.find_first_of('\n');

    // parse line by line
    while (endPos != std::string::npos && startPos < endPos)
    {
		str_view = std::string_view(sRequest).substr(startPos, endPos - startPos);

        if (!bTypeUrlExist)
        {
            bTypeUrlExist = ParseTypeAndURL(str_view);
        }
        else
        {
            ParseHeadersValues(str_view);
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

bool HTTP::ParseTypeAndURL(std::string_view str_view)
{
    auto spacePos = str_view.find_first_of(' ');
    if (spacePos == std::string::npos)
        return false;

    auto sType = str_view.substr(0, spacePos);
    
    for (const auto& type : m_Types)
    {
        if (CompareCaseInsensitive(type.first, std::string{ sType }))
        {
            m_Type = type.second;
            break;
        }
    }

    if (!m_Type)
        return false;

    m_URL = str_view.substr(spacePos + 2, str_view.size() - spacePos - 1);

    if (!m_URL)
        return false;

    return true;
}

bool HTTP::ParseHeadersValues(std::string_view str_view)
{
    auto colonPos = str_view.find_first_of(':');
    if (colonPos == std::string::npos)
        return false;

    auto sHeader = str_view.substr(0, colonPos);
    for (const auto& header : m_Headers)
    {
        if (CompareCaseInsensitive(header.first, std::string{ sHeader }))
        {
            m_HeadersValue[header.second] = str_view.substr(colonPos + 2, str_view.length() - colonPos - 1);
            break;
        }
    }

    return true;
}

bool HTTP::CompareCaseInsensitive(std::string str1, std::string str2)
{
    std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);

    return str1 == str2;
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

