PHP_ARG_ENABLE(hsf_tool,
  [Whether to enable the "hsf_tool" extension],
  [  enable-hsf_tool        Enable "hsf_tool" ext])

if test $PHP_HSF_TOOL != "no"; then
  PHP_SUBST(HSF_TOOL_SHARED_LIBADD)
  PHP_NEW_EXTENSION(hsf_tool, hsf_tool.c, $ext_shared)
fi

