import asyncio
import json
import cachetools
import datetime

TTL = 30

clients = cachetools.TTLCache(4096, TTL)


class EchoServerProtocol:
    def connection_made(self, transport):
        self.transport = transport

    def datagram_received(self, data, addr):
        print("Received:", data.decode())
        message = json.loads(data)
        client_id = message.get('machine')
        if not client_id:
            self.transport.sendto(json.dumps({'status':'bad request'}).encode(), addr)
            return
        clients[client_id] = addr
        if message.get('action') == 'invite':
            message['date'] = datetime.datetime.now().isoformat()
            self.send_to_all(message)

        if message.get('shutdown'):
            del clients[client_id]

        self.transport.sendto(json.dumps({'status':'online'}).encode(), addr)


    def send_to_all(self, message):
        data = json.dumps(message).encode()
        
        for addr in clients.values():
            self.transport.sendto(data, addr)


loop = asyncio.get_event_loop()
print("Starting UDP server")

listen = loop.create_datagram_endpoint(EchoServerProtocol, local_addr=('0.0.0.0', 7818))
transport, protocol = loop.run_until_complete(listen)


try:
    loop.run_forever()
except KeyboardInterrupt:
    pass

transport.close()
loop.close()
