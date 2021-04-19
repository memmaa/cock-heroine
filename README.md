# Cock Heroine
A tool for recording, editing and playing beat patterns for cock hero videos

# Why?
The year is 2015. Cock Hero movies have been around for a while, but technology hasn't caught up yet.
Some Cock Hero videos have simple beats - maybe they're a bit boring. Others have intricate and difficult ones - that's fun, but getting them right takes your attention away from the video. If only there was a better way...
Neither the fleshlight launch nor funscripts will emerge for a couple of years, and 'beats only' tracks are a distant fantasy.
The author has an arduino, a collection of vibrating toys, and a mission: to make them play cock hero.

# Is this another funscript editor?
No, for all of the reasons above.  
But, as they are a popular file format for storing sex hardware instructions, funscripts are supported.

# So what does it do then?
Cock Heroine is designed specifically with Cock Hero games in mind.  
You can store timestamps for all the events, just like you can with a funscript, but these events aren't tied to particular hardware.  
You can import funscripts, or record events on-the-fly, and then use the editor to make sure they're prefectly in time with the music.
Then you can use them to control hardware and play back the events.

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
* Export beat audio file (using any beat sound you choose) for use in completed videos, or for use by other tools.
* UI controls for some customisations
![image](https://user-images.githubusercontent.com/82333228/115297179-ce464000-a153-11eb-9e08-d94e04ffa8be.png)
* Handy support - both playback and csv export
* Adjust stroke location, speed, and choose how short strokes are handled
* Export to full speed or half speed funscripts
* Keyboard shortcuts for almost everything
* Probably some other stuff I forgot
