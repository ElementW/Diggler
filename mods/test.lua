require('io')

local hexedryne = {
	mods = {},

	exportedFuncs = {}
}

function hexedryne.export(name, func)
	hexedryne.exportedFuncs[name] = func
end

package.loaded['hexedryne'] = hexedryne

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

hexedryne.MODSTATUS = {
	UNAVAILABLE = 0,
	DISABLED = 1,
	ERRORED = 2,
	LOADED = 10,
	INITIALIZED = 20,
	DEINITIALIZED = 90,
	UNLOADED = 100
}

function hexedryne.loadModLua(path)
	local digglerOverlay = {}
	local packageOverlay = { ['path'] = path .. '/?.lua;' .. package.path }
	setoverlay(packageOverlay, package)
	local env = {
		['package'] = packageOverlay,
		['print'] = function (...)
			print("<init>", ...)
		end,
		['require'] = function (module)
			if module == 'hexedryne' then
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
	for name, func in pairs(hexedryne.exportedFuncs) do
		digglerOverlay[name] = function (...)
			func(env.CurrentMod, ...)
		end
	end

	if r1 then
		return r2, nil
	end
	return r1, r2
end

function hexedryne.loadMod(path)
	local mod, err = hexedryne.loadModLua(path)
	if mod then
		if hexedryne.mods[mod.id] then
			error("Mod already loaded")
		end
		mod.status = hexedryne.MODSTATUS.LOADED
		hexedryne.mods[mod.id] = mod
		print("Loaded mod '" .. mod.name .. "' <" .. mod.id .. "> v" .. mod.versionStr .. " (" .. mod.version .. ")")
		return mod
	else
		print("Error loading mod: " .. err)
	end
	return nil
end

function hexedryne.initMod(mod)
	mod.init()
	mod.status = hexedryne.MODSTATUS.INITIALIZED
end

function hexedryne.getMod(mod, id)
	return hexedryne.mods[id]
end
hexedryne.export("getMod", hexedryne.getMod)

function hexedryne.registerBlock(mod, name, block)
	print("Calling registerBlock from mod " .. (mod and mod.id or "<none>"))
end
hexedryne.export("registerBlock", hexedryne.registerBlock)

local m = hexedryne.loadMod('TestMod')
hexedryne.initMod(m)