function onMessage(id,data)
	print(data);
	Send(id,data);
end

function onDisconnect(id)
	print('Disconnected: '..id);
end

function onConnect(id)
	print('Connected: '..id);
end
