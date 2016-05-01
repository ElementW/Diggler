require('io')

local diggler = {
	mods = {},
	modOverlays = {},
	modsById = {},

	exportedFuncs = {}
}

function diggler.export(name, func)
	diggler.exportedFuncs[name] = func
end

package.loaded['diggler'] = diggler
package.loaded['lds'] = require('lds/lds')

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

diggler.MODSTATUS = {
	UNAVAILABLE = 0,
	DISABLED = 1,
	ERRORED = 2,
	LOADED = 10,
	INITIALIZED = 20,
	DEINITIALIZED = 90,
	UNLOADED = 100
}

local match = string.match
local function trim(s)
	return match(s,'^()%s*$') and '' or match(s,'^%s*(.*%S)')
end

function diggler.loadModLua(path)
	local digglerOverlay = {}
	local packageOverlay = { ['path'] = path .. '/?.lua;' .. package.path }
	setoverlay(packageOverlay, package)
	local env = {
		['package'] = packageOverlay,
		['print'] = function (...)
			print("<init " .. path .. ">", ...)
		end,
		['require'] = function (module)
			if module == 'diggler' then
				return digglerOverlay
			end
			return require(module)
		end
	}
	setoverlay(env, _G)

	local file = io.open(path .. '/mod.lua', "r")
	if file == nil then
		error("mod.lua not found")
	end
	local untrusted_code = file:read('*a')
	file:close()

	if untrusted_code:byte(1) == 27 then error("binary bytecode prohibited") end
	local untrusted_function, message = loadstring(untrusted_code)
	if not untrusted_function then error(message) end
	setfenv(untrusted_function, env)
	local status, ret = pcall(untrusted_function)

	--- Sanity checks
	if ret == nil then
		error("No mod table returned")
	end
	local fieldChecks = {'uuid', 'id', 'name', 'version', 'versionStr'}
	for _, field in ipairs(fieldChecks) do
		if ret[field] == nil then
			error(field .. " is nil")
		end
	end
	do -- UUID
		if type(ret.uuid) ~= 'string' then
			error("uuid isn't a string")
		end
		if ret.uuid:len() ~= 22 then
			error("uuid is of length " .. ret.uuid:len() .. ", expected 22")
		end
		local uuidInvalid = ret.uuid:match('[^a-zA-Z0-9+/]')
		if uuidInvalid ~= nil and uuidInvalid ~= "" then
			error("uuid contains invalid characters: " .. uuidInvalid)
		end
	end
	do -- ID
		if type(ret.id) ~= 'string' then
			error("id isn't a string")
		end
		if ret.id:len() == 0 or trim(ret.id):len() == 0 then
			error("id is empty")
		end
		if ret.id:match('%s') ~= nil then
			error("id contains whitespace")
		end
		local punct = ret.id:match('[^%a%d]')
		if punct ~= nil and punct ~= "" then
			error("id contains non-letter/non-digit characters: '" .. punct .. "'")
		end
	end
	do -- Version number
		if type(ret.version) ~= 'number' then
			error("version isn't a number")
		end
	end
	do -- Version string
		if type(ret.versionStr) ~= 'string' then
			error("versionStr isn't a string")
		end
	end

	--- Setup actual mod environment
	env.CurrentMod = ret
	
	diggler.modOverlays[ret.uuid] = {
		['env'] = env,
		['package'] = packageOverlay,
		['diggler'] = digglerOverlay
	}

	env.print = function (...)
		print(env.CurrentMod.id..":", ...)
	end
	for name, func in pairs(diggler.exportedFuncs) do
		digglerOverlay[name] = function (...)
			func(env.CurrentMod, ...)
		end
	end

	if status then
		return ret, nil
	end
	return status, ret
end

function diggler.loadMod(path)
	local mod, err = diggler.loadModLua(path)
	if mod then
		if diggler.mods[mod.uuid] then
			error("Mod '" .. mod.id .. "' already loaded")
		end
		mod.status = diggler.MODSTATUS.LOADED
		diggler.mods[mod.uuid] = mod
		diggler.modsById[mod.id] = mod
		print("Loaded mod '" .. mod.name .. "' <" .. mod.uuid .. " " .. mod.id .. "> v" .. mod.versionStr .. " (" .. mod.version .. ")")
		return mod
	else
		print("Error loading mod: " .. err)
	end
	return nil
end

function diggler.initMod(mod)
	mod.init()
	mod.status = diggler.MODSTATUS.INITIALIZED
end

function diggler.getMod(mod, id)
	return diggler.modsById[id]
end
diggler.export("getMod", diggler.getMod)

function diggler.getModByUuid(mod, uuid)
	return diggler.mods[uuid]
end
diggler.export("getMod", diggler.getMod)


function diggler.registerBlock(mod, name, block)
	print("Calling registerBlock from mod " .. (mod and mod.id or "<none>"))
end
diggler.export("registerBlock", diggler.registerBlock)


local m = diggler.loadMod('TestMod')
diggler.initMod(m)
