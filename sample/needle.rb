#!/usr/bin/ruby -w
#
# Given a list of ISBNs and Z39.50 servers, try to get a MARC record
# for each book.  The servers listed here seem to be the best
# worldwide.  If an ISBN cannot be found then we use the xisbn service
# to try different editions.
#
# TODO: If more then one record is found, pick the longest one.
#
# By Devin Bayer - Summer 2006 - Public Domain
#
#
require 'rubygems'
require 'zoom'
require 'net/http'
Net::HTTP.version_1_2
require 'rexml/document'

servers_source = [
    # Server, Username, Password
    [ 'z3950.loc.gov:7090/Voyager' ], # Library of Congress
    [ 'amicus.nlc-bnc.ca/ANY', 'akvadrako', 'aw4gliu' ], # Canada
    [ 'catnyp.nypl.org:210/INNOPAC' ], # New York Public
    [ 'z3950.copac.ac.uk:2100/COPAC' ], # United Kingdom
    [ 'z3950.btj.se:210/BURK' ], # Sweden
    [ '195.249.206.204:210/Default' ], # DenMark
    [ 'library.ox.ac.uk:210/ADVANCE' ], # Oxford
    [ '216.16.224.199:210/INNOPAC' ], # Cambridge
    [ 'prodorbis.library.yale.edu:7090/Voyager' ], # Yale
    [ 'zsrv.library.northwestern.edu:11090/Voyager' ]] # NorthWestern University
    
#########################################
# Connect to each server
#########################################
@servers = Array.new

servers_source.each do |array|
    tries = 0
    $stderr.puts 'INFO: connecting to ' + array[0]
    begin
	con = ZOOM::Connection.new()
	con.preferred_record_syntax = 'MARC21'
	con.element_set_name = 'F'
	if array[1] then
	    con.user = array[1]
	    con.password = array[2]
	end
	con.connect(array[0])
	@servers << con
    rescue RuntimeError
	$stderr.puts 'ERROR: connecting to ' + array[0] + ': ' + $!
	tries += 1
	retry if tries < 3
	$stderr.puts 'WARNING: giving up on ' + array[0]
    end
end

#################################
# search for ISBN on each server
#  return true if found
#################################
def search(isbn)
    @servers.each do |con|
	begin
	    rset = con.search("@attr 1=7 #{isbn}")
	    if rset.size > 0 and rset[0].to_s.chomp.length > 1 then
		$stderr.puts con.host + ': ' + isbn + ': ' + 
		    rset.size.to_s + ' records'
		puts rset[0].raw('marc8')
		return true
	    end
	rescue RuntimeError
	    $stderr.puts 'WARNING: ' + con.host + ': ' +
		isbn + ': ' + $!
	end
    end
    return false
end

#########################################
# Lookup each ISBN
#########################################
failed_isbns = Array.new

xisbn = Net::HTTP.new('labs.oclc.org')

File.open('isbns').each_line do |isbn|
    isbn = isbn.chomp.upcase
    next if isbn.length < 1
    if not search(isbn) then
	tries = 0
	# Try alternate editions
	begin
	    xisbn.start if not xisbn.started?
	    xml = xisbn.get("/xisbn/#{isbn}").body
	rescue SocketError, Errno::ECONNRESET, Errno::EPIPE, EOFError, Timeout::Error
	    xisbn.finish if xisbn.started?
	    if tries < 3 then
		tries += 1
		retry
	    else
		$stderr.puts "ERROR: xisbn failure: " + $!
	    end
	end
	found = false
	REXML::Document.new(xml).root.each_element do |elem|
	    alt = elem.texts.to_s.chomp.upcase
	    next if alt == isbn
	    if search(alt) then
		$stderr.puts "INFO: alternate for #{isbn}: #{alt}" 
		found = true
		break
	    end
	end
	if not found then
	    $stderr.puts "INFO: isbn #{isbn} not found"
	    failed_isbns << isbn
	end
    end
end

if failed_isbns.size > 0 then
    $stderr.puts "ISBN's not found:"
    failed_isbns.each { |isbn| $stderr.puts isbn }
end
