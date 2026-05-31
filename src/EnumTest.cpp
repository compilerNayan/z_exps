//#define KUEHFIEKS
#ifdef KUEHFIEKS

#include "internal/07-enrollment/01-type/01-EnrollmentStatus.h"
extern "C" void app_main(void) {
    printf("Hello\n");
    EnrollmentStatus status = EnrollmentStatus::NotStarted;
    
    auto ss = nayan::serializer::SerializationUtility::Serialize(status);
    printf("Serialsdsized: %s\n", ss.c_str());
    printf("Hello");
}

#endif