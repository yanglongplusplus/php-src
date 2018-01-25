//
// Created by YangLong on 12/01/2018.
//

#include "php.h"
#include "Zend/zend_extensions.h"
#include "Zend/zend_smart_str.h"
#include "zend_ext_sample.h"

#define PRINT(what) fprintf(stderr, what "\n");

static PHP_MINIT_FUNCTION(pib)
{
    PRINT("PHP extension is starting up");

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(pib)
{
    PRINT("PHP extension is shutting down");

    return SUCCESS;
}

static PHP_RINIT_FUNCTION(pib)
{
    PRINT("PHP extension new request starting up");

    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(pib)
{
    PRINT("PHP extension current request is shutting down");

    return SUCCESS;
}

/* This line should stay commented
ZEND_GET_MODULE(pib)
*/

/* Declared as static, thus private */
static zend_module_entry pib_module_entry = {
        STANDARD_MODULE_HEADER,
        "pib",
        NULL, /* Function entries */
        PHP_MINIT(pib), /* Module init */
        PHP_MSHUTDOWN(pib), /* Module shutdown */
        PHP_RINIT(pib), /* Request init */
        PHP_RSHUTDOWN(pib), /* Request shutdown */
        NULL, /* Module information */
        "0.1", /* Replace with version number for your extension */
        STANDARD_MODULE_PROPERTIES
};

static void pib_zend_extension_activate(void) {
    CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
    PRINT("Zend extension new request starting up");
}

static void pib_zend_extension_deactivate(void) {
    CG(compiler_options) &= ~ZEND_COMPILE_EXTENDED_INFO;
    PRINT("Zend extension current request is shutting down");
}

static void pib_zend_extension_message_handler(int code, void *ext) {
    php_printf("We just detected that zend_extension '%s' is trying to load\n", ((zend_extension *)ext)->name);
}

static void pib_zend_extension_op_array_handler(zend_op_array *op_array) {
    smart_str out = {0};

    smart_str_appends(&out, "We just compiled ");

    if (op_array->function_name) {
        uint32_t i, num_args = op_array->num_args;

        if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
            smart_str_appends(&out, "a closure ");
        } else {
            smart_str_appends(&out, "function ");
            smart_str_append(&out, op_array->function_name);
        }
        smart_str_appendc(&out, '(');

        /* The variadic arg is not declared as an arg internally */
        if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
            num_args++;
        }
        for (i=0; i<num_args; i++) {
            zend_arg_info arg = op_array->arg_info[i];

            if (arg.pass_by_reference) {
                smart_str_appendc(&out, '&');
            }
            if (arg.is_variadic) {
                smart_str_appends(&out, "...");
            }
            smart_str_appendc(&out, '$');
            smart_str_append(&out, arg.name);
            if (i != num_args - 1) {
                smart_str_appends(&out, ", ");
            }
        }

        smart_str_appends(&out, ") in file ");
        smart_str_append(&out, op_array->filename);
        smart_str_appends(&out, " between line ");
        smart_str_append_unsigned(&out, op_array->line_start);
        smart_str_appends(&out, " and line ");
        smart_str_append_unsigned(&out, op_array->line_end);
    } else {
        smart_str_appends(&out, "the file ");
        smart_str_append(&out, op_array->filename);
    }

    smart_str_0(&out);
    php_printf("%s\n", ZSTR_VAL(out.s));
    smart_str_free(&out);
}

static void pib_zend_extension_fcall_begin_handler(zend_execute_data *execute_data) {
    if (!execute_data->call) {
        /* Fetch the next OPline. We use pointer arithmetic for that */
        zend_op n = execute_data->func->op_array.opcodes[(execute_data->opline - execute_data->func->op_array.opcodes) + 1];
        if (n.extended_value == ZEND_EVAL) {
            php_printf("Begining of a code eval() in %s:%u", ZSTR_VAL(execute_data->func->op_array.filename), n.lineno);
        } else {
            /* The file to be include()ed is stored into the operand 1 of the OPLine */
            zend_string *file = zval_get_string(EX_CONSTANT(n.op1));
            php_printf("Begining of an include of file '%s'", ZSTR_VAL(file));
            zend_string_release(file);
        }
    } else if (execute_data->call->func->common.fn_flags & ZEND_ACC_STATIC) {
        php_printf("Begining of a new static method call : '%s::%s'",
                   ZSTR_VAL(Z_CE(execute_data->call->This)->name),
                   ZSTR_VAL(execute_data->call->func->common.function_name));
    } else if (Z_TYPE(execute_data->call->This) == IS_OBJECT) {
        php_printf("Begining of a new method call : %s->%s",
                   ZSTR_VAL(Z_OBJCE(execute_data->call->This)->name),
                   ZSTR_VAL(execute_data->call->func->common.function_name));
    } else {
        php_printf("Begining of a new function call : %s", ZSTR_VAL(execute_data->call->func->common.function_name));
    }
    PHPWRITE("\n", 1);
}

static int pib_zend_extension_startup(zend_extension *ext)
{
    PRINT("Zend extension is starting up");

    /* When the Zend extension part will startup(), make it register
       a PHP extension by calling ourselves zend_startup_module() */
    return zend_startup_module(&pib_module_entry);
}

static void pib_zend_extension_shutdown(zend_extension *ext)
{
    PRINT("Zend extension is shutting down");
}

/* Remember that we must declare such a symbol in a Zend extension. It is used to check
 * if it was built against the same API as the one PHP runtime uses */
zend_extension_version_info extension_version_info = {
        ZEND_EXTENSION_API_NO,
        ZEND_EXTENSION_BUILD_ID
};

zend_extension zend_extension_entry = {
        "zend-extension-sample",
        "1.0",
        "Long Yang",
        "Home Page",
        "Our Copyright",
        pib_zend_extension_startup,               /* startup() : module startup */
        pib_zend_extension_shutdown,              /* shutdown() : module shutdown */
        pib_zend_extension_activate,              /* activate() : request startup */
        pib_zend_extension_deactivate,            /* deactivate() : request shutdown */
        pib_zend_extension_message_handler,       /* message_handler() */
        pib_zend_extension_op_array_handler,      /* compiler op_array_handler() */
        NULL,                                     /* VM statement_handler() */
        pib_zend_extension_fcall_begin_handler,   /* VM fcall_begin_handler() */
        NULL,                                     /* VM fcall_end_handler() */
        NULL,                                     /* compiler op_array_ctor() */
        NULL,                                     /* compiler op_array_dtor() */
        STANDARD_ZEND_EXTENSION_PROPERTIES        /* Structure-ending macro */
};
