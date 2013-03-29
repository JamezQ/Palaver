Palaver 2.0 Rewrite
=====================
Anything in this document is not final, this is a notepad of ideas for the Palaver rewrite. To access the old Palaver, switch the branch to "master".


The first order of business will be to rewrite dictionary.c to be nicer, and to give dictionaries more features. I will be replacing the recognize script with the binary, and the binary will be renamed "palaver". It will do everything from getting the test, changing it's encoding to UTF-8, checking it against dictionaries, and finally running the commands. The majority of palaver will be this c program.

Palaver will thus be separated into these parts.

1. A UI to handle user input of speech, even if that ui is running in the background to continuously get speech.
2. A back end to translate that speech into text.
3. A program to find out what to run based on that text.
4. Hundreds of small scripts that are run based on the command.

The parts are rather separate, with 3 and 4 being the most connected of anything. The majority of the palaver program will be the dictionary interpreter. The rest of the program can be swapped out with other parts.

The next thing to think about will be the new dictionary syntax, these will just be minor ad-ons and tweaks to make it better at matching human speech.

look for it in NEW_SYNTAX.txt