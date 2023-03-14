# gltrim thoughts and design notes

## Scope

The program targets at trimming a trace to contain the minimum number
of calls needed to create the image rendered by the target frame.

## Design notes

The programs scans through the trace and collects calls of the various
GL objects and the GL state. At the start of the target frame the
current state is recorded to the output, and all the calls recorded in
the objects used in the callaset corresponding to the frame.

The calls are collected in a std::set so that duplicate calls are
eliminated, and then the set is sorted based on call number and written.

The following types of calls need to be considered:

* State calls: These need to be updated whenever they occure, but only the
  last call before the target framess or setup frames are copied needs to
  be remembered. Set --keep-all-states to override this behaviour and retain
  all state calls.

* Programs: The object must be tracked and calls related to attached
  shaders, uniforms, and attribute arrays

* Textures and buffers: creation and use must be tracked
  - Texture data may also be created by drawing to a fbo
  - Buffer data may be changed by compute shaders

* Frame buffer objects:
  The draw buffer must keep all the state information just like the target
  frame: An attached texture can be used as data source in the target frame
  and an attached renderbuffer can be the source of a blit. In both cases,
  the calls to create the output need to be recorded.
  - Since an FBO needs to keep track of its attachements, and a
    texture/renderbuffer needs to keep track of its data source(s)  a circular
    reference is created, this needs to be handled when writing the calls.

## Currently known problems

* in CIV5 the terrain tiles are not retained and some icons are not drawn
  correctly, this is probably a texture attachement problem. One can work
  around this by selecting a series of frames as setup frames. These will
  all be put into the one setup frame.

* Trimming Alien Isolation doesn't retain all state changes as required.
  Keeping all state changes may help here.

* Games like SOMA, that combine rendered results from various frames to create
  a motion blur effect need to keep a sufficient number of frames to re-create
  that effect. It may not be possible to re-create the frame correctly.

## Notes for future optimization

* buffer calls could be optimized so that buffer calls that upload data
  that is overwritten before it is used in the target frame are dropped

* useless binding and texture unit calls could be dropped

