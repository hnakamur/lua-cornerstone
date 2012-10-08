local exports = {}

exports['bitvector'] = function(test)
  local cs = require('cornerstone')
  local a = bitvector.new(1000)
  test.equal(type(a), 'userdata')
  test.equal(bitvector.length(a), 1000)
  for i = 1, 1000 do
    bitvector.set(a, i, i % 5 == 0)
  end
  test.ok(bitvector.get(a, 10))
  test.done()
end

return exports
