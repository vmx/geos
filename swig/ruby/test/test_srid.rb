#!/usr/bin/env ruby

require 'test/unit'
require 'geos'

class SridTest < Test::Unit::TestCase

  def test_srid
    cs = Geos::CoordinateSequence.new(1,2)
    cs.set_x(0, 7)
    cs.set_y(0, 8)
    
    geom1 = Geos.create_point(cs)
    geom1.srid = 4326
  end

  def test_roundtrip_srid
    geom1 = Geos.geom_from_wkt("POINT(7 8)")
    geom1.srid = 4326
    
    assert_equal(4326, geom1.srid)
    hex = Geos.geom_to_hex(geom1)

    geom2 = Geos.geom_from_hex(hex)
    assert_equal(4326, geom2.srid)
  end
  
  def test_read_srid
    # srid=4326;POINT(7 8)
    xdr = "0101000020E61000000000000000001C400000000000002040"
    
    Geos::wkb_byte_order = Geos::GEOS_WKB_NDR
    geom = Geos.geom_from_hex(xdr)
    
    assert_equal(7, geom.coord_seq.get_x(0))
    assert_equal(8,geom.coord_seq.get_y(0))
    assert_equal(4326, geom.srid)
  end
end
