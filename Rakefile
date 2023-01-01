# frozen_string_literal: true

require "rubygems"
require "rake"
require "rake/clean"
require "rake/testtask"
require "rdoc/task"
require "rake/manifest/task"
require "rake/packagetask"
require "rubygems/package_task"
require "mkmf"

CLEAN.include "**/*.o"
CLEAN.include "**/*.so"
CLEAN.include "**/Makefile"
CLEAN.include "**/*.log"
CLEAN.include "pkg"

task default: [:build, :test, :package, "manifest:check"]

task build: [:clean] do
  Dir.chdir("ext") do
    system("ruby", "extconf.rb")
    system("make")
  end
end

task :package do
  system("gem build alexandria-zoom.gemspec")
end

Rake::TestTask.new("test") do |t|
  t.test_files = FileList["test/*_test.rb"]
  t.ruby_opts = ["-r test/unit", "-I ext", "-r zoom"]
  t.verbose = true
end

Rake::TestTask.new("live_test") do |t|
  t.test_files = FileList["test/*_live.rb"]
  t.ruby_opts = ["-r test/unit", "-I ext", "-r zoom"]
  t.verbose = true
end

Rake::Manifest::Task.new do |t|
  t.patterns = ["ext/*.c",
                "ext/*.h",
                "ext/*.rb",
                "test/**/*",
                "sample/**/*",
                "README.md", "ChangeLog", "Makefile"]
end

Rake::RDocTask.new do |rd|
  rd.main = "README.md"
  rd.rdoc_files.include("README.md", "ext/*.c")
end
