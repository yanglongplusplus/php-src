#ifndef PHP_HSF_TOOL_H
/* Prevent double inclusion */
#define PHP_HSF_TOOL_H

/* Define Extension Properties */
#define PHP_HSF_TOOL_EXTNAME   "hsf_tool"
#define PHP_HSF_TOOL_EXTVER    "1.0"

/* Import configure options when building outside of the PHP source tree */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Include PHP Standard Header */
#include "php.h"

/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry hsf_hsf_tool_module_entry;
#define phpext_hsf_tool_ptr &hsf_tool_module_entry

PHP_MINIT_FUNCTION(hsf_tool);
PHP_MSHUTDOWN_FUNCTION(hsf_tool);
PHP_RINIT_FUNCTION(hsf_tool);
PHP_RSHUTDOWN_FUNCTION(hsf_tool);
PHP_MINFO_FUNCTION(hsf_tool);

PHP_FUNCTION(get_curl_handle);

#endif /* PHP_HSF_TOOL_H */

