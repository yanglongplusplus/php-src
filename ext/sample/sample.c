//
// Created by YangLong on 12/01/2018.
//

#include "php_sample.h"

PHP_INI_BEGIN()
    PHP_INI_ENTRY("sample.greeting", "Hello World", PHP_INI_ALL, NULL)
PHP_INI_END()

PHP_MINIT_FUNCTION(sample)
        {
                REGISTER_INI_ENTRIES();
                return SUCCESS;
        }

PHP_FUNCTION(sayhi)
        {
                const char *greeting = INI_STR("sample.greeting");
                php_printf("%s\n", greeting);
        }

PHP_FUNCTION(sample_count_array)
        {
                zval *arr;
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
            RETURN_NULL();
        }
        RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(arr)));
        }

ZEND_BEGIN_ARG_INFO(php_sample_array_arginfo, 0)
ZEND_ARG_ARRAY_INFO(0, "arr", 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_sample_functions[] = {
        PHP_FE(sample_count_array, php_sample_array_arginfo)
        PHP_FE(sayhi, NULL)
        { NULL, NULL, NULL }
};

zend_module_entry sample_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
        STANDARD_MODULE_HEADER,
#endif
        PHP_SAMPLE_EXTNAME,
        php_sample_functions, /* Functions */
        PHP_MINIT(sample), /* MINIT */
        NULL, /* MSHUTDOWN */
        NULL, /* RINIT */
        NULL, /* RSHUTDOWN */
        NULL, /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
        PHP_SAMPLE_EXTVER,
#endif
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SAMPLE
ZEND_GET_MODULE(sample)
#endif