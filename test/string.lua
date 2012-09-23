local exports = {}

exports['string.endsWith'] = function(test)
  local cs = require('cornerstone')
  local str = cs.string
  test.ok(str.endsWith('hello.txt', '.txt'))
  test.ok(not str.endsWith('hello.txt', '.md'))
  test.ok(str.endsWith('hello', ''))
  test.ok(str.endsWith('', ''))
  test.ok(not str.endsWith('', 'no'))
  test.done()
end

exports['string.indexOf'] = function(test)
  local cs = require('cornerstone')
  local str = cs.string
  test.equal(str.indexOf('hello.txt', 'he'), 1)
  test.equal(str.indexOf('hello.txt', '.'), 6)
  test.equal(str.indexOf('hello.txt', ''), 1)
  test.equal(str.indexOf('hello.txt', '', 4), 4)
  test.equal(str.indexOf('hello.txt', 'll', 4), nil)
  test.equal(str.indexOf('hello.txt', 'no'), nil)
  test.equal(str.indexOf('cornerstone', 'r', 4), 6)
  test.equal(str.indexOf('hello.txt', 'he', 10), nil)
  test.done()
end

exports['string.lastIndexOf'] = function(test)
  local cs = require('cornerstone')
  local str = cs.string
  test.equal(str.lastIndexOf('cornerstone', 'e'), 11)
  test.equal(str.lastIndexOf('cornerstone', 'x'), nil)
  test.equal(str.lastIndexOf('cornerstone', 'o'), 9)
  test.equal(str.lastIndexOf('cornerstone', 'o', 5), 2)
  test.equal(str.lastIndexOf('cornerstone', 'n', 3), nil)
  test.equal(str.lastIndexOf('cornerstone', ''), 11)
  test.equal(str.lastIndexOf('cornerstone', 'co'), 1)
  test.done()
end

exports['string.startsWith'] = function(test)
  local cs = require('cornerstone')
  local str = cs.string
  test.ok(str.startsWith('hello', 'he'))
  test.ok(not str.startsWith('hello', 'she'))
  test.ok(str.startsWith('hello', ''))
  test.ok(str.startsWith('', ''))
  test.ok(not str.startsWith('', 'no'))
  test.done()
end

return exports
