require 'zoom'

ZOOM::Connection.open('z3950.loc.gov', 7090) do |conn|
    conn.database_name = 'Voyager'
    conn.preferred_record_syntax = 'USMARC'
    rset = conn.search('@attr 1=7 0253333490')
    p rset[0]
end
