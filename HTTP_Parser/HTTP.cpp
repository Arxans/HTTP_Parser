#include "HTTP.h"

#include <algorithm>
#include <sstream>


HTTP::HTTP()
{
    InitTypes();
    InitHeaders();
}

bool HTTP::LoadFromFile(const std::string& fileName)
{
    std::ifstream fileStream(fileName);
    return Parse(fileStream);
}

bool HTTP::LoadFromString(const std::string& sRequest)
{
    std::istringstream stringStream(sRequest);
    return Parse(stringStream);
}

std::optional<std::string> HTTP::GetHeaderValue(std::string sHeader) const
{
    if (sHeader.empty())
        return {};

    std::transform(sHeader.begin(), sHeader.end(), sHeader.begin(), tolower);

    auto it = m_HeadersName.find(sHeader);
    if (it == m_HeadersName.end())
        return {};

    return GetHeaderValue(it->second);
}

std::optional<std::string> HTTP::GetHeaderValue(HTTP_HEADERS header) const
{
    auto it = m_HeadersValue.find(header);
    if (it == m_HeadersValue.end())
        return {};

    return it->second;
}

void HTTP::InitTypes()
{
    m_Types =
    {
        { "GET",        HTTP_TYPES::GET     },
        { "POST",       HTTP_TYPES::POST    },
        { "CONNECT",    HTTP_TYPES::CONNECT },
        { "PUT",        HTTP_TYPES::PUT     },
        { "HEAD",       HTTP_TYPES::HEAD    }
    };
}

void HTTP::InitHeaders()
{
    m_HeadersName =
    {
        { "host",               HTTP_HEADERS::Host              },
        { "user-agent",         HTTP_HEADERS::UserAgent         },
        { "accept",             HTTP_HEADERS::Accept            },
        { "accept-charset",     HTTP_HEADERS::AcceptCharset     },
        { "accept-encoding",    HTTP_HEADERS::AcceptEncoding    },
        { "accept-language",    HTTP_HEADERS::AcceptLanguage    },
        { "connection",         HTTP_HEADERS::Connection        },
        { "content-type",       HTTP_HEADERS::ContentType       },
        { "content-length",     HTTP_HEADERS::ContentLength     },
        { "authorization",      HTTP_HEADERS::Authorization     },
        { "from",               HTTP_HEADERS::From              },
    };
}

bool HTTP::Parse(std::istream& stream)
{
    if (stream.fail())
        return false;

    bool bResult = true;

    /* for text with '\n' symbols
    * example:
    *       POST /cgi-bin/process.cgi HTTP/1.1\n
    *       User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n
    *       Host: www.example.com\n
    *       Content-Type: application/x-www-form-urlencoded\n
    */
    std::string line;
    while (std::getline(stream, line).good() && !line.empty())
    {
        if (!ParseLine(line))
            bResult = false;
    }

    /* for single lines without '\n'
    * example:
    *       Connection: Keep-Alive
    */
    if (!line.empty())
    {
        if (!ParseLine(line))
            bResult = false;
    }

    return bResult;
}

bool HTTP::ParseLine(std::string_view line)
{
    return (!m_Type || !m_URL) 
        ? ParseTypeAndURL(line) 
        : ParseHeadersValues(line);
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

    m_URL = str_view.substr(spacePos + 1, str_view.size() - spacePos);

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
    for (const auto& header : m_HeadersName)
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