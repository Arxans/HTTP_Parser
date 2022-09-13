#include "HTTP.h"

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

