# frozen_string_literal: true

require "mkmf"
require "pkg-config"

unless PKGConfig.have_package("yaz")
  warn "yaz development files do not appear to be installed"
  exit(false)
end

unless have_header("yaz/zoom.h")
  warn "yaz zoom header not available"
  exit
end

append_cflags PKGConfig.cflags("yaz")
append_ldflags PKGConfig.libs("yaz")

create_makefile("zoom")
