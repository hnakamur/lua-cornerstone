local exports = {}

exports['regexp.match'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('so...')
  local mr = re:match('something is something')
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 1)
  test.equal(mr:last(), 5)
  test.done()
end

exports['regexp.match.offset'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('so...')
  local mr = re:match('something is something', 2)
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 14)
  test.equal(mr:last(), 18)
  test.done()
end

exports['regexp.match.reuse'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('so...')

  local mr = re:match('something is something')
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 1)
  test.equal(mr:last(), 5)

  mr = re:match('something is something', 2)
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 14)
  test.equal(mr:last(), 18)

  test.done()
end

exports['regexp.match.caseless'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp

  local re = regexp.compile('so...')
  local mr = re:match('Something is something')
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 14)
  test.equal(mr:last(), 18)

  re = regexp.compile('so...', regexp.CASELESS)
  mr = re:match('Something is something')
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 1)
  test.equal(mr:last(), 5)

  test.done()
end

exports['regexp.match.capture'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('([^:]+):\\s+(.*)')
  local mr = re:match('Connection: close')
  test.ok(mr)
  test.equal(mr:captureCount(), 2)
  test.equal(mr:first(0), 1)
  test.equal(mr:last(0), 17)
  test.equal(mr:first(1), 1)
  test.equal(mr:last(1), 10)
  test.equal(mr:first(2), 13)
  test.equal(mr:last(2), 17)
  test.done()
end

exports['regexp.match.utf8'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('..')
  local mr = re:match('\227\129\130\227\129\132')
  test.ok(mr)
  test.equal(mr:captureCount(), 0)
  test.equal(mr:first(0), 1)
  test.equal(mr:last(0), 2)
  test.done()
end

exports['regexp.match.word'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile([[\w+]])
  local mr = re:match('Isaac Newton, physicist')
  test.ok(mr)
  test.equal(mr:group(0), 'Isaac')
  test.done()
end

exports['regexp.match.no_match'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile('(\w+) (\w+)') -- Oops, forgot to escape.
  local mr, err = re:match('Isaac Newton, physicist')
  test.is_nil(mr)
  test.equal(err, 'NOMATCH')
  test.done()
end

exports['regexp.match.group'] = function(test)
  local cs = require('cornerstone')
  local regexp = cs.utf8.regexp
  local re = regexp.compile([[(\w+) (\w+)]])
  local mr = re:match('Isaac Newton, physicist')
  test.ok(mr)
  test.equal(mr:group(0), 'Isaac Newton')
  test.equal(mr:group(1), 'Isaac')
  test.equal(mr:group(2), 'Newton')
  test.done()
end

return exports
