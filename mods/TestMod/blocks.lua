print("Yo, " .. CurrentMod.id .. " blocks.lua here") 

local grassBlock = {
  appearance = {
    variabilty = {'static'},
    textures = {
      grass = {
        path = 'self:/tex/grass.png',
        ['repeat'] = {
          xdiv = 8,
          ydiv = 8
        }
      }
    },
    look = {
      type = 'cube',
      sides = {
        all = 'grass'
      }
    }
  }
}

diggler.registerBlock('grass', grassBlock)
