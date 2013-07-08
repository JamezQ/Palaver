#!/usr/bin/env python

import urllib2,os,pynotify

with open("CONFIG",'r') as data:
		for l in data.read().split("\n"):
			print l
			if l.startswith("UPDATEID="):
				updateID = l.replace("\n",'').replace("UPDATEID=",'')

f = urllib2.urlopen("http://palaver.bmandesigns.com/functions.php?f=coreUpdate&id="+updateID)
data = f.read()
print data
id,files = data.split("|$|")
files = files.split("|#|")

if id > updateID:
	f = urllib2.urlopen("http://github.com/JamezQ/Palaver/archive/master.zip")
	with open('update.zip', "wb") as local_file:
		local_file.write(f.read())

	os.system("unzip -q update.zip -d Updates")
	for f in files:
		if f != '':
			print f
			os.system("mkdir "+os.path.dirname(f))
			os.system("cp Updates/Palaver-master/"+f+" "+f)
	os.system("rm Updates -r -f")
	os.system("rm update.zip")
	print "Done"
	lines = []
	with open("CONFIG",'r') as data:
		for l in data.read().split("\n"):
			lines.append(l)
	with open("CONFIG",'w') as data:
		for l in lines:
			if l.startswith("UPDATEID="):
				data.write("UPDATEID="+str(id)+"\n")
			else:
				if l != '\n':
					data.write(l+"\n")
	pynotify.init("Speech Recognition")
	n = pynotify.Notification("Palaver Update","Palaver just successfully updated")
	n.show()
	os.system("./restart")
else:
	print "System Up To Date"
