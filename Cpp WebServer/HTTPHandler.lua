function getstrfromtable(t)
	local str = ''
	if(type(t) == 'table') then
		for i,j in pairs(t) do
		if(i=='title') then
			str=str..'<h1>'
		else
			str=str..'<p>'
		end
		str = str .. getstrfromtable(j)
		if(i=='title') then
			str=str..'</h1>'
		else
			str=str..'</p>'
		end
		end
	end
	if(type(t)=='string') then
		str=t
	end
	return str
end
function onMessage(packet)--id is number, Packet - HTTP packet
	local id = packet["id"]
	local servdir = packet["servdir"]
	local ret = {
		sendFile=true,
		response=true,
		messageBody=nil,
		headers={},
		code=0,
		message="",
		cookies={},
	}
	local cookies = {}
	if(packet.headers.Cookie ~= nil) then--парсинг куки
		local strings = {}
		string.gsub(packet.headers.Cookie, "([^;]+)", function (w)
		  table.insert(strings, w)
		end)
		for i,j in pairs(strings) do
			_,_,name,value = j:find('([^=]+)=([^=]+)')
			cookies[name]=value
		end
	end
	local uri = packet.uri
	if(uri:sub(1,1)=="/") then
		uri=uri:sub(2,#uri)
	end
	if(packet.method == 'GET') then
		if (uri == 'test') then
			ret.code=200--использование куки для подсчета количества посещений страницыы
			ret.message='success'
			if(cookies.count==nil) then
				 ret.messageBody="<html><head><meta charset=\"utf-8\"></head><body><p>вы зашли на страницу 0 раз</p></body></html>";
				 ret.cookies.count="0"
			 else
				ret.cookies.count=tostring(tonumber(cookies.count)+1)
				ret.messageBody="<html><head><meta charset=\"utf-8\"></head><body><p>вы зашли на страницу "..cookies.count.." раз</p></body></html>";
			end
			ret.sendFile=false
			return ret
		end
		if (uri:find('[.]') == nil) then
			local file = io.open(servdir..'/'..uri..'.html','rb')
			if(file ~= nil) then
				file:close()
				ret.sendFile=true
				ret.code=200
				ret.message="OK"
				return ret
			end
		end
		local file = io.open(servdir..'/'..uri,'rb')
		if(file ~= nil) then
			ret.sendFile=true
			return ret
		end
		ret.sendFile=false
		ret.message = 'Not Found'
		ret.code=404
		ret.messageBody='<html><head><meta charset="utf-8"></head><body><h1>File not found!</h1></body></html>'
		return ret
	end
	if(packet.method == 'POST') then--
		ret.sendFile=false
		ret.code=404
		ret.messageBody='<html><body><p>Пост запросы не настроены</p></body></html>'
		ret.message='doesn\'t support'
	end
	return ret
end