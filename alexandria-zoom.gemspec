# frozen_string_literal: true

RUBY_ZOOM_VERSION = "0.5.0"

Gem::Specification.new do |s|
  s.name = "alexandria-zoom"
  s.version = RUBY_ZOOM_VERSION
  s.homepage = "http://ruby-zoom.rubyforge.org"
  s.platform = Gem::Platform::RUBY
  s.author = ["Laurent Sansonetti", "Ed Summers"]
  s.summary = <<~TEXT
    Ruby/ZOOM provides a Ruby binding to the Z39.50 Object-Orientation Model
    (ZOOM), an abstract object-oriented programming interface to a subset of
    the services specified by the Z39.50 standard, also known as the
    international standard ISO 23950.  This version introduces ZOOM Extended
    Services.
  TEXT

  s.metadata["rubygems_mfa_required"] = "true"
  s.required_ruby_version = ">= 2.7.0"

  s.files =
    Dir.glob("ext/*.c") +
    Dir.glob("ext/*.h") +
    Dir.glob("ext/*.rb") +
    Dir.glob("test/**/*") +
    Dir.glob("sample/**/*") +
    ["README.md", "ChangeLog", "Rakefile"]
  s.extensions = "ext/extconf.rb"

  s.add_dependency "pkg-config", "~> 1.5.1"

  s.add_development_dependency "rake", "~> 13.0"
  s.add_development_dependency "rubocop", "~> 1.41"
  s.add_development_dependency "test-unit", "~> 3.3"
end
