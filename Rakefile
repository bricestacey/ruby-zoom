RUBY_ZOOM_VERSION = '0.4.1'

require 'rubygems'
require 'rake'
require 'rake/clean'
require 'rake/testtask'
require 'rake/rdoctask'
require 'rake/packagetask'
require 'rake/gempackagetask'
require 'mkmf'

CLEAN.include '**/*.o'
CLEAN.include '**/*.so'
CLEAN.include '**/Makefile'
CLEAN.include '**/*.log'
CLEAN.include 'pkg'

task :default => [:build, :test, :package] do |t|
end

task :build => [:clean] do |t|
  Dir.chdir('ext') do 
    system('ruby', 'extconf.rb')
    system('make')
  end
end

Rake::TestTask.new('test') do |t|
  t.test_files = FileList['test/*_test.rb']
  t.ruby_opts = ['-r test/unit', '-I ext', '-r zoom']
  t.verbose = true
end

Rake::TestTask.new('live_test') do |t|
  t.test_files = FileList['test/*_live.rb']
  t.ruby_opts = ['-r test/unit', '-I ext', '-r zoom']
  t.verbose = true
end

spec = Gem::Specification.new do |s|
  s.name = 'zoom'
  s.version = RUBY_ZOOM_VERSION
  s.homepage = 'http://ruby-zoom.rubyforge.org'
  s.platform = Gem::Platform::RUBY
  s.author = [ "Laurent Sansonetti", "Ed Summers" ]
  s.summary = 'Ruby/ZOOM provides a Ruby binding to the Z40.50 Object-Orientation Model (ZOOM), an abstract object-oriented programming interface to a subset of the services specified by the Z39.50 standard, also known as the international standard ISO 23950.  This version introduces ZOOM Extended Services.'
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

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_zip = true
  pkg.need_tar = true
end

Rake::RDocTask.new do |rd|
  rd.main = "README.md"
  rd.rdoc_files.include("README.md", "ext/*.c")
end
