#include "HTTP.h"
#include <iostream>
#include <fstream>


void WriteType(const HTTP& httpRequest);
void WriteURL(const HTTP& httpRequest);

void HttpRequest1();
void HttpRequest2();
void HttpRequest3();


int main(int argc, char** argv)
{
    // small examples of work

    HTTP httpRequest0;
    if (argc == 2)      
    {
        if (httpRequest0.LoadFromFile(argv[1]))
            std::cout << "file read" << std::endl;
    }
    else
    {
        std::string line;
        while (std::getline(std::cin, line).good() && !line.empty())
            httpRequest0.LoadFromString(line);
    }

    std::cout << std::endl << "=== httpRequest0 ===" << std::endl;
    WriteType(httpRequest0);
    WriteURL(httpRequest0);

    HttpRequest1();
    HttpRequest2();
    HttpRequest3();
}


void WriteType(const HTTP& httpRequest)
{
    auto httpType = httpRequest.GetType();
    if (!httpType.has_value())
    {
        std::cout << "Type is empty!" << std::endl;
        return;
    }

    std::string sType;
    switch (httpType.value())
    {
    case HTTP::HTTP_TYPES::GET:
        sType = "GET";
        break;
    case HTTP::HTTP_TYPES::POST:
        sType = "POST";
        break;
    case HTTP::HTTP_TYPES::CONNECT:
        sType = "CONNECT";
        break;
    case HTTP::HTTP_TYPES::PUT:
        sType = "PUT";
        break;
    case HTTP::HTTP_TYPES::HEAD:
        sType = "GET";
        break;
    default:
        sType = "UNKNOWN";
    }

    std::cout << "Type: " << sType << std::endl;
}

void WriteURL(const HTTP& httpRequest)
{
    auto httpULR = httpRequest.GetURL();
    if (!httpULR.has_value())
    {
        std::cout << "URL is empty!" << std::endl;
        return;
    }

    std::cout << "URL: " << httpULR.value() << std::endl;
}

void HttpRequest1()
{
    // HTTP request 1
    std::string sRequest1 =
        "GET /wiki/http HTTP/1.1\n"
        "Host: ru.wikipedia.org\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\n"
        "Accept: text/html\n"
        "Connection: close";

    HTTP httpRequest1;
    httpRequest1.LoadFromString(sRequest1);

    std::cout << std::endl << "=== httpRequest1 ===" << std::endl;
    WriteType(httpRequest1);
    WriteURL(httpRequest1);
    auto headerValue1_1 = httpRequest1.GetHeaderValue(HTTP::HTTP_HEADERS::Accept);
    auto headerValue1_2 = httpRequest1.GetHeaderValue("Host");
    auto headerValue1_3 = httpRequest1.GetHeaderValue("Host1");    // nullopt
}

void HttpRequest2()
{
    // HTTP request 2
    std::string sRequest2 =
        "GET /hello.htm HTTP/1.1\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
        "Host: www.example.com\n"
        "Accept-Language: ru-ru\n"
        "Accept-Encoding: gzip, deflate\n"
        "Connection: Keep-Alive\n";

    HTTP httpRequest2;
    httpRequest2.LoadFromString(sRequest2);

    std::cout << std::endl << "=== httpRequest2 ===" << std::endl;
    WriteType(httpRequest2);
    WriteURL(httpRequest2);
    auto headerValue2_1 = httpRequest2.GetHeaderValue(HTTP::HTTP_HEADERS::AcceptEncoding);
    auto headerValue2_2 = httpRequest2.GetHeaderValue("Accept-Language");
    auto headerValue2_3 = httpRequest2.GetHeaderValue("ACCEPT-Language");
}

void HttpRequest3()
{
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

    HTTP httpRequest3;
    httpRequest3.LoadFromString(sRequest3);

    std::cout << std::endl << "=== httpRequest3 ===" << std::endl;
    WriteType(httpRequest3);
    WriteURL(httpRequest3);
    auto headerValue3_1 = httpRequest3.GetHeaderValue(HTTP::HTTP_HEADERS::ContentType);
    auto headerValue3_2 = httpRequest3.GetHeaderValue("Content-Type");
}
