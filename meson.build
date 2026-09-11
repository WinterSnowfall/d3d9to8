project('d3d9to8', ['c', 'cpp'], version : '0.1', meson_version : '>= 0.58', default_options : [ 'cpp_std=c++17', 'b_vscrt=static_from_buildtype', 'warning_level=2' ])

pkg = import('pkgconfig')
cpu_family = target_machine.cpu_family()
platform   = target_machine.system()

cpp = meson.get_compiler('cpp')
cc = meson.get_compiler('c')
compiler_is_msvc = cpp.get_argument_syntax() == 'msvc'

compiler_args = [
  '-msse',
  '-msse2',
  '-msse3',
  '-mfpmath=sse',
  '-Wimplicit-fallthrough',
  # gcc
  '-Wno-missing-field-initializers',
  '-Wno-unused-parameter',
  '-Wno-misleading-indentation',
  '-Wno-cast-function-type', # Needed for GetProcAddress.
  # clang
  '-Wno-unused-private-field',
  '-Wno-microsoft-exception-spec',
  '-Wno-extern-c-compat',
  '-Wno-unused-const-variable',
  '-Wno-missing-braces',
]

link_args = [ ]

compiler_args += [
  '-DNOMINMAX',
  '-D_WIN32_WINNT=0x0501',
]

if not compiler_is_msvc
  link_args += [
    '-static',
    '-static-libgcc',
    '-static-libstdc++',
    # We need to set the section alignment for debug symbols to
    # work properly as well as avoiding a memcpy from the Wine loader.
    '-Wl,--file-alignment=4096',
  ]

  # Wine's built-in back traces only work with dwarf4 symbols
  if get_option('debug')
    compiler_args += [
      '-gdwarf-4',
    ]
  endif

  if cpu_family == 'x86'
    # Enable stdcall fixup on 32-bit
    link_args += [
      '-Wl,--enable-stdcall-fixup',
      '-Wl,--kill-at',
    ]
    # Fix stack alignment issues with mingw on 32-bit
    compiler_args += [
      '-mpreferred-stack-boundary=2'
    ]
  endif
else
  # setup file alignment + enable PDB output for MSVC builds
  compiler_args += [
    '/Z7'
  ]
  link_args += [
    '/FILEALIGN:4096',
    '/DEBUG:FULL'
  ]
endif

lib_d3d9 = cpp.find_library('d3d9')

if compiler_is_msvc
  res_ext = '.res'
  wrc = find_program('rc')
  wrc_generator = generator(wrc,
    output    : [ '@BASENAME@' + res_ext ],
    arguments : [ '/fo', '@OUTPUT@', '@INPUT@' ],
  )
else
  res_ext = '.o'
  wrc = find_program('windres')
  wrc_generator = generator(wrc,
    output    : [ '@BASENAME@' + res_ext ],
    arguments : [ '-i', '@INPUT@', '-o', '@OUTPUT@' ],
  )
endif

add_project_arguments(cpp.get_supported_arguments(compiler_args), language: 'cpp')
add_project_arguments(cc.get_supported_arguments(compiler_args), language: 'c')
add_project_link_arguments(cpp.get_supported_link_arguments(link_args), language: 'cpp')
add_project_link_arguments(cc.get_supported_link_arguments(link_args), language: 'c')

exe_ext = ''
def_spec_ext = '.def'

conf_data = configuration_data()
conf_data.set('BUILD_COMPILER', cpp.get_id())
conf_data.set('BUILD_COMPILER_VERSION', cpp.version())
conf_data.set('BUILD_TARGET', cpu_family)

subdir('src')
