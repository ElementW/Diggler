require('io')

local Diggler = {
	mods = {},

	exportedFuncs = {}
}

function Diggler.export(name, func)
	Diggler.exportedFuncs[name] = func
end

package.loaded['Diggler'] = Diggler

local function setoverlay(tab, orig)
	local mt = getmetatable(tab) or {}
	mt.__index = function (tab, key)
		if rawget(tab, key) ~= nil then
			return rawget(tab, key)
		else
			return orig[key]
		end
	end
	setmetatable(tab, mt)
end

Diggler.MODSTATUS = {
	UNAVAILABLE = 0,
	DISABLED = 1,
	ERRORED = 2,
	LOADED = 10,
	INITIALIZED = 20,
	DEINITIALIZED = 90,
	UNLOADED = 100
}

function Diggler.loadModLua(path)
	local digglerOverlay = {}
	local packageOverlay = { ['path'] = path .. '/?.lua;' .. package.path, ['loaded'] = packageLoadedOverlay }
	setoverlay(packageOverlay, package)
	local env = {
		['package'] = packageOverlay,
		['print'] = function (...)
			print("<init>", ...)
		end,
		['require'] = function (module)
			if module == 'Diggler' then
				return digglerOverlay
			end
			return require(module)
		end
	}
	setoverlay(env, _G)

	local file = io.open(path .. '/mod.lua', "r")
	if file == nil then
		return nil, "mod.lua not found"
	end
	local untrusted_code = file:read('*a')
	file:close()

	if untrusted_code:byte(1) == 27 then return nil, "binary bytecode prohibited" end
	local untrusted_function, message = loadstring(untrusted_code)
	if not untrusted_function then return nil, message end
	setfenv(untrusted_function, env)
	local r1, r2 = pcall(untrusted_function)

	-- Setup actual mod environment
	env.CurrentMod = r2
	env.print = function (...)
		print(env.CurrentMod.id..":", ...)
	end
	for name, func in pairs(Diggler.exportedFuncs) do
		digglerOverlay[name] = function (...)
			func(env.CurrentMod, ...)
		end
	end

	if r1 then
		return r2, nil
	end
	return r1, r2
end

function Diggler.loadMod(path)
	local mod, err = Diggler.loadModLua(path)
	if mod then
		if Diggler.mods[mod.id] then
			error("Mod already loaded")
		end
		mod.status = Diggler.MODSTATUS.LOADED
		Diggler.mods[mod.id] = mod
		print("Loaded mod '" .. mod.name .. "' <" .. mod.id .. "> v" .. mod.versionStr .. " (" .. mod.version .. ")")
		return mod
	else
		print("Error loading mod: " .. err)
	end
	return nil
end

function Diggler.initMod(id)
	local mod = Diggler.mods[id]
	mod.init()
	mod.status = Diggler.MODSTATUS.INITIALIZED
end

function Diggler.getMod(mod, id)
	return Diggler.mods[id]
end
Diggler.export("getMod", Diggler.getMod)

function Diggler.registerBlock(mod, name, block)
	print("Calling registerBlock from mod " .. (mod and mod.id or "<none>"))
end
Diggler.export("registerBlock", Diggler.registerBlock)

Diggler.loadMod('TestMod')
Diggler.initMod('TestMod')