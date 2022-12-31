# frozen_string_literal: true

class TestPackage < Test::Unit::TestCase
  def setup
    @connection = ZOOM::Connection.new
  end

  def test_connection_package
    assert(@connection.respond_to?("package"))
    p = @connection.package
    assert_equal(p.class.to_s, "ZOOM::Package")
  end

  def test_option_returns_same_value
    p = @connection.package
    p.action = "update"
    assert_equal("update", p.action)
  end

  def test_options_containing_hyphen
    # option contact-name
    p = @connection.package
    assert(p.respond_to?("contact_name"))
    assert(p.respond_to?("contact_name="))
    assert(p.respond_to?("set_contact_name"))

    p.contact_name = "contact_name value"
    assert_equal("contact_name value", p.contact_name)
  end

  def test_option_containing_fullstop
    # option correlationInfo.note
    p = @connection.package
    assert(p.respond_to?("correlation_info_note"))
    assert(p.respond_to?("correlation_info_note="))
    assert_equal(true, p.respond_to?("set_correlation_info_note"))

    p.correlation_info_note = "correlation_info_note value"
    assert_equal("correlation_info_note value", p.correlation_info_note)
  end
end
