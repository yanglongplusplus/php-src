//
// Created by YangLong on 12/01/2018.
//

#ifndef PHP_SAMPLE_H
/* Prevent double inclusion */
#define PHP_SAMPLE_H

/* Define Extension Properties */
#define PHP_SAMPLE_EXTNAME    "zend_ext_sample"
#define PHP_SAMPLE_EXTVER    "1.0"

/* Import configure options
   when building outside of
   the PHP source tree */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Include PHP Standard Header */
#include "php.h"

/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry zend_ext_sample_module_entry;
#define phpext_zend_ext_sample_module_entry_ptr &zend_ext_sample_module_entry

#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && \
                          PHP_MINOR_VERSION > 0)
    ZEND_BEGIN_ARG_INFO_EX(php_zend_ext_sample_retref_arginfo, 0, 1, 0)
    ZEND_END_ARG_INFO ()
#endif /* PHP >= 5.1.0 */

#endif /* PHP_SAMPLE_H */
