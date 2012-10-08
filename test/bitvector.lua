local exports = {}

exports['bitvector'] = function(test)
  local cs = require('cornerstone')
  local v = bitvector.new(1000)
  test.equal(type(v), 'userdata')
  test.equal(bitvector.length(v), 1000)
  for i = 1, 1000 do
    bitvector.set(v, i, i % 5 == 0)
  end
  test.ok(bitvector.get(v, 10))
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
