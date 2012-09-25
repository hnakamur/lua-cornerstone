local exports = {}

exports['utf8.char.ascii'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.char(97), 'a')
  test.equal(utf8.char(97, 98), 'a', 'b')
  test.equal(utf8.char())
  test.done()
end

exports['utf8.char.hiragana'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.char(0x3042), '\227\129\130')
  test.equal(utf8.char(0x3042, 0x3044), '\227\129\130', '\227\129\132')
  test.done()
end

exports['utf8.codePoint.ascii'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  local string = require('string')
  test.equal(utf8.codePoint('a'), string.byte('a'))
  test.equal(utf8.codePoint('abc', 2), string.byte('abc', 2))
  test.equal(utf8.codePoint('abc', 2, 3), string.byte('abc', 2, 3))

  -- negative index
  test.equal(utf8.codePoint('abc', -2, -1), string.byte('abc', -2, -1))

  -- partly out of range.
  test.equal(utf8.codePoint('abc', 2, 4), string.byte('abc', 2, 4))
  test.equal(utf8.codePoint('abc', 0, 2), string.byte('abc', 0, 2))

  -- empty
  test.equal(utf8.codePoint('abc', 4), string.byte('abc', 4))

  test.done()
end

exports['utf8.codePoint.hiragana'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.codePoint('\227\129\130\227\129\132', 1), 0x3042)
  test.equal(utf8.codePoint('\227\129\130\227\129\132', 2), 0x3044)
  test.equal(utf8.codePoint('\227\129\130\227\129\132', 1, 2), 0x3042, 0x3044)
  test.equal(utf8.codePoint('\227\129\130\227\129\132', 3))
  test.equal(utf8.codePoint('\227\129\130\227\129\132', -2), 0x3042)
  test.equal(utf8.codePoint('\227\129\130\227\129\132', -1), 0x3044)
  test.done()
end

return exports
