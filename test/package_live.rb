# frozen_string_literal: true

class PackageLiveTest < Test::Unit::TestCase
  # test z39.50 instance that supports extended services by connecting to
  # zebra instance
  #
  # important: you won't be able to run these tests if port 99999 isn't
  # available

  def test_crud_record
    # start the zebra server if it's not already started
    Dir.chdir("test/zebra") do
      @pid = fork do
        $stderr.close
        exec "zebrasrv tcp:@:99999 -l live_test.log"
      end
    end

    # ensure that the server has time to get up
    sleep 1

    @id = "14055446"
    @record = File.read("test/zebra/records/programming_ruby.xml")
    @record_update = File.read("test/zebra/records/programming_ruby_update.xml")

    # make clean before test
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      p = conn.package
      p.wait_action = "waitIfPossible"
      p.action = "recordDelete"
      p.record = @record_update
      p.send("update")
      p.send("commit")
    end

    # create
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      p = conn.package
      p.wait_action = "waitIfPossible"
      p.action = "specialUpdate"
      p.record = @record
      p.send("update")
      p.send("commit")
    end

    # make sure it's there
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      conn.preferred_record_syntax = "XML"
      result_set = conn.search("@attr 1=12 \"#{@id}\"")
      assert_equal 1, result_set.length
    end

    # make sure can search by author
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      conn.preferred_record_syntax = "XML"
      result_set = conn.search("@attr 1=1 David")
      assert_equal 1, result_set.length
    end

    # update
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      p = conn.package
      p.wait_action = "waitIfPossible"
      p.action = "specialUpdate"
      p.record = @record_update
      p.send("update")
      p.send("commit")
    end

    # confirm update record is there
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      conn.preferred_record_syntax = "XML"
      result_set = conn.search("@attr 1=1 Jason")
      assert_equal 1, result_set.length
    end

    # confirm original record has been written over
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      conn.preferred_record_syntax = "XML"
      result_set = conn.search("@attr 1=1 David")
      assert_equal 0, result_set.length
    end

    # cleanup
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      p = conn.package
      p.wait_action = "waitIfPossible"
      p.action = "recordDelete"
      p.record = @record_update
      p.send("update")
      p.send("commit")
    end

    # make sure the file has been destroyed
    ZOOM::Connection.open("localhost:99999/test") do |conn|
      conn.preferred_record_syntax = "XML"
      result_set = conn.search("@attr 1=12 \"#{@id}\"")
      assert_equal 0, result_set.length
    end

    Process.kill("TERM", @pid)
  end
end
