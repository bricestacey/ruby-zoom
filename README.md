Ruby/ZOOM
=========

Ruby/ZOOM provides a Ruby binding to the Z39.50 Object-Orientation Model 
(ZOOM), an abstract object-oriented programming interface to a subset of the 
services specified by the Z39.50 standard, also known as the international 
standard ISO 23950.

This software is based (and therefore depends) on YAZ, a free-software 
implementation of the Z39.50/SRW/SRU standards, but could be easily ported to 
any ZOOM compliant implementation.

Ruby/ZOOM is free-software, covered by the GNU Lesser General Public License 
and provided without any warranties of any kind.

Requirements
------------

    Ruby:       http://www.ruby-lang.org/
    YAZ (*):    http://www.indexdata.dk/yaz/ 

    * : Make sure you did pass the --enabled-shared option to the configure
        script before building YAZ.  Ruby/ZOOM requires a YAZ shared library 
        and YAZ does not build it by default. 

Build from Source
-------

    0. checkout out code from git at github
    1. install Rake
    2. rake 
    3. gem install pkg/zoom-*.gem

Build from Source using bundler
-----------------

    0. add to your Gemfile:
    gem 'zoom', :git => 'https://github.com/bricestacey/ruby-zoom.git'
    1. bundle

Samples
-------

    Some programming examples are available in the `sample' directory. 

Canonical Sample Program
------------------------

  To give a flavour of the Ruby binding, here is ZOOM's equivalent of the
  ``Hello World'' program: a tiny Z39.50 client that fetches and displays
  the MARC record for Farlow & Brett Surman's The Complete Dinosaur from
  the Library of Congress.


    require 'zoom'

    ZOOM::Connection.open('z3950.loc.gov', 7090) do |conn|
      conn.database_name = 'Voyager'
      conn.preferred_record_syntax = 'USMARC'
      rset = conn.search('@attr 1=7 0253333490')
      p rset[0]
    end

ZOOM Extended Services
----------------------

  With release 0.4.1 extended services support has been added which allows
  create, update and delete of records. This only works for XML.
  Here's an example adapted from the test suite. 

    require 'zoom'

    new_record = File.read('programming_ruby.xml')

    ZOOM::Connection.open('localhost:99999/test') do |conn|
      p = conn.package
      p.wait_action = 'waitIfPossible'
      p.action = 'specialUpdate'
      p.record = new_record
      p.send('update')
      p.send('commit')     
    end


Copying
-------

    Copyright (c) 2005 Laurent Sansonetti <lrz@chopine.be>
      
    This library is free software.
    You can distribute/modify this program under the terms of
    the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.

Project Website
---------------

    http://ruby-zoom.rubyforge.org
