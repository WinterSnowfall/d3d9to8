d3d9_res = wrc_generator.process('version.rc')

d3d9_src = [
  'd3d9_buffer.cpp',
  'd3d9_main.cpp',
  'd3d9_logger.cpp',
  'd3d9_device.cpp',
  'd3d9_interface.cpp',
  'd3d9_surface.cpp',
  'd3d9_swapchain.cpp',
  'd3d9_texture.cpp',
  'd3d9_shader.cpp',
  'd3d9_stateblock.cpp',
  'd3d9_query.cpp',
  'd3d9_vertex_declaration.cpp',
  'd3d9_shader_validator.cpp',
]

d3d9_ld_args      = []
d3d9_link_depends = []

d3d9_dll = shared_library('d3d9', d3d9_src, d3d9_res,
  dependencies        : [ ],
  include_directories : [ ],
  install             : true,
  vs_module_defs      : 'd3d9'+def_spec_ext,
  link_args           : d3d9_ld_args,
  link_depends        : [ d3d9_link_depends ],
  kwargs              : {'name_prefix': ''},
)

d3d9_dep = declare_dependency(
  link_with           : [ d3d9_dll ],
  include_directories : [ ],
)

