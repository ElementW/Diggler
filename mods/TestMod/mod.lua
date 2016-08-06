local D = require('diggler')

local TestMod = {
  uuid = "BWtAgKUjcW+H+CUQcOW3/Q",
  id = "TestMod",
  name = "Test Mod",
  version = 1,
  versionStr = "1.0.0",
  description = "A mod to test Lua scripting ability",
  tags = {"test"},
  authors = {"ElementW"},
  license = {
    name = "GPLv3",
    descUrl = "https://www.gnu.org/licenses/gpl-3.0.html",
    fulltextUrl = "https://www.gnu.org/licenses/gpl-3.0.txt"
  },
  deps = {},
  optdeps = {},

  clientside = true,
  serverside = true,

  interfaces = {
    provides = {
      "diggler.isBlockUseless"
    },
    uses = {
      
    }
  }
}

function TestMod.init() 
  print("Hey i'm " .. CurrentMod.id)
  dofile(CurrentModPath .. '/blocks.lua')
end

function TestMod.deinit() 
  print("Bye")
end

print(D.mods)

return TestMod
