// Copyright (c) 2017 Baidu, Inc.
// Author: Zhangyi Chen (chenzhangyi01@baidu.com)

#ifndef BUTIL_TEST_SSTREAM_WORKAROUND
#define BUTIL_TEST_SSTREAM_WORKAROUND

// Test binaries compile with -Dprivate=public -Dprotected=public so that
// tests can reach into class internals (white-box testing).  This breaks
// certain standard library headers where a member is forward-declared under
// the class's *implicit* private access (which the macro cannot affect)
// and then defined under an explicit `private:` (which the macro turns
// into `public:`), causing "redeclared with different access" errors.
//
// Known affected headers:
//   <sstream>  — GCC 5.x: std::__cxx11::basic_stringbuf::__xfer_bufptrs
//   <any>      — GCC 13:  std::any::_Manager_internal, _Manager_external
//
// The fix: undef the macro, include the problematic header (so it gets
// parsed with correct access specifiers), then re-define the macro.
// The -include flag ensures this header is processed before any user
// #include directives.

#ifdef private
# undef private
# undef protected
# include <sstream>
# if __cplusplus >= 201703L
#  include <any>
# endif
# define private public
# define protected public
#else
# include <sstream>
# if __cplusplus >= 201703L
#  include <any>
# endif
#endif

#endif  //  BUTIL_TEST_SSTREAM_WORKAROUND
