local exports = {}

exports['bitvector'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  test.equal(type(v), 'userdata')
  test.equal(v:length(), 1000)
  for i = 1, 1000 do
    v:set(i, i % 5 == 0)
  end
  test.ok(v:get(10))
  test.done()
end

exports['bitvector.__index'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  for i = 1, 1000 do
    v:set(i, i % 5 == 0)
  end
  test.ok(v[10])
  test.done()
end

exports['bitvector.__newindex'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  for i = 1, 1000 do
    v[i] = i % 5 == 0
  end
  test.ok(v:get(10))
  test.done()
end

exports['bitvector.__len'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  test.equal(#v, 1000)
  test.done()
end

exports['bitvector.__tostring'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  test.equal(tostring(v), 'bitvector(1000)')
  test.done()
end

exports['bitvector.checkudata'] = function(test)
  local cs = require('cornerstone')
  local ok, err = pcall(bitvector.get, io.stdin, 10)
  test.ok(not ok)
  test.is_string(err)
  test.done()
end

return exports
