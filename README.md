# Cock Heroine
A tool for recording, editing and playing beat patterns for cock hero videos

# Licensing
The code is made available under the Artistic License 2.0.  
The main points are:
* You CAN freely use the software as supplied here
* You CAN make and use your own modifications to the code
* BUT there are restrictions on distributing your modified version of the code
* See LICENSING.txt for details.

# Why?
The year is 2015. Cock Hero movies have been around for a while, but technology hasn't caught up yet.  
Some Cock Hero videos have simple beats - maybe they're a bit boring. Others have intricate and difficult ones - that's fun, but getting them right takes your attention away from the video. If only there was a better way...  
Neither the fleshlight launch nor funscripts will emerge for a couple of years, and 'beats only' audio tracks are a distant fantasy.  
The author has an arduino, a collection of vibrating toys, and a mission: to make them play cock hero.  

# Why now?
Fast forward to 2021.
6 years later and life has changed.  
There are lots of new tools available, though none that completely fulfills Cock Heroine's role.
There are new demands on the developer's time and Cock Heroine might have to take a back seat.
By opening up the code to the world, we can:
* Allow other people to make use of the tool, as I have done - seveal popular, publicly available funscripts have been quietly created with Cock Heroine
* Harness the community, allowing other people to contribute to the tool's development.
* Keep me motivated. My excitement for the project has faded a little, and without the enthusiasm of the community, I fear the tool will fall into neglect.

# Is this another funscript editor?
No, for all of the reasons above.  
But, as they became a popular file format for storing sex hardware instructions, funscript support was added.

# So what does it do then?
Cock Heroine is designed specifically with Cock Hero games in mind.  
You can store timestamps for all the events, just like you can with a funscript, but these events aren't tied to particular hardware.  
You can import funscripts, or record events on-the-fly, and then use the editor to make sure they're prefectly in time with the music.  
Then you can use them to create beat tracks, control hardware and play back the events, create funscripts or whatever.

# Feature list
* Linux, Mac and Windows support
* Sortable, editable table of all event data  
![image](https://user-images.githubusercontent.com/82333228/115289801-e5ccfb00-a14a-11eb-8f7f-0f592fd9c0ad.png)
* Record events as you play, in full-screen if you like, then go back and fix them up later
* Use different types of event, like half strokes, edging, butt plug pulses...
* Adjust the 'value' of the events for things like deeper or lighter strokes
* Make 'optional' events for things like breaks between rounds
* Select beats and analyse them  
![image](https://user-images.githubusercontent.com/82333228/115291601-dbabfc00-a14c-11eb-8593-4caa8ba270b5.png)
* Automatic tempo detection, tap a key to specify the tempo, or enter a value yourself
* Automatic, configurable pattern detection and naming  
![image](https://user-images.githubusercontent.com/82333228/115293051-bf10c380-a14e-11eb-9582-9e3c0ebbac73.png)
* Individuallly or batch-edit event data
* Open beats in the editor  
![image](https://user-images.githubusercontent.com/82333228/115291269-735d1a80-a14c-11eb-8fc1-6d70728e03ad.png)
* Uses the concept of 'Beat intervals' or 'values' (the time between the timestamps) instead of the timestamps themselves (yeah, I know there's another musical concept of 'intervals' being the difference in pitch, but that doesn't really apply when you're only working with rhythms)
* Highlighting of unknown values so you can identify them
* Values colour-coded by accuracy so you can quickly see the ones that need tweaking
* Data displayed about all patterns in the editor at the time, automatically named and colour-coded by 'difficulty'
* Preconfigured with all commonly-used beat values
* Add your own custom beat values for complicated or unusual patterns
* Data displayed about all beat values currently detected
* Seldom-matched beat values highlighted for attention as they are often mistakes
* Disable individual beat values if you know you won't use them
* Add: Add extra beats perfectly in time, by specifying their 'value' in relation to the other beats
* Split/merge: Select a range of beats and re-pattern them entering a new sequence of values that will take their place
* Adjust: Tweak values by moving the timestamps on either side - move one, the other, or both equally
  * Select 'smart adjust' to automatically fit it best with the surrounding beats
* Delete: Remove individual values, and optionally shift the surrounding beats to close the gap
* Once you've fixed up any mistakes, optimise your events so they're perfectly in time with the music  
![image](https://user-images.githubusercontent.com/82333228/115296247-a30f2100-a152-11eb-8f53-763327f8ab3a.png)
* Using CockHeroMarkupLanguage, metadata is saved for each event, like the value of the beat, what pattern it's part of, and the tempo
* Import and export to and from MIDI files
* Export beat track audio file (using any beat sound you choose) for use in completed videos, or for use by other tools.
* UI controls for some customisations  
![image](https://user-images.githubusercontent.com/82333228/115297179-ce464000-a153-11eb-9e08-d94e04ffa8be.png)
* Handy support - both playback and csv export
* Adjust stroke location, speed, and choose how short strokes are handled
* Import funscripts either as beats or as raw data
* Export to full speed or half speed funscripts
* Keyboard shortcuts for almost everything
* Probably some other stuff I forgot

# What doesn't it do?
There are (more than) a few things Cock Heroine doesn't do, which I wish it did, but haven't had the time to add yet.  
These include:
* [Buttplug](https://buttplug.io/) support, to control more types of hardware.
* More drag-n-drop UI interfaces
* Support for 'edging' type events on the Handy
* Ability to export beat meters instead of just rendering them inside the tool
* Undo functionality
* Ask to save on exit
* Make it less glitchy and make it crash less often (though to be fair it's not terrible)
* Fully customisable keyboard shortcuts (they're half-done, but never quite made it)
* A list of things that gets longer every time I think about it

# How do I run it?
I'll admit, I haven't found a very good way of distributing it yet.  
The basic steps are:
* Download and install Qt Creator (complete with a version of Qt 5.x) on your platform (Linux, Mac, Windows)
* Download the Cock Heroine source
* Open up the project file (CockHeroine.pro) in Qt Creator
* Set up at least 1 configuration (if Qt installed properly, this should be as easy as clicking 'OK')
* 'Run' the project by clicking the play button in the bottom left of Qt Creator
* On Windows you will probably need to install a video codec pack because... Windows
* On Linux, you might need to add yourself to the comms group in order to connect to the Arduino companion
* Any other problems, consider getting in touch as I might have seen them before.

If you know a better way to package and distribute a Qt application, get in touch!

# How do I use it?
If you have an existing Cock Heroine filetype or funscript:
* File => Open (select your video)
* If the script isn't automatically selected for you, File => Open (select the script)
* Connect your Handy or Arduino companion
* Press play!

If you want to create a new script:
* File => Open (select your video, say 'no' if it asks to load a script as well)
* Press play
* Record events as the video is playing
  * Press 'f' to toggle fullscreen
  * Press 's' or the numberpad '5' to record a normal stroke event
  * Depress the 'e' key to start an edging period
  * Release the 'e' key to finish the edging period
  * There are a bunch of other shoftcuts, but I never got around to documenting them properly
* Pause, play, stop, record more events as you need
  * Use the arrow jeys to skip forward or back in the video
  * Select items in the table and press delete to remove them
* Once you're done recording on-the-fly, select a group of events (click and drag or Ctrl-click) and press 'Enter' or 'Return' to open them in the editor.
* Select the analysis options if you like (the defaults are usually fine), then press OK
* If Analysis fails, make sure the tempo is detected correctly and thata all of the beat 'values' you used are checked in the bottom right table, then click 'Analyse' in the top left to restart analysis.
* Use the editor to fine-tune your inputs and make sure all the detected beata 'values' are correct
  * Press the 'Adjust' button top-centre to automatically select a poorly-matched beat for your attention
* Once all the 'values' are correct, click 'Optimise' to align your beats perfectly
* Click 'Apply' to apply your changes back into the table in the main window
* If you mess up in the editor, click 'Cancel' to exit the ediitor without applying your changes (you can then reselct any events you like and try again)
* Click 'Save' to save your script to a CHML file
  * Do this regularly (usually every time you finish using the editor) - the software is not perfect, and no one like data loss.
* Rinse, repeat and you're done! You can now export to a funscript, play the script on your handy or arduino companion, export a beat track or midi file or whatver you like.

# What's wrong with it?
A lot, probably.  
Don't forget to save regularly.  
It might crash from time to time... Usually if you try to perform an edit action when you just deleted the last beat that was selected. I should fix that, but the bug is a little elusive.  
Just make sure you select the beat you want to edit in the table on the left of the editor, and you'll probably be fine...

# How do I make it better?
* Submit useful bug reports!
* Make a change yourself and and submit a pull request!
* Find a way to donate to me even though I haven't posted my PayPal details!
* Or just send me encouraging messages!
