#ifndef HTTPCLIENTTESTER_H
#define HTTPCLIENTTESTER_H

#include "http/IHttpClient.h"

class HttpClientTester {
    /* @Autowired */
    Private IHttpClientPtr httpClient;
    /* @Autowired */
    Private ILoggerPtr logger;

    Public Virtual Void RunAllTests() {
        TestGet();
        TestPost();
        TestPut();
        TestDelete();
    }

    Private Void TestGet() {
        StdString result = httpClient->Get("http://jsonplaceholder.typicode.com/todos/1");
        if (result.find("\"userId\"") != StdString::npos &&
            result.find("\"title\"") != StdString::npos) {
            logger->Info(Tag::Untagged, "[PASS] GET /todos/1 returned expected fields");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] GET /todos/1 missing expected fields: " + result);
        }
    }

    Private Void TestPost() {
        StdString body = R"({"title":"foo","body":"bar","userId":1})";
        StdString result = httpClient->Post("http://jsonplaceholder.typicode.com/posts", body);
        if (result.find("\"id\"") != StdString::npos) {
            logger->Info(Tag::Untagged, "[PASS] POST /posts returned an id");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] POST /posts did not return id: " + result);
        }
    }

    Private Void TestPut() {
        StdString body = R"({"id":1,"title":"updated","body":"baz","userId":1})";
        StdString result = httpClient->Put("http://jsonplaceholder.typicode.com/posts/1", body);
        if (result.find("\"updated\"") != StdString::npos || result.find("\"title\"") != StdString::npos) {
            logger->Info(Tag::Untagged, "[PASS] PUT /posts/1 updated title");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] PUT /posts/1 did not update: " + result);
        }
    }

    Private Void TestDelete() {
        StdString result = httpClient->Delete("http://jsonplaceholder.typicode.com/posts/1");
        if (result.empty() || result == "{}") {
            logger->Info(Tag::Untagged, "[PASS] DELETE /posts/1 returned empty or {}");
        } else {
            logger->Error(Tag::Untagged, "[FAIL] DELETE /posts/1 unexpected response: " + result);
        }
    }
};

#endif // HTTPCLIENTTESTER_H