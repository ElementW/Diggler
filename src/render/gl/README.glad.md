# Using GLAD in Diggler

GLAD is a small, generated OpenGL loader targeting a single GL version with an optional set of supported extensions.

It is hosted on GitHub at https://github.com/Dav1dde/glad . It can be either installed, or [used from a web service](http://glad.dav1d.de/).

## Diggler requirements

Diggler mandates a minimum support of OpenGL (ES) 2.0.

On desktop OpenGL, loader support of these extensions is required:

* GL_ARB_buffer_storage
* GL_ARB_direct_state_access
* GL_ARB_framebuffer_object
* GL_ARB_shader_image_load_store
* GL_ARB_vertex_array_object
* GL_EXT_direct_state_access
* GL_KHR_debug

```
--profile="compatibility" --api="gl=2.0" --generator="c" --spec="gl" --extensions="GL_ARB_buffer_storage,GL_ARB_direct_state_access,GL_ARB_framebuffer_object,GL_ARB_shader_image_load_store,GL_ARB_vertex_array_object,GL_EXT_direct_state_access,GL_KHR_debug"
```

As said, you can [generate the loader online](http://glad.dav1d.de/#profile=compatibility&language=c&specification=gl&loader=on&api=gl%3D2.0&extensions=GL_ARB_buffer_storage&extensions=GL_ARB_direct_state_access&extensions=GL_ARB_framebuffer_object&extensions=GL_ARB_shader_image_load_store&extensions=GL_ARB_vertex_array_object&extensions=GL_EXT_direct_state_access&extensions=GL_KHR_debug).
