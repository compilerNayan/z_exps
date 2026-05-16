#ifndef MKDIRTESTS_H
#define MKDIRTESTS_H

#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <dirent.h>

#include <cstdio>


void TestMkdirAndList() {
    printf("=== mkdir / listing test ===\n");

    // Try to create nested directories
    if (mkdir("/spiffs/dirA", 0777) == 0 || errno == EEXIST)
        printf("[INFO] /spiffs/dirA created or exists\n");
    else
        printf("[ERROR] mkdir /spiffs/dirA failed, errno=%d\n", errno);

    if (mkdir("/spiffs/dirA/dirB", 0777) == 0 || errno == EEXIST)
        printf("[INFO] /spiffs/dirA/dirB created or exists\n");
    else
        printf("[ERROR] mkdir /spiffs/dirA/dirB failed, errno=%d\n", errno);

    // Try to create a file in nested dir
    FILE *f = fopen("/spiffs/dirA/dirB/test.txt", "w");
    if (f) {
        fprintf(f, "hello nested\n");
        fclose(f);
        printf("[PASS] Created file in nested dir\n");
    } else {
        printf("[FAIL] Could not create file in nested dir\n");
    }

    // List contents of /spiffs
    DIR *d = opendir("/spiffs");
    if (d) {
        struct dirent *ent;
        printf("Contents of /spiffs:\n");
        while ((ent = readdir(d)) != NULL) {
            printf("  %s\n", ent->d_name);
        }
        closedir(d);
    } else {
        printf("[ERROR] Could not open /spiffs\n");
    }
}


void TestFlatVsNestedAndLength() {
    printf("=== SPIFFS filename tests ===\n");

    // Flat filename with slash in name
    FILE *f1 = fopen("/spiffs/dirX_file.txt", "w");
    if (f1) {
        fprintf(f1, "flat style\n");
        fclose(f1);
        printf("[PASS] Created flat file with slash in name\n");
    } else {
        printf("[FAIL] Could not create flat file\n");
    }

    // Nested filename (true directory style)
    FILE *f2 = fopen("/spiffs/dirX/dirY/file.txt", "w");
    if (f2) {
        fprintf(f2, "nested style\n");
        fclose(f2);
        printf("[PASS] Created nested file\n");
    } else {
        printf("[FAIL] Could not create nested file (SPIFFS does not support dirs)\n");
    }

    // Test max filename length
    char longname[128];
    memset(longname, 'a', sizeof(longname));
    longname[sizeof(longname)-1] = '\0';

    char path[160];
    snprintf(path, sizeof(path), "/spiffs/%s.txt", longname);

    FILE *f3 = fopen(path, "w");
    if (f3) {
        fprintf(f3, "long filename\n");
        fclose(f3);
        printf("[PASS] Created long filename (len=%zu)\n", strlen(path));
    } else {
        printf("[FAIL] Could not create long filename (len=%zu)\n", strlen(path));
    }
}


#endif // MKDIRTESTS_H