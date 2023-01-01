# frozen_string_literal: true

class SearchTest < Test::Unit::TestCase
  # Walter McGinnis, 2007-08-06
  # for some reason the result as string has an extra \n at the end
  # that now needs to be trimmed off the end
  # by doing this with a gsub for replacing \n\n pattern with a single \n
  # if this changes back to a single \n in the future
  # this shouldn't break
  def test_search_results
    ZOOM::Connection.open("z3950.loc.gov", 7090) do |conn|
      conn.database_name = "Voyager"
      conn.preferred_record_syntax = "USMARC"
      result_set = conn.search("@attr 1=7 0253333490")
      assert_equal 1, result_set.length
      assert_equal File.read("test/record.txt"), result_set[0].to_s.gsub("\n\n", "\n")
      assert_equal File.read("test/record.dat"), result_set[0].raw
    end
  end
end
