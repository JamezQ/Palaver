If you wish to help with translations, email brett@bmandesigns.com

A repo is currently in beta. To join send an email to brett@bmandesigns.com with your first and last name, email address and a username

BEFORE USE, PLEASE INSTALL:
sox
python-argparse


# For note to self
libsox-fmt-mp3
mutt

# You may already have the notification system.
notification-daemon,notify-osd or xfce4,notifyd

wget
espeak

# To send text
xvkbd

# For automation
xautomation


I may have missed something, if I did tell me.


TO USE: 
 * TO SKIP STEP 1 , run ./setup while in the base directory.
1. If you are running 32 bit, recompile dictionary.c.
2. Add the hotkey to ubuntu by going to keyboard->shortcuts
   then selecting "custom shortcuts"
   Click the + to add one, and use the full path to 'hotkey' for
   the command.
3. Press the hotkey before you speak *and* after you speak. 
   This is something that will be worked on in the private beta, to
   make it so it's not required.


Upon first use, some users have reported an error message: 
"There was an error while reading main.dic"
If this happens, try running 'make -B' from within "/Palaver/Recognition" folder,
then use your hotkey and dictate one of the default commands. i.e. "Open Documents"


Some commands require extra setup, I think this is only the
note to self command. You will need to setup a .muttrc and change
the note_to_self script.
http://go2linux.garron.me/linux/2010/10/how-send-email-command-line-gmail-mutt-789


If you have any trouble setting it up, please don't hesitate to ask.


Hey everyone in the private beta, if you are wondering about the
change in the code since the video, I mainly rewrote the scripts to
not use a full path. I also changed a bit of dictionary.c.

Here is what I want to work before I release the code.

PLEASE READ BEFORE_RELEASE.TXT

I want a decent amount of simple scripts for people to use
when they are making their *own* scripts, and to use when adding stuff
to dictionaries.

I want to make sure that the application works logically and that
people can change it for their needs and use other people's scripts or
dictionaries. 

If you have any comments, ideas, suggestions, etc, PLEASE email me.
I expect everyone to email me to send code or ask questions or such.

Once we have something that is stable enough, and works on enough
systems, and is easy enough to add commands and such to, it should be
packaged and released.

