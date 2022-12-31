# frozen_string_literal: true

class SearchTest < Test::Unit::TestCase
  # Jason Ronallo, 2007-09-20
  # With libyaz3 3.0.12 threading fails for gem at
  # point of retrieving a record out of a result set.
  # libyaz3 3.0.10 worked fine.
  # Update: 3.0.14 fixes the problem.

  def test_thread
    thread = Thread.new do
      ZOOM::Connection.open("z3950.loc.gov", 7090) do |conn|
        conn.database_name = "Voyager"
        conn.preferred_record_syntax = "USMARC"
        result_set = conn.search("nature")
        array = result_set[0, 6] # change the 2nd number
        assert_equal 6, array.length # change the number
      end
    end
    thread.join
  end
end
