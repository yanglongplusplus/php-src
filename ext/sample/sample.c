// sample.c

#include "php_sample.h"

PHP_FUNCTION(sayhi)
{
    php_printf("%s\n", "Hello World !");
}

PHP_FUNCTION(xxoo)
{
    php_printf("%s\n", "XXOO !");
}

PHP_FUNCTION(sample_count)
{
    zval *arr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr)
            == FAILURE) {
        RETURN_NULL();
    }
    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(arr)));
}

PHP_FUNCTION(make_array)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &n)
            == FAILURE) {
        RETURN_NULL();
    }
    zval zv;
    array_init(&zv);
    zval tmp;
    int i = 0;
    char *k;
    char *v;
    for (; i < n; i++) {
        spprintf(&k, 0, "K%d", i);
        spprintf(&v, 0, "V%d", i);
        ZVAL_STRING(&tmp, v);
        zend_hash_str_update(Z_ARRVAL_P(&zv), k, strlen(k), &tmp);
        efree(k);
        efree(v);
    }

    RETURN_ARR(Z_ARRVAL_P(&zv));
}

PHP_FUNCTION(count_return_by_ref)
{
    zval *arr = NULL;
    zval *v = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az", &arr, &v)
            == FAILURE) {
        RETURN_NULL();
    }
    v->value.ref->val.value.lval = zend_hash_num_elements(Z_ARRVAL_P(arr));
    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(arr)));
}

ZEND_BEGIN_ARG_INFO(php_count_return_by_ref_arginfo, 0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()

static zend_function_entry php_sample_functions[] = {
    PHP_FE(sayhi, NULL)
    PHP_FE(xxoo, NULL)
    PHP_FE(make_array, NULL)
    PHP_FE(sample_count, NULL)
    PHP_FE(count_return_by_ref, php_count_return_by_ref_arginfo)
    { NULL, NULL, NULL }
};

zend_module_entry sample_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_SAMPLE_EXTNAME,
    php_sample_functions, /* Functions */
    NULL, /* MINIT */
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

