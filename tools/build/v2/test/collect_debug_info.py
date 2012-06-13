#!/usr/bin/python

# Copyright 2012 Jurko Gospodnetic
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

#   Temporarily enabled dummy test that always fails and is used to collect
# extra debugging information from Boost Build test runner sites.

import BoostBuild

import os
import re
import sys


###############################################################################
#
# Public interface.
#
###############################################################################

def collectDebugInfo():
    t = _init()

    dummyVars = ["WOOF_WOOFIE_%d" % x for x in xrange(4)]
    global tag

    tag = "Python version"
    try:
        _info(sys.version)
    except:
        _info_exc()

    tag = "Python platform"
    try:
        _info(sys.platform)
    except:
        _info_exc()

    tag = "XXX in os.environ"
    try:
        def f(name):
            return "%s: %s" % (name, name in os.environ)
        _infoX(f(x) for x in dummyVars)
    except:
        _info_exc()

    tag = "os.environ[XXX]"
    try:
        def f(name):
            try:
                result = os.environ[name]
            except:
                result = _str_exc()
            return "%s: %r" % (name, result)
        _infoX(f(x) for x in dummyVars)
    except:
        _info_exc()

    tag = "os.environ.get(XXX)"
    try:
        def f(name):
            return "%s: %r" % (name, os.environ.get(name))
        _infoX(f(x) for x in dummyVars)
    except:
        _info_exc()

    tag = "os.getenv(XXX)"
    try:
        def f(name):
            return "%s: %r" % (name, os.getenv(name))
        _infoX(f(x) for x in dummyVars)
    except:
        _info_exc()

    name = dummyVars[0]
    value = "foo"
    tag = "os.putenv(%s) to %r" % (name, value)
    try:
        results = []
        _collect(results, "before", name, t)
        os.putenv(name, value)
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[1]
    value = "bar"
    tag = "os.environ[%s] to %r" % (name, value)
    try:
        results = []
        _collect(results, "before", name, t)
        os.environ[name] = value
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[1]
    value = "baz"
    tag = "os.putenv(%s) to %r" % (name, value)
    try:
        results = []
        _collect(results, "before", name, t)
        os.putenv(name, value)
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[1]
    value = ""
    tag = "os.putenv(%s) to %r" % (name, value)
    try:
        results = []
        _collect(results, "before", name, t)
        os.putenv(name, value)
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[2]
    value = "foo"
    tag = "os.unsetenv(%s) from %r" % (name, value)
    try:
        results = []
        os.environ[name] = value
        _collect(results, "before", name, t)
        os.unsetenv(name)
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[2]
    value = "foo"
    tag = "del os.environ[%s] from %r" % (name, value)
    try:
        results = []
        os.environ[name] = value
        _collect(results, "before", name, t)
        del os.environ[name]
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[2]
    value = "foo"
    tag = "os.environ.pop(%s) from %r" % (name, value)
    try:
        results = []
        os.environ[name] = value
        _collect(results, "before", name, t)
        os.environ.pop(name)
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[2]
    value1 = "foo"
    value2 = ""
    tag = "os.environ[%s] to %r from %r" % (name, value2, value1)
    try:
        results = []
        os.environ[name] = value1
        _collect(results, "before", name, t)
        os.environ[name] = value2
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    name = dummyVars[3]
    value = '""'
    tag = "os.environ[%s] to %r" % (name, value)
    try:
        results = []
        _collect(results, "before", name, t)
        os.environ[name] = value
        _collect(results, "after", name, t)
        _infoX(results)
    except:
        _info_exc()

    # Avoid the 'changes caused by the last build command' report.
    if hasattr(t, 'difference'):
        del t.difference

    # Report prepared annotations.
    t.fail_test(1, dump_stdio=False, dump_stack=False)


###############################################################################
#
# Private interface.
#
###############################################################################

def _collect(results, prefix, name, t):
    results.append("%s - %s - os.getenv(): %r" % (prefix, name, os.getenv(
        name)))
    results.append("%s - %s - os.environ.get(): %r" % (prefix, name,
        os.environ.get(name)))
    external_values = _getExternalValues(t, name)
    results.append("%s - %s - external: %r" % (prefix, name,
        external_values[name]))


def _getExternalValues(t, *args):
    t.run_build_system(" ".join("--var-name=%s" % x for x in args))
    result = dict()
    for x in args:
        m = re.search(r"^\*\*\* %s: '(.*)' \*\*\*$" % x, t.stdout(),
            re.MULTILINE)
        if m:
            result[x] = m.group(1)
        else:
            result[x] = None
    return result


def _init():
    toolsetName = "__myDummyToolset__"

    t = BoostBuild.Tester("toolset=%s" % toolsetName, pass_toolset=False,
        use_test_config=False)

    #   Prepare a dummy toolset so we do not get errors in case the default one
    # is not found.
    t.write(toolsetName + ".jam", """\
import feature ;
feature.extend toolset : %s ;
rule init ( ) { }
""" % toolsetName )

    # Python version of the same dummy toolset.
    t.write(toolsetName + ".py", """\
from b2.build import feature
feature.extend('toolset', ['%s'])
def init(): pass
""" % toolsetName )

    t.write("jamroot.jam", """\
import os ;
local names = [ MATCH ^--var-name=(.*) : [ modules.peek : ARGV ] ] ;
for x in $(names)
{
    value = [ os.environ $(x) ] ;
    ECHO *** $(x): '$(value)' *** ;
}
""")

    return t


def _info(*values):
    values = list(values) + [""]
    BoostBuild.annotation(tag, "\n".join(str(x) for x in values))


def _infoX(values):
    _info(*values)


def _info_exc():
    _info(_str_exc())


def _str_exc():
    exc_type, exc_value = sys.exc_info()[0:2]
    if exc_type is None:
        exc_type_name = "None"
    else:
        exc_type_name = exc_type.__name__
    return "*** EXCEPTION *** %s - %s ***" % (exc_type_name, exc_value)


###############################################################################
#
# main()
# ------
#
###############################################################################

collectDebugInfo()
