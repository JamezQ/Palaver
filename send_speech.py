#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import httplib
import json
import sys
import os

class Stt:
    def get_language(self):
        filePath = os.environ['HOME'] + '/.palaver.d/UserInfo'

        if os.path.isfile(filePath):
            content = open(filePath, 'r')

            for line in content:
                if 'LANGUAGE' in line:
                    return str(line).partition('=')[2].strip()

        return str('en')

    def speech_to_text(self, audio):
        url = 'www.google.com'
        path = '/speech-api/v1/recognize?client=chromium&lang=' + self.get_language()
        headers = {'Content-type': 'audio/x-flac; rate=16000' };
        conn = httplib.HTTPSConnection(url)
        conn.request('POST', path, audio, headers)
        response = conn.getresponse()
        data = response.read()
        try:
            jsdata = json.loads(data)
            return jsdata['hypotheses'][0]['utterance']
        except Exception:
            return ''
	    
