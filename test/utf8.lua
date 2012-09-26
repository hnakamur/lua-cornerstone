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

exports['utf8.char.surrogate_pair'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.char(0x2000B), '\240\160\128\139')
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

exports['utf8.codePoint.surrogate_pair'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.codePoint('\240\160\128\139'), 0x2000B)
  test.done()
end

exports['utf8.len.ascii'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.len('a'), 1)
  test.equal(utf8.len('ab'), 2)
  test.equal(utf8.char())
  test.done()
end

exports['utf8.len.empty'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.len(''), 0)
  test.done()
end

exports['utf8.len.hiragana'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.len('\227\129\130'), 1)
  test.equal(utf8.len('\227\129\130\227\129\132'), 2)
  test.done()
end

exports['utf8.reverse.ascii'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.reverse('abc'), 'cba')
  test.done()
end

exports['utf8.reverse.hiragana'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.reverse('\227\129\130\227\129\132\227\129\134'),
      '\227\129\134\227\129\132\227\129\130')
  test.done()
end

exports['utf8.sub.ascii'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.sub('abc', 1, 2), 'ab')
  test.equal(utf8.sub('abc', 2), 'bc')
  test.equal(utf8.sub('abc', -1), 'c')
  test.equal(utf8.sub('abc', -2, -1), 'bc')

  test.equal(utf8.sub('abc', 4), '')
  test.equal(utf8.sub('abc', 3, 2), '')

  test.done()
end

exports['utf8.sub.hiragana'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', 1, 2),
      '\227\129\130\227\129\132')
  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', 2),
      '\227\129\132\227\129\134')
  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', -1),
      '\227\129\134')
  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', -2, -1), 
      '\227\129\132\227\129\134')

  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', 4), '')
  test.equal(utf8.sub('\227\129\130\227\129\132\227\129\134', 3, 2), '')

  test.done()
end

exports['utf8.endsWith'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.ok(utf8.endsWith('hello.txt', '.txt'))
  test.ok(not utf8.endsWith('hello.txt', '.md'))
  test.ok(utf8.endsWith('hello', ''))
  test.ok(utf8.endsWith('', ''))
  test.ok(not utf8.endsWith('', 'no'))
  test.done()
end

exports['utf8.indexOf'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.indexOf('hello.txt', 'he'), 1)
  test.equal(utf8.indexOf('hello.txt', '.'), 6)
  test.equal(utf8.indexOf('hello.txt', ''), 1)
  test.equal(utf8.indexOf('hello.txt', '', 4), 4)
  test.equal(utf8.indexOf('hello.txt', 'll', 4), nil)
  test.equal(utf8.indexOf('hello.txt', 'no'), nil)
  test.equal(utf8.indexOf('cornerstone', 'r', 4), 6)
  test.equal(utf8.indexOf('hello.txt', 'he', 10), nil)
  test.equal(utf8.indexOf('\227\129\130\227\129\132\227\129\134',
      '\227\129\132'), 2)
  test.equal(utf8.indexOf('\227\129\130\227\129\132\227\129\132\227\129\134',
      '\227\129\132', 3), 3)
  test.done()
end

exports['utf8.lastIndexOf'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.equal(utf8.lastIndexOf('cornerstone', 'e'), 11)
  test.equal(utf8.lastIndexOf('cornerstone', 'x'), nil)
  test.equal(utf8.lastIndexOf('cornerstone', 'o'), 9)
  test.equal(utf8.lastIndexOf('cornerstone', 'o', 5), 2)
  test.equal(utf8.lastIndexOf('cornerstone', 'n', 3), nil)
  test.equal(utf8.lastIndexOf('cornerstone', ''), 11)
  test.equal(utf8.lastIndexOf('cornerstone', 'co'), 1)
  test.equal(utf8.lastIndexOf(
      '\227\129\130\227\129\132\227\129\132\227\129\134',
      '\227\129\132'), 3)
  test.done()
end

exports['utf8.startsWith'] = function(test)
  local cs = require('cornerstone')
  local utf8 = cs.utf8
  test.ok(utf8.startsWith('hello', 'he'))
  test.ok(not utf8.startsWith('hello', 'she'))
  test.ok(utf8.startsWith('hello', ''))
  test.ok(utf8.startsWith('', ''))
  test.ok(not utf8.startsWith('', 'no'))
  test.done()
end

return exports
