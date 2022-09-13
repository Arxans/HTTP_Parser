#include <iostream>
#include <optional>
#include <map>
#include <algorithm>

class HTTP
{
private:
    using TypeName = std::string;
    using HeaderName = std::string;
    using HeaderValue = std::string;

public:
    HTTP(const std::string& sRequest);
    virtual ~HTTP() {};

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
        AcceptCharset,
        AcceptEncoding,
        AcceptLanguage,
        Connection,
        ContentType,
        ContentLength,
        Authorization,
        From
    };

    std::optional<HTTP_TYPES> GetType() const { return m_Type; }
    std::optional<std::string> GetURL() const { return m_URL; }

    std::optional<std::string> GetHeaderValue(std::string sHeader) const;
    std::optional<std::string> GetHeaderValue(HTTP_HEADERS header) const;

private:
    void InitTypes();
    void InitHeaders();

    bool ParseTypeAndURL(std::string_view str_view);
    bool ParseHeadersValues(std::string_view str_view);

    bool CompareCaseInsensitive(std::string str1, std::string str2);
    size_t FindCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);

private:
    std::optional<HTTP_TYPES> m_Type;
    std::optional<std::string> m_URL;

    std::map<TypeName, HTTP_TYPES> m_Types;
    std::map<HeaderName, HTTP_HEADERS> m_HeadersName;
    
    std::map<HTTP_HEADERS, HeaderValue> m_HeadersValue;
};

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

int main()
{
    // HTTP request 1
    std::string sRequest1 =
        "GET /wiki/http HTTP/1.1\n"
        "Host: ru.wikipedia.org\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\n"
        "Accept: text/html\n"
        "Connection: close";

    HTTP httpRequest1(sRequest1);

    auto type1 = httpRequest1.GetType();
    auto URL1 = httpRequest1.GetURL();
    auto headerValue1_1 = httpRequest1.GetHeaderValue(HTTP::HTTP_HEADERS::Accept);
    auto headerValue1_2 = httpRequest1.GetHeaderValue("Host");
    auto headerValue1_3 = httpRequest1.GetHeaderValue("Host1");    // nullopt


    // HTTP request 2
    std::string sRequest2 =
        "GET /hello.htm HTTP/1.1\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
        "Host: www.example.com\n"
        "Accept-Language: ru-ru\n"
        "Accept-Encoding: gzip, deflate\n"
        "Connection: Keep-Alive\n";

    HTTP httpRequest2(sRequest2);

    auto type2 = httpRequest2.GetType();
    auto URL2 = httpRequest2.GetURL();
    auto headerValue2_1 = httpRequest2.GetHeaderValue(HTTP::HTTP_HEADERS::AcceptEncoding);
    auto headerValue2_2 = httpRequest2.GetHeaderValue("Accept-Language");
    auto headerValue2_3 = httpRequest2.GetHeaderValue("ACCEPT-Language");


    // HTTP request 3
    std::string sRequest3 =
        "POST /cgi-bin/process.cgi HTTP/1.1\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
        "Host: www.example.com\n"
        "Content-Type: application/x-www-form-urlencoded\n"
        "Content-Length: length\n"
        "Accept-Language: ru-ru\n"
        "Accept-Encoding: gzip, deflate\n"
        "Connection: Keep-Alive\n";
 
    HTTP httpRequest3(sRequest3);

    auto type3 = httpRequest3.GetType();
    auto URL3 = httpRequest3.GetURL();
    auto headerValue3_1 = httpRequest3.GetHeaderValue(HTTP::HTTP_HEADERS::ContentType);
    auto headerValue3_2 = httpRequest3.GetHeaderValue("Content-Type");
}

