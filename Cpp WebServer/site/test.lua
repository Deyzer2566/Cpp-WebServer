function split (inputstr, sep)
    if sep == nil then
		sep = "%s"
    end
    local t={}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
		table.insert(t, str)
    end
    return t
end

function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end

function onMessage(id,data)
	print(data);
	Disconnect(id);
end

function onDisconnect(id)
	
end

function onConnect(id)
	print(id)
end
Count_Players=0
Player = {prot=0,field={},connected=false,nick="",ready=false,have=0}
Players = {}