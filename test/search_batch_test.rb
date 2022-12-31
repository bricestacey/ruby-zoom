# frozen_string_literal: true

require "test/unit"
require "zoom"

class SearchBatchTest < Test::Unit::TestCase
  # Terry Reese 9-17-07
  # Test for batch results
  def test_search_batch_test
    ZOOM::Connection.open("z3950.loc.gov", 7090) do |conn|
      conn.database_name = "Voyager"
      conn.preferred_record_syntax = "USMARC"
      result_set = conn.search('@attr 1=4 "Oregon"')
      records = result_set[0..10]
      assert_equal 10, records.length
    end
  end
end
