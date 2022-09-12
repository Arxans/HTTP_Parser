#include <iostream>
#include <optional>
#include <map>

class HTTP
{
public:
    HTTP(const std::string& sRequest)
    {
        // получить строку, распарсить её.
        // parse string
    };
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

    std::optional<HTTP_TYPES> GetType() const
    {
        return m_httpType;
    }

    std::optional<std::string> GetHeaderValue(HTTP_HEADERS header) const
    {
        auto it = m_httpHeadersMap.find(header);
        if (it == m_httpHeadersMap.end())
            return {};

        return { it->second }; // TODO: check return it->second without {}
        
    }

private:
    std::optional<HTTP_TYPES> m_httpType;
    std::map<HTTP_HEADERS, std::string> m_httpHeadersMap;
};

int main()
{
    std::string sRequest = R"(
        GET /wiki/http HTTP/1.1
        Host: ru.wikipedia.org
        User - Agent : Mozilla / 5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko / 2008050509 Firefox / 3.0b5
        Accept : text / html
        Connection : close
    )";
}
