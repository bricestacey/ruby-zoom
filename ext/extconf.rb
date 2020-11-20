require 'mkmf'
require 'pkg-config'

unless PKGConfig.have_package('yaz')
  $stderr.puts 'yaz development files do not appear to be installed'
  exit(false)
end

unless have_header('yaz/zoom.h')
  $stderr.puts 'yaz zoom header not available'
  exit
end

$CFLAGS << " #{PKGConfig.cflags('yaz')} "
$LDFLAGS << " #{PKGConfig.libs('yaz')} "

create_makefile("zoom")

