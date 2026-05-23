#ifndef HTTPCLIENTTESTER_H
#define HTTPCLIENTTESTER_H

#include "http/IHttpClient.h"

class HttpClientTester {
    /*--@Autowired--*/
    IHttpClientPtr httpClient = Implementation<IHttpClient>::type::GetInstance();
    /*--@Autowired--*/
    ILoggerPtr logger = Implementation<ILogger>::type::GetInstance();

Public Virtual Void RunAllTests() {
        logger->Info(Tag::Untagged, "=== Running HTTP tests ===");
        TestGet("http://jsonplaceholder.typicode.com/todos/1");
        TestPost("http://jsonplaceholder.typicode.com/posts");
        TestPut("http://jsonplaceholder.typicode.com/posts/1");
        TestDelete("http://jsonplaceholder.typicode.com/posts/1");

        logger->Info(Tag::Untagged, "=== Running HTTPS tests ===");
        TestGet("https://jsonplaceholder.typicode.com/todos/1");
        TestPost("https://jsonplaceholder.typicode.com/posts");
        TestPut("https://jsonplaceholder.typicode.com/posts/1");
        TestDelete("https://jsonplaceholder.typicode.com/posts/1");
    }

Private Void TestGet(const CStdString& url) {
        StdString result = httpClient->Get(url);
        logger->Info(Tag::Untagged, "GET response (" + url + "): " + result);
        if (!result.empty()) {
            logger->Info(Tag::Untagged, "[PASS] GET returned non-empty response");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] GET returned empty response");
        }
    }

Private Void TestPost(const CStdString& url) {
        StdString body = R"({"title":"foo","body":"bar","userId":1})";
        StdString result = httpClient->Post(url, body);
        logger->Info(Tag::Untagged, "POST response (" + url + "): " + result);
        if (!result.empty()) {
            logger->Info(Tag::Untagged, "[PASS] POST returned non-empty response");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] POST returned empty response");
        }
    }

Private Void TestPut(const CStdString& url) {
        StdString body = R"({"id":1,"title":"updated","body":"baz","userId":1})";
        StdString result = httpClient->Put(url, body);
        logger->Info(Tag::Untagged, "PUT response (" + url + "): " + result);
        if (!result.empty()) {
            logger->Info(Tag::Untagged, "[PASS] PUT returned non-empty response");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] PUT returned empty response");
        }
    }

Private Void TestDelete(const CStdString& url) {
        StdString result = httpClient->Delete(url);
        logger->Info(Tag::Untagged, "DELETE response (" + url + "): " + result);
        if (result.empty() || result == "{}") {
            logger->Info(Tag::Untagged, "[PASS] DELETE returned empty or {}");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] DELETE unexpected response: " + result);
        }
    }
};

#endif // HTTPCLIENTTESTER_H
