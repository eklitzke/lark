import sys
import pprint
from urlparse import urlparse
from thrift.transport import TTransport
from thrift.transport import TSocket
from thrift.transport import THttpClient
from thrift.protocol import TBinaryProtocol

import lark.gen.LarkService as LarkService
from lark.gen.ttypes import *

host = 'localhost'
port = 9090
uri = ''
framed = False
http = False

if http:
  transport = THttpClient.THttpClient(host, port, uri)
else:
  socket = TSocket.TSocket(host, port)
  if framed:
    transport = TTransport.TFramedTransport(socket)
  else:
    transport = TTransport.TBufferedTransport(socket)

protocol = TBinaryProtocol.TBinaryProtocol(transport)
client = LarkService.Client(protocol)
transport.open()
file_query = FileQuery()
file_query.binaryTerms = [BinaryTerm("artist", TermOperator.like, "Abe Vigoda")]
files = client.listFiles(file_query)
#pprint.pprint(files)

print "playing:"
#client.enqueueByQuery(file_query)
status = client.status()
print status

status.playback = Playback.PLAYING
status.position = 20
client.setStatus(status)
print client.status()

