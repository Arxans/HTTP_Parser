#pragma once

#include <string>
#include <optional>
#include <map>
#include <fstream>

class HTTP
{
private:
    using TypeName = std::string;
    using HeaderName = std::string;
    using HeaderValue = std::string;

public:
    HTTP();
    virtual ~HTTP() {};

    bool LoadFromFile(const std::string& fileName);
    bool LoadFromString(const std::string& sRequest);

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

    bool Parse(std::istream& stream);

    bool ParseLine(std::string_view line);
    bool ParseTypeAndURL(std::string_view str_view);
    bool ParseHeadersValues(std::string_view str_view);

    bool CompareCaseInsensitive(std::string str1, std::string str2);

private:
    std::optional<HTTP_TYPES> m_Type;
    std::optional<std::string> m_URL;

    std::map<TypeName, HTTP_TYPES> m_Types;
    std::map<HeaderName, HTTP_HEADERS> m_HeadersName;
    
    std::map<HTTP_HEADERS, HeaderValue> m_HeadersValue;
};
