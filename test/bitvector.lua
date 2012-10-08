local exports = {}

exports['BitVector'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local v = BitVector.new(1000)
  test.equal(type(v), 'userdata')
  test.equal(v:length(), 1000)
  for i = 1, 1000 do
    v:set(i, i % 5 == 0)
  end
  test.ok(v:get(10))
  test.done()
end

exports['BitVector.__index'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local v = BitVector.new(1000)
  for i = 1, 1000 do
    v:set(i, i % 5 == 0)
  end
  test.ok(v[10])
  test.done()
end

exports['BitVector.__newindex'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local v = BitVector.new(1000)
  for i = 1, 1000 do
    v[i] = i % 5 == 0
  end
  test.ok(v:get(10))
  test.done()
end

exports['BitVector.__len'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local v = BitVector.new(1000)
  test.equal(#v, 1000)
  test.done()
end

exports['BitVector.__tostring'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local v = BitVector.new(1000)
  test.equal(tostring(v), 'cs.BitVector(1000)')
  test.done()
end

exports['BitVector.checkudata'] = function(test)
  local cs = require('cornerstone')
  local BitVector = cs.BitVector
  local ok, err = pcall(BitVector.get, io.stdin, 10)
  test.ok(not ok)
  test.is_string(err)
  test.done()
end

return exports
