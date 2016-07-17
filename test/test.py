#!/usr/bin/env python

from graypy import GELFHandler
from time import sleep
import logging

class Main(object):
	def __init__(self):
		super(Main, self).__init__()
		self.logger = logging.getLogger("main")
		self.logger.setLevel(logging.DEBUG)
		self.logger.addHandler(GELFHandler('localhost', 12201))

	def __call__(self):
		while True:
			self.logger.info('test message', extra={
				'key': 'value'
			})
			sleep(1)

if __name__ == "__main__":
	Main()()
