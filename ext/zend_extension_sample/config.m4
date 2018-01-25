PHP_ARG_ENABLE(zend_ext_sample,
  [Whether to enable the "zend_ext_sample" extension],
  [  enable-zendextsample        Enable "zend_ext_sample" extension support])

if test $PHP_ZEND_EXT_SAMPLE != "no"; then
  PHP_SUBST(ZEND_EXT_SAMPLE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(zend_ext_sample, zend_ext_sample.c, $ext_shared)
fi
