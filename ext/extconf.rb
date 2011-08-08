require 'mkmf'

unless system('yaz-config')
  $stderr.puts 'yaz does not appear to be installed'
  exit
end

unless have_header('yaz/zoom.h')
  $stderr.puts 'yaz zoom header not available'
  exit
end

$CFLAGS << " #{`yaz-config --cflags`} "
$LDFLAGS << " #{`yaz-config --libs`} "

create_makefile("zoom")

