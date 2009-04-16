"""
Usage:
  python client.py list artist equals Hanggai
  python client.py play title like '%Banjo%'
"""
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

def make_client(port, host):
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
	return client

def make_query(args):
	"""Construct a FileQuery based on the args."""
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
	return file_query

if __name__ == '__main__':
	parser = optparse.OptionParser()
	parser.add_option('-p', '--port', type='int', dest='port', default=9090, help='The port larkd is running on')
	parser.add_option('--host', dest='host', default='localhost', help='The host larkd is running on')
	opts, args = parser.parse_args()

	client = make_client(opts.port, opts.host)

	cmd = args[0]
	args = args[1:]

	# You issue a queue/like command like:
	# python client.py queue artist like foo title equals bar
	if cmd in ('queue', 'list'):
		file_query = make_query(args)
		files = client.listFiles(file_query)
		print 'Matched URIs'
		print '------------'
		print '\n'.join(f.uri for f in files)

		if cmd == 'queue':
			print '\nIssuing play request to larkd...'
			client.enqueueByQuery(file_query)
	elif cmd == 'play':
		st = client.status()
		st.playback = Playback.PLAYING
		if st.position <= 0:
			st.position = 0
		if args:
			st.position = int(args[0])
		print st
		client.setStatus(st)
	elif cmd == 'scan':
		fs_path = args[0]
		print 'Scanning %s' % fs_path
		client.scan(fs_path)
	elif cmd == 'playlist':
		print client.playlist()
	
	elif cmd == 'status':
		print client.status()
	else:
		raise ValueError("no such command")
