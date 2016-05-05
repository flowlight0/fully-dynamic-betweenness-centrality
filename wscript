#! /usr/bin/env python3

APPNAME= 'fully_dynamic_betweenness_centrality'
VERSION= '0.0.1'

top = '.'
out = 'bin'

def options(opt):
    opt.tool_options('compiler_cxx')
    opt.tool_options('waf_unit_test')
    
def configure(conf):
    conf.load('compiler_cxx')
    conf.check_tool('compiler_cxx')
    conf.env.append_value('CXXFLAGS', ['-Wall', '-Wextra', '-g', '-O3', '-std=c++11', '-Wno-unused-local-typedefs'])
    conf.check_cxx(lib = ['pthread'], uselib_store = 'common')
    
def build(bld):
    bld.recurse('lib')
    bld.recurse('src')
    
