ffi.cdef(diggler.io.readFile(nil, digglerNative.gameLuaRuntimePath .. '/ffi/content/BlockDef.code.h'))
ffi.cdef(diggler.io.readFile(nil, digglerNative.gameLuaRuntimePath .. '/ffi/content/Registry.code.h'))

local registerBlock = function(mod, name, block)
  local app = block.appearance

  local texturesCount = 0
  for _, _ in pairs(app.textures) do
    texturesCount = texturesCount + 1
  end
  local textures = ffi.new('struct Diggler_Content_BlockDef_appearance_texture[?]', texturesCount)
  local texids = {}
  do
    local i = 0
    for k, v in pairs(app.textures) do
      local tex = textures[i]
      local rpt = v['repeat']
      tex.name = k
      tex.path = v.path
      tex.repeatXdiv = (rpt and rpt.xdiv) or 1
      tex.repeatYdiv = (rpt and rpt.ydiv) or 1
      texids[k] = i
      i = i + 1
    end
  end

  local bdef = ffi.new('struct Diggler_Content_BlockDef', {
    appearance = {
      variabilty = 0,
      texturesCount = texturesCount,
      textures = textures,
      look = {
        type = 1
      }
    }
  })

  local cbdata = bdef.appearance.look.data.cube
  local sides = app.look.sides
  for k, v in pairs(sides) do
    if k == 'all' then
      for f = 0, 5 do
        cbdata.sides[f].texture = texids[v]
      end
    else
      local faceids = {
        xi = 0, ['x+'] = 0,
        xd = 1, ['x-'] = 1,
        yi = 2, ['y+'] = 2,
        yd = 3, ['y-'] = 3,
        zi = 4, ['z+'] = 4,
        zs = 5, ['z-'] = 5
      }
      if faceids[k] then
        cbdata.sides[faceids[k]].texture = texids[v]
      end
    end
  end

  ffi.C.Diggler_Content_Registry_registerBlock(diggler.gameInstance, name, bdef)
end
diggler.export("registerBlock", registerBlock)
