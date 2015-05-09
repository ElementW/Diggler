local D = require('Diggler')

local TestMod = {
	id = "TestMod",
	name = "Test Mod",
	version = 1,
	versionStr = "1.0.0",
	description = "A mod to test Lua scripting ability",
	authors = {"gravgun"},
	license = "GPLv3",
	deps = {},
	optdeps = {},
	
	clientside = true,
	serverside = true
}

function TestMod.init() 
	print("Hey i'm " .. CurrentMod.id)
	D.registerBlock('test', {
		dispname = 'block.test.name',
		sandboxTab = 'blocks',
		harvest = { pickaxe = 0, shovel = 10000 },
		maxStackSize = 32,
		tex = "test.png"
	})
end

function TestMod.deinit() 
	print("Bye")
end

print(D.mods)

return TestMod