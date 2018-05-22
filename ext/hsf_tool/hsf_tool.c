#include "php_hsf_tool.h"

static zval curl_handle;
static int is_inited = 0;

static int le_sample_descriptor;
static int le_sample_descriptor_persist;

static void hsf_tool_log();
#define PHP_SAMPLE_DESCRIPTOR_RES_NAME "File Descriptor"

static void php_sample_descriptor_dtor(zend_resource *rsrc)
{
    fclose((FILE *)rsrc->ptr);
}

static void php_sample_descriptor_dtor_persistent(zend_resource *rsrc)
{
    fclose((FILE *)rsrc->ptr);
}

struct data_t {
    int id;
    char *name;
};

struct data_t data;

PHP_MINIT_FUNCTION(hsf_tool)
{
    data.id = 0;
    data.name = NULL;

    hsf_tool_log("minit");
    le_sample_descriptor = zend_register_list_destructors_ex(
            php_sample_descriptor_dtor,
            NULL,
            PHP_SAMPLE_DESCRIPTOR_RES_NAME,
            module_number);

    le_sample_descriptor_persist = zend_register_list_destructors_ex(
            NULL,
            php_sample_descriptor_dtor_persistent,
            PHP_SAMPLE_DESCRIPTOR_RES_NAME,
            module_number);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hsf_tool)
{
    close_curl_handle();
    zval_ptr_dtor(&curl_handle);
    ZVAL_UNDEF(&curl_handle);
    return SUCCESS;
}

PHP_RINIT_FUNCTION(hsf_tool)
{
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(hsf_tool)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(hsf_tool)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "hsf_tool", PHP_HSF_TOOL_EXTVER);
    php_info_print_table_end();
}

PHP_FUNCTION(global_var_addr_test)
{
    if (data.id == 0) {
        pid_t pid = getpid();
        char *str = "n";
        data.id = (int)pid;
        data.name = str;
        php_write("init<br/>", strlen("init<br/>"));
    }

    char *addr = malloc(100);
    int n = sprintf(addr, "%d %p", data.id, (void*)&data);
    addr[n] = '\0';
    php_write(addr, strlen(addr));
    free(addr);
}

PHP_FUNCTION(get_curl_handle)
{
    if (is_inited == 0) {
        int result = init_curl_handle();
        if (result == SUCCESS) {
            is_inited = 1;
        }
    }
    *return_value = curl_handle;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(sample_fopen)
{
    FILE *fp;
    char *filename, *mode, *hash_key;;
    size_t filename_len, mode_len, hash_key_len;
    zend_resource *p_zr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filename, &filename_len, &mode, &mode_len) == FAILURE) {
        RETURN_NULL();
    }
    /* Try to find an already opened file */
    hash_key_len = spprintf(&hash_key, 0, "sample_descriptor:%s:%s", filename, mode);
    if ((p_zr = zend_hash_str_find_ptr(&EG(persistent_list), hash_key, hash_key_len)) != NULL) {
        hsf_tool_log("hit");
        /* There's already a file open, return that! */
        ZVAL_RES(return_value, zend_register_resource(p_zr->ptr, le_sample_descriptor_persist));
        efree(hash_key);
        return;
    }

    hsf_tool_log("miss");

    if (!filename_len || !mode_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING,
                "Invalid filename or mode length");
        RETURN_FALSE;
    }
    fp = fopen(filename, mode);
    if (!fp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING,
                "Unable to open %s using mode %s",
                filename, mode);
        RETURN_FALSE;
    }
    zend_resource *zr = emalloc(sizeof(zend_resource));
    zr->type = le_sample_descriptor_persist;
    zr->ptr = fp;

    zval zv;
    ZVAL_NEW_PERSISTENT_RES(&zv, -1, fp, le_sample_descriptor_persist);
    zend_hash_str_update(&EG(persistent_list), hash_key, hash_key_len, &zv);

    ZVAL_RES(return_value, zend_register_resource(fp, le_sample_descriptor_persist));
    efree(hash_key);
}

PHP_FUNCTION(sample_fwrite)
{
    FILE *fp;
    zval *file_resource;
    char *data;
    size_t data_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &file_resource, &data, &data_len) == FAILURE ) {
        RETURN_NULL();
    }
    /* Use the zval* to verify the resource type and
     * retrieve its pointer from the lookup table */
    fp = (FILE*)zend_fetch_resource(Z_RES_P(file_resource), PHP_SAMPLE_DESCRIPTOR_RES_NAME, le_sample_descriptor_persist);

    /* Write the data, and
     * return the number of bytes which were
     * successfully written to the file */
    RETURN_LONG(fwrite(data, 1, data_len, fp));
}

PHP_FUNCTION(sample_fclose)
{
    zval *file_resource;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &file_resource) == FAILURE) {
        RETURN_NULL();
    }

    /* Force the resource into self-destruct mode */
    zend_list_delete(Z_RES_P(file_resource));
    RETURN_TRUE;
}

static zend_function_entry php_hsf_tool_functions[] = {
        PHP_FE(get_curl_handle, NULL)
        PHP_FE(sample_fopen, NULL)
        PHP_FE(sample_fwrite, NULL)
        PHP_FE(sample_fclose, NULL)
        PHP_FE(global_var_addr_test, NULL)
        { NULL, NULL, NULL }
};

zend_module_entry hsf_tool_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_HSF_TOOL_EXTNAME,
    php_hsf_tool_functions, /* Functions */
    PHP_MINIT(hsf_tool), /* MINIT */
    PHP_MSHUTDOWN(hsf_tool), /* MSHUTDOWN */
    PHP_RINIT(hsf_tool), /* RINIT */
    PHP_RSHUTDOWN(hsf_tool), /* RSHUTDOWN */
    PHP_MINFO(hsf_tool), /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_HSF_TOOL_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HSF_TOOL
ZEND_GET_MODULE(hsf_tool)
#endif

int init_curl_handle() {
    zval function_name;
    ZVAL_STRING(&function_name, "curl_init");
    if (call_user_function_ex(CG(function_table), NULL, &function_name, &curl_handle, 0, NULL, 0, NULL TSRMLS_CC) != SUCCESS) {
        zend_error(E_ERROR, "call curl_init failed!");
        return FAILURE;
    }
    zval_ptr_dtor(&function_name);
    ZVAL_UNDEF(&function_name);
    return SUCCESS;
}

int close_curl_handle() {
    zval params[1];
    params[0] = curl_handle;
    zval function_name;
    ZVAL_STRING(&function_name, "curl_close");
    if (call_user_function_ex(CG(function_table), NULL, &function_name, &curl_handle, 1, (zval*)&params, 0, NULL TSRMLS_CC) != SUCCESS) {
        zend_error(E_ERROR, "call curl_close failed!");
        return FAILURE;
    }
    zval_ptr_dtor(&function_name);
    ZVAL_UNDEF(&function_name);
    return SUCCESS;
}

void hsf_tool_log(char *data) {
    FILE *fp;
    char *str = (char*)malloc(10 + strlen(data));
    pid_t pid = getpid();
    int n = sprintf(str, "%d %s", pid, data);
    str[n] = '\n';
    str[n+1] = '\0';
    fp = fopen("/data/php.log", "a");
    fwrite(str, 1, strlen(str), fp);
    fclose(fp);
    free(str);
}