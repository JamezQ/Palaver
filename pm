#!/usr/bin/env python
import pynotify, urllib2, subprocess

f = urllib2.urlopen("http://palaver.bmandesigns.com/functions.php?f=messageUpdate")
id,title,text = f.read().split("||")

with open("CONFIG",'r') as data:
		for l in data.read().split("\n"):
			print l
			if l.startswith("LASTID="):
				lastId = l.replace("\n",'').replace("LASTID=",'')

print lastId
if id > lastId:
	pynotify.init("Speech Recognition")
	n = pynotify.Notification(title,text)
	n.show()
	lines = []
	with open("CONFIG",'r') as data:
		for l in data.read().split("\n"):
			lines.append(l)
	print lines
	with open("CONFIG",'w') as data:
		for l in lines:
			if l.startswith("LASTID="):
				print "Boom!"
				data.write("LASTID="+str(id)+"\n")
			else:
				if l != '\n':
					data.write(l+"\n")
