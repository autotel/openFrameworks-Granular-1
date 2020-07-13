# openFrameworks-Granular-1

video: https://www.youtube.com/watch?v=LBbfVmMTy4Y
blog post: https://autotel.co/portfolio/granular-synth-exercise/

# making it work

edit ofApp.cpp at the line where samples are loaded and write down the names of your own samples. The app looks for the samples in the "data" directory relative to the executable.

modify the makefile so that OF_ROOT points to your actual of_root. 

run `make`. The executable should appear at the `bin` directory.

# Granular synth toy programming exercise.

There are two samples, and a small portion of this sample is looped when the dot is triggered. The distance between the center of the screen and the dot will define the length of the granule (i.e. small portion of sound); while the polar position (i.e. position of the dot in a counter-clockwise fashion) determines the position, along the whole sample, from where the granule is taken.

This program loads a sample in WAV PCM format, searches all zero crossings, and lets you play with the sample granules by dragging the circles. It snaps the start and endpoints of all the granules to the nearest zero crossing, effectively eliminating the jumps that occur otherwise. I chose this method instead of fade in/fade out, because it keeps better the original timbre of the sample.

There are still some popping noises because it doesn’t wait the granule buffer to end playing before updating it to the new. It happens when scrubbing because of sudden changes on the sample being looped.
It also needs optimization in the way that the waveform is displayed. In the version that is displayed here, the program draws every single point of the granule, making the drawing routine very laggy.

From this point, there are many interesting processing and interaction possibilities for this mockup, that may follow depending on available time.

This project was developed during Nuno Correia‘s course “Generative Media Coding”, at Aalto Media Lab.
