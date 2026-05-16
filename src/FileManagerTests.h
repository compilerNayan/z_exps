#ifndef FILEMANAGERTESTS_H
#define FILEMANAGERTESTS_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include "esp_log.h"
#include "io/IFileManager.h"

// Generate random "dir path" string (treated as flat filename in SPIFFS)
static std::string RandomDirPath() {
    int levels = 1 + (std::rand() % 4);
    std::string path;
    for (int i = 0; i < levels; ++i) {
        int r = std::rand() % 10000;
        path += "dir" + std::to_string(r);
        if (i < levels - 1) path += "/";
    }
    return path;
}

void RunFileManagerTests() {
    /*--@Autowired--*/
    IFileManagerPtr fileManager = Implementation<IFileManager>::type::GetInstance();

    std::srand(std::time(nullptr));

    printf("=== FileManager Tests Start ===\n");

    // Basic flat file tests
    CStdString file1 = "test1.txt";

    if (fileManager->Create(file1, "Hello World\n"))
        printf("[PASS] Created file1\n");
    else
        printf("[FAIL] Could not create file1\n");

    auto contents1 = fileManager->Read(file1);
    printf(contents1.empty() ? "[FAIL] Read file1 empty\n" : "[PASS] Read file1: %s\n", contents1.c_str());

    if (fileManager->Append(file1, "Appended line\n")) {
        auto c = fileManager->Read(file1);
        printf("[PASS] After append file1: %s\n", c.c_str());
    }

    if (fileManager->Update(file1, "Overwritten\n")) {
        auto c = fileManager->Read(file1);
        printf("[PASS] After update file1: %s\n", c.c_str());
    }

    if (fileManager->Delete(file1)) {
        auto c = fileManager->Read(file1);
        printf(c.empty() ? "[PASS] file1 deleted\n" : "[FAIL] file1 still readable\n");
    }

    // "Dir path" tests (treated as flat filenames)
    printf("=== Dir Path Tests (flat filenames) ===\n");
    for (int i = 0; i < 3; ++i) {
        std::string randomDir = RandomDirPath();
        CStdString nestedFile1 = randomDir + "/nested1.txt";
        CStdString nestedFile2 = randomDir + "/nested2.txt";

        printf("[INFO] Testing path: %s\n", randomDir.c_str());

        if (fileManager->Create(nestedFile1, "Nested file1 contents\n"))
            printf("[PASS] Created nestedFile1\n");

        auto nc1 = fileManager->Read(nestedFile1);
        printf(nc1.empty() ? "[FAIL] Read nestedFile1 empty\n" : "[PASS] Read nestedFile1: %s\n", nc1.c_str());

        if (fileManager->Append(nestedFile1, "Appended nested line\n")) {
            auto nc1a = fileManager->Read(nestedFile1);
            printf("[PASS] After append nestedFile1: %s\n", nc1a.c_str());
        }

        if (fileManager->Create(nestedFile2, "Nested file2 contents\n"))
            printf("[PASS] Created nestedFile2\n");

        if (fileManager->Delete(nestedFile1)) {
            auto check = fileManager->Read(nestedFile1);
            printf(check.empty() ? "[PASS] nestedFile1 deleted\n" : "[FAIL] nestedFile1 still readable\n");
        }

        auto nc2 = fileManager->Read(nestedFile2);
        printf(nc2.empty() ? "[FAIL] nestedFile2 missing\n" : "[PASS] nestedFile2 exists: %s\n", nc2.c_str());
    }

    // Max filename length test
    printf("=== Max Filename Length Test ===\n");
    std::string longName(40, 'a'); // 40 chars, exceeds default SPIFFS limit (32)
    CStdString longFile = longName + ".txt";

    if (fileManager->Create(longFile, "Long filename test\n"))
        printf("[PASS] Created long filename (len=%zu)\n", longFile.size());
    else
        printf("[FAIL] Could not create long filename (len=%zu)\n", longFile.size());

    printf("=== FileManager Tests End ===\n");
}

#endif // FILEMANAGERTESTS_H
