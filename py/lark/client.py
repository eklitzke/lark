import sys
import pprint
from urlparse import urlparse
from thrift.transport import TTransport
from thrift.transport import TSocket
from thrift.transport import THttpClient
from thrift.protocol import TBinaryProtocol

import lark.gen.LarkService as LarkService
from lark.gen.ttypes import *

import optparse

if __name__ == '__main__':
	parser = optparse.OptionParser()
	parser.add_option('-p', '--port', type='int', dest='port', default=9090, help='The port larkd is running on')
	parser.add_option('--host', dest='host', default='localhost', help='The host larkd is running on')
	opts, args = parser.parse_args()

	uri = ''
	framed = False
	http = False

	if http:
	  transport = THttpClient.THttpClient(opts.host, opts.port, uri)
	else:
	  socket = TSocket.TSocket(opts.host, opts.port)
	  if framed:
		transport = TTransport.TFramedTransport(socket)
	  else:
		transport = TTransport.TBufferedTransport(socket)

	protocol = TBinaryProtocol.TBinaryProtocol(transport)
	client = LarkService.Client(protocol)
	transport.open()

	cmd = args[0]
	args = args[1:]

	# You issue a play command like:
	# python client.py play artist like foo title equals bar
	if cmd == 'play':
		groups = []
		while args:
			groups.append(args[:3])
			args = args[3:]
		
		op_dict = {
			'not': TermOperator.not_,
			'equals': TermOperator.equal,
			'like': TermOperator.like,
			'not_equal': TermOperator.not_equal,
			'less_than': TermOperator.less_than,
			'less_than_equal': TermOperator.less_than_equal,
			'greater_than': TermOperator.greater_than,
			'greater_than_equal': TermOperator.greater_than_equal
		}

		file_query = FileQuery()
		file_query.binaryTerms = []
		for lhs, op, rhs in groups:
			op = op_dict[op]
			file_query.binaryTerms.append(BinaryTerm(lhs, op, rhs))
		files = client.listFiles(file_query)
		print 'Playing:'
		print '--------'
		print '\n'.join(f.uri for f in files)

		client.enqueueByQuery(file_query)
