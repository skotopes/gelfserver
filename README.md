# About

This application is a simple replacement for graylog server that you can use while developing your software. It's intended to be used on developer's host and provides only basic functionality. By default it listens for UDP GELF on localhost:12201.

![Main window](/GELFServer.png)

# Prebuilt packages

MacOS: http://arfarius.com/GELFServer.dmg

# How to build

	qmake .
	make

# TODO:

- Chunked GELF
- Permanent storage
- nice UI
