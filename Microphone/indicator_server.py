#!/usr/bin/env python
import gtk,appindicator,subprocess,os,sys

class indicator:
	def __init__(self):
		try:
			os.chdir("Microphone")
		except:
			print "Currently in",os.getcwd()
		PWD=str(os.getcwd())
		self.ind = appindicator.Indicator("MyApp", PWD + "/Indicator/mic.png", appindicator.CATEGORY_APPLICATION_STATUS)
		self.ind.set_status(appindicator.STATUS_ACTIVE)

		self.menu_setup()
		self.ind.set_menu(self.menu)

	def menu_setup(self):
		self.menu = gtk.Menu()

		self.p_item = gtk.MenuItem("Install a Plugin")
		self.p_item.connect("activate", self.install)
		self.p_item.show()
		self.menu.append(self.p_item)

		self.r_item = gtk.MenuItem("Restart Servers")
		self.r_item.connect("activate", self.restart)
		self.r_item.show()
		self.menu.append(self.r_item)

		self.quit_item = gtk.MenuItem("Quit")
		self.quit_item.connect("activate", self.quit)
		self.quit_item.show()
		self.menu.append(self.quit_item)

	def main(self):
		gtk.main()

	def quit(self, widget):
		sys.exit(0)
	
	def restart(self,widget):
		os.chdir("../")
		subprocess.call(["./restart"])

	def install(self,widget):
		os.chdir("../")
		subprocess.call(["./install_gui"])
		os.chdir("Microphone")

ind = indicator()
ind.main()
