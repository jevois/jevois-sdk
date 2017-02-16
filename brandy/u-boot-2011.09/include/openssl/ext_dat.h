#include"conf.h"


extern X509V3_EXT_METHOD v3_ns_ia5_list[];


static X509V3_EXT_METHOD * standard_exts[] = {
  &v3_ns_ia5_list[0],
  &v3_ns_ia5_list[1],
  &v3_ns_ia5_list[2],
  &v3_ns_ia5_list[3],
  &v3_ns_ia5_list[4],
  &v3_ns_ia5_list[5],
  &v3_ns_ia5_list[6],
  &v3_ns_ia5_list[7],
  &v3_ns_ia5_list[8],
  
};

/* Number of standard extensions */

#define STANDARD_EXTENSION_COUNT (sizeof(standard_exts)/sizeof(X509V3_EXT_METHOD *))

