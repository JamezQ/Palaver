#!/usr/bin/python

# open a microphone in pyAudio and listen for taps

import pyaudio
import struct
import math, time, os

INITIAL_TAP_THRESHOLD = 0.010
FORMAT = pyaudio.paInt16 
SHORT_NORMALIZE = (1.0/32768.0)
CHANNELS = 2
RATE = 44100  
INPUT_BLOCK_TIME = 0.05
INPUT_FRAMES_PER_BLOCK = int(RATE*INPUT_BLOCK_TIME)
# if we get this many noisy blocks in a row, increase the threshold
OVERSENSITIVE = 15.0/INPUT_BLOCK_TIME                    
# if we get this many quiet blocks in a row, decrease the threshold
UNDERSENSITIVE = 120.0/INPUT_BLOCK_TIME 
# if the noise was longer than this many blocks, it's not a 'tap'
MAX_TAP_BLOCKS = 0.15/INPUT_BLOCK_TIME

def get_rms( block ):
	# RMS amplitude is defined as the square root of the 
	# mean over time of the square of the amplitude.
	# so we need to convert this string of bytes into 
	# a string of 16-bit samples...quit()


	# we will get one short out for each 
	# two chars in the string.
	count = len(block)/2
	format = "%dh"%(count)
	shorts = struct.unpack( format, block )

	# iterate over the block.
	sum_squares = 0.0
	for sample in shorts:
		# sample is a signed short in +/- 32768. 
		# normalize it to 1.0
		n = sample * SHORT_NORMALIZE
		sum_squares += n*n

	return math.sqrt( sum_squares / count )

class TapTester(object):
	def __init__(self):
		self.pa = pyaudio.PyAudio()
		self.stream = self.open_mic_stream()
		self.noisycount = MAX_TAP_BLOCKS+1 
		self.quietcount = 0 
		self.errorcount = 0
		self.trigger = True
		self.switchmiddle = False
		self.switchon = False
		self.switchoff = True
		self.starting = True
		self.startBlocks = 0
		self.ampAverage =0

	def stop(self):
		self.stream.close()

	def find_input_device(self):
		device_index = None            
		for i in range( self.pa.get_device_count() ):     
			devinfo = self.pa.get_device_info_by_index(i)   

			for keyword in ["mic","input"]:
				if keyword in devinfo["name"].lower():
					device_index = i
					print "Alternate Device Found",devinfo["name"]

		if device_index == None:
			print( "No preferred input found; using default input device." )

		return device_index

	def open_mic_stream( self ):
		device_index = self.find_input_device()

		stream = self.pa.open(   format = FORMAT,
				                 channels = CHANNELS,
				                 rate = RATE,
				                 input = True,
				                 input_device_index = device_index,
				                 frames_per_buffer = INPUT_FRAMES_PER_BLOCK)

		return stream


	def listen(self):
		try:
			block = self.stream.read(INPUT_FRAMES_PER_BLOCK)
		except IOError, e:
			# dammit. 
			self.errorcount += 1
			print( "(%d) Error recording: %s"%(self.errorcount,e) )
			self.noisycount = 1
			return

		amplitude = get_rms( block )
		amp2 = amplitude*1000
		if self.starting == True:
			if self.startBlocks == 0:
				print "Please talk into the mic"
				time.sleep(1)
				self.ampAverage = float(amp2)
			else:
				#print amp2
				self.ampAverage = (self.ampAverage + float(amp2)) / 2.0
			self.startBlocks += 1
			if self.startBlocks == 51:
				print "Original",self.ampAverage
				self.ampAverage = self.ampAverage - (self.ampAverage * .5)
				print "Baseline set at",self.ampAverage
				self.starting = False
				print "Ready"
		#print self.noisycount, self.quietcount
		#else:
			#print self.noisycount,self.quietcount
		if amp2 > self.ampAverage:
			self.noisycount += 1
			self.quietcount = 0
		else:
			self.noisycount = 0
			self.quietcount += 1
		if self.noisycount > 15:
			if self.switchoff:
				print "Switch On"
				self.switchon = True
			self.switchoff = False
		if self.quietcount > 5:
			if self.switchon and self.switchmiddle == False:
				print "Switch Middle"
				os.system("./hotkey")
				self.quietcount = 0
				self.switchmiddle = True
			if self.quietcount > 20:
				if self.switchon and self.switchmiddle:
					print "Switch Off"
					os.system("./hotkey")
				self.switchon = False
				self.switchmiddle = False
				self.switchoff = True

if __name__ == "__main__":
	tt = TapTester()

	listen = True

	while listen == True:
		tt.listen()
