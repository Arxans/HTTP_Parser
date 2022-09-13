#include "HTTP.h"

#include <algorithm>


HTTP::HTTP(const std::string& sRequest)
{
    InitTypes();
    InitHeaders();

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

size_t HTTP::FindCaseInsensitive(std::string data, std::string toSearch, size_t pos)
{
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);

    return data.find(toSearch, pos);
}