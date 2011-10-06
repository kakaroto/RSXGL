//-*-C++-*-
// RSXGL - Graphics library for the PS3 GPU.
//
// Copyright (c) 2011 Alexander Betts (alex.betts@gmail.com)
//
// framebuffer.h - Offscreen rendering targets.

#ifndef rsxgl_framebuffer_H
#define rsxgl_framebuffer_H

#include "gl_constants.h"
#include "rsxgl_limits.h"
#include "gl_object.h"
#include "smint_array.h"
#include "arena.h"
#include "textures.h"

#include <boost/mpl/list.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/deref.hpp>

union write_mask_t {
  uint8_t all;
  struct {
    uint8_t r:1, g:1, b:1, a:1, depth:1, stencil:1;
  } parts;
};

enum rsxgl_framebuffer_formats {
  RSXGL_FRAMEBUFFER_FORMAT_R5G6B5 = 0,
  RSXGL_FRAMEBUFFER_FORMAT_X8R8G8B8 = 1,
  RSXGL_FRAMEBUFFER_FORMAT_A8R8G8B8 = 2,
  RSXGL_FRAMEBUFFER_FORMAT_B8 = 3,
  RSXGL_FRAMEBUFFER_FORMAT_A16B16G16R16_FLOAT = 4,
  RSXGL_FRAMEBUFFER_FORMAT_A32B32G32R32_FLOAT = 5,
  RSXGL_FRAMEBUFFER_FORMAT_R32_FLOAT = 6,
  RSXGL_FRAMEBUFFER_FORMAT_X8B8G8R8 = 7,
  RSXGL_FRAMEBUFFER_FORMAT_A8B8G8R8 = 8,
  RSXGL_FRAMEBUFFER_FORMAT_DEPTH24_D8 = 9,
  RSXGL_FRAMEBUFFER_FORMAT_DEPTH16 = 10,
  RSXGL_MAX_FRAMEBUFFER_FORMATS = 11
};

enum rsxgl_renderbuffer_target {
  RSXGL_RENDERBUFFER = 0,
  RSXGL_MAX_RENDERBUFFER_TARGETS = 1
};

typedef boost::uint_value_t< RSXGL_MAX_RENDERBUFFER_SIZE - 1 >::least framebuffer_dimension_size_type;

struct surface_t {
  uint32_t pitch;
  memory_t memory;

  surface_t()
    : pitch(0) {
  }
};

struct renderbuffer_t {
  typedef bindable_gl_object< renderbuffer_t, RSXGL_MAX_RENDERBUFFERS, RSXGL_MAX_RENDERBUFFER_TARGETS > gl_object_type;
  typedef typename gl_object_type::name_type name_type;
  typedef typename gl_object_type::storage_type storage_type;
  typedef typename gl_object_type::binding_bitfield_type binding_bitfield_type;
  typedef typename gl_object_type::binding_type binding_type;

  static storage_type & storage();

  binding_bitfield_type binding_bitfield;

  uint32_t deleted:1, timestamp:31;
  uint32_t ref_count;

  uint8_t format, samples;
  framebuffer_dimension_size_type size[2];
  surface_t surface;
  memory_arena_t::name_type arena;

  renderbuffer_t();
  ~renderbuffer_t();
};

enum rsxgl_framebuffer_target {
  RSXGL_DRAW_FRAMEBUFFER = 0,
  RSXGL_READ_FRAMEBUFFER = 1,
  RSXGL_MAX_FRAMEBUFFER_TARGETS = 2
};

enum rsxgl_framebuffer_attachment {
  RSXGL_COLOR_ATTACHMENT0,
  RSXGL_DEPTH_STENCIL_ATTACHMENT = RSXGL_COLOR_ATTACHMENT0 + RSXGL_MAX_COLOR_ATTACHMENTS,
  RSXGL_MAX_ATTACHMENTS = RSXGL_DEPTH_STENCIL_ATTACHMENT + 1
};

enum rsxgl_framebuffer_attachment_type {
  RSXGL_ATTACHMENT_TYPE_NONE = 0,
  RSXGL_ATTACHMENT_TYPE_RENDERBUFFER = 1,
  RSXGL_ATTACHMENT_TYPE_TEXTURE = 2,
  RSXGL_MAX_ATTACHMENT_TYPES = 3
};

struct framebuffer_t {
  typedef bindable_gl_object< framebuffer_t, RSXGL_MAX_FRAMEBUFFERS, RSXGL_MAX_FRAMEBUFFER_TARGETS, 1 > gl_object_type;
  typedef typename gl_object_type::name_type name_type;
  typedef typename gl_object_type::storage_type storage_type;
  typedef typename gl_object_type::binding_bitfield_type binding_bitfield_type;
  typedef typename gl_object_type::binding_type binding_type;

  static storage_type & storage();

  binding_bitfield_type binding_bitfield;

  // Determine the largest of the name types for things that can be attached to the framebuffer (renderbuffers and textures):
  typedef boost::mpl::list< framebuffer_t::name_type, texture_t::name_type > attachment_name_types;
  typedef boost::mpl::max_element< boost::mpl::transform_view< attachment_name_types, boost::mpl::sizeof_< boost::mpl::_1 > > >::type attachment_name_types_iter;
  typedef boost::mpl::deref< attachment_name_types_iter::base >::type attachment_name_type;

  typedef smint_array< RSXGL_MAX_ATTACHMENT_TYPES - 1, RSXGL_MAX_ATTACHMENTS > attachment_types_t;
  attachment_types_t attachment_types;
  attachment_name_type attachments[RSXGL_MAX_ATTACHMENTS];

  // input is a draw buffer index, output is one of rsxgl_outbuffer above:
  smint_array< RSXGL_MAX_COLOR_ATTACHMENTS, RSXGL_MAX_DRAW_BUFFERS > mapping;

  uint8_t is_default:1, invalid:1;
  write_mask_t write_mask;

  uint16_t format, enabled;
  framebuffer_dimension_size_type size[2];
  surface_t surfaces[RSXGL_MAX_ATTACHMENTS];

  framebuffer_t();
  ~framebuffer_t();
};

struct rsxgl_context_t;

void rsxgl_renderbuffer_validate(rsxgl_context_t *,renderbuffer_t &,const uint32_t);
void rsxgl_framebuffer_validate(rsxgl_context_t *,framebuffer_t &,const uint32_t);
void rsxgl_draw_framebuffer_validate(rsxgl_context_t *,const uint32_t);

#endif
