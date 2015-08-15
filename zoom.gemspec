RUBY_ZOOM_VERSION = '0.5.0'

Gem::Specification.new do |s|
  s.name = 'zoom'
  s.version = RUBY_ZOOM_VERSION
  s.homepage = 'http://ruby-zoom.rubyforge.org'
  s.platform = Gem::Platform::RUBY
  s.author = [ "Laurent Sansonetti", "Ed Summers" ]
  s.summary = 'Ruby/ZOOM provides a Ruby binding to the Z39.50 Object-Orientation Model (ZOOM), an abstract object-oriented programming interface to a subset of the services specified by the Z39.50 standard, also known as the international standard ISO 23950.  This version introduces ZOOM Extended Services.'
  s.autorequire = 'zoom'
  s.files = 
    Dir.glob('ext/*.c') +
    Dir.glob('ext/*.h') +
    Dir.glob('ext/*.rb') +
    Dir.glob('test/**/*') + 
    Dir.glob('sample/**/*') +
    ['README.md', 'ChangeLog', 'Rakefile']
  s.extensions = 'ext/extconf.rb'
end
