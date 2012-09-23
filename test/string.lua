local exports = {}

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

return exports
