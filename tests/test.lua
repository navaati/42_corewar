print('Corewar Lua Test written for Lua 5.2, current '.._VERSION)

local opt = {
	['diff|'] = 70,
	['diff<'] = 160,
	['diff>'] = 128
}

local opts = {
	['-h'] = {
		desc = 'The help option',
		fn = function () os.exit(usage()) end
	},
	['-C'] = {
		desc = 'The ColorList option',
		fn = function () os.exit(color_list()) end
	},
	['-c'] = {
		desc = 'The ColorSelection option',
		fn = function () return function (color1)
			opt['diff|'] = color1
			return function (color2)
			opt['diff<'] = color2
			return function (color3)
			opt['diff>'] = color3
			end end end
		end
	}
}

function usage()
	for k,v in pairs(opts) do
		print(k, v.desc)
	end
end

local colors = {
	black = 0,
	red = 1,
	green = 2,
	yellow = 3,
	blue = 4,
	magenta = 5,
	cyan = 6,
	white = 7,

	orange = 202
}

function color_shell(str, color)
	if colors[color] then return('\27[38;5;'..colors[color]..'m'..str..'\27[39m')
	else return('\27[38;5;'..color..'m'..str..'\27[39m') end
end

function color_start(color)
	if colors[color] then io.write('\27[38;5;'..colors[color]..'m')
	else io.write('\27[38;5;'..color..'m') end
end

function color_end()
	io.write('\27[39m')
end

function color_list()
	for i=0,255 do
		color_start(i)
		io.write(string.format('%4d', i))
		if (i + 1) % 8 == 0 then io.write('\n') end
		color_end()
	end
end

local champs = {}

for l in io.popen('ls */*.s'):lines() do
	table.insert(champs, l:match('(.+)%.'))
end

function process_out(f)
	for l in f:lines() do
		print(l)
	end
end

function exec(path)
	return function(opt)
		return function(redirection)
			return function(champ)
				local cmd = path..' '..opt..' '..champ..' '..redirection
				print(cmd)
				local res, exit, sig = os.execute(cmd)
				if sig ~= 0 then
					color_start('196')
					print( exit, sig )
					color_end()
				else
					print( exit, sig )
				end
			end
		end
	end
end

function make_champions(champion)
	local acc = champion
	local function nest(n)
		if n then acc = acc..' '..n return nest
		else return acc end
	end
	return nest
end

local compile = exec('../../zaz_corewar/asm')('')('')

-- 0 : Show only essentials
-- 1 : Show lives
-- 2 : Show cycles
-- 4 : Show operations (Params are NOT litteral ...)
-- 8 : Show deaths
-- 16 : Show PC movements (Except for jumps)

local corewar = exec('../build/cli/corewar')('-v 20 -d 4000')(' &> /tmp/corewar_file')
local zazcorewar = exec('../../zaz_corewar/corewar')('-v 20 -d 4000')(' &> /tmp/zaz_file')

function color_diff(file)
	for l in file:lines() do
		if l:match('<') then
			color_start(opt['diff<']) print(l) color_end()
		elseif l:match('[^-]>') then
			color_start(opt['diff>']) print(l) color_end()
		elseif l:match('%D%s|') then
			color_start(opt['diff|']) print(l) color_end()
		end
	end
end

function test(champion)
	color_start(0)
	print('------------------------')
	compile(champion..'.s')
	color_end()
	corewar(champion..'.cor')
	zazcorewar(champion..'.cor')
	color_diff( io.popen('diff -y /tmp/zaz_file /tmp/corewar_file') )
end

local fn

for k,v in ipairs(arg) do
	if fn then fn = fn(v)
	elseif opts[v] then
		fn = opts[v].fn
		fn = fn()
	else
		print('unreconized option:', v)
		usage()
	end
end

for k,v in pairs(opt) do
	print(k,v)
end

os.execute('make -C ..')

for k,v in pairs(champs) do
	test(v)
end
