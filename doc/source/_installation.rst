Installing Habit
================

System Requirements
*******************

Habit2 for MacOS requires MacOS 10.10 (Yosemite) or greater. 

In order to run experiments, you will need at least one external monitor attached (for single-stimulus experiments,
two are needed for dual-stimulus experiments). 

When developing and testing experiments, however, you do not need any external monitor attached. When running Habit2
on a computer that has no monitors attached, the main Habit2 window will display all experiment names on a yellow background, 
to indicate the settings for that experiment do not allow it to be run 'on that computer'. You will still be able to 'test'
the experiment, because Habit2 allows experiments to be run in a testing mode where the stimuli are displayed in a small window
on your screen. When external monitors are attached, and when the monitor assignments are made for the workspace, the 
experiment names should be displayed on a green background once again.

Download
********

Download the latest version of Habit2 from `our website <http://habit.ucdavis.edu/download.html>`_. 

Install - MacOS
***************

The download comes in the form of an installer package. The first time you install Habit2, your Mac may (depending on your
security settings) *block the installation* 
and issue a warning message that says "Habit2 cannot be installed because it is from an unidentified developer".  This message 
comes from your Mac's "Gatekeeper" service, which checks installer packages and applications against your computer's security
settings, and ultimately attempts to protect end users from installing malicious software. 
The message **does not mean that Habit2 presents a security risk**. Rather, the Gatekeeper only allows installations
to proceed if the application was downloaded from the App Store, where Apple actively checks the applications for malicious software. 
The App Store can be a cumbersome hurdle for developers, and so Apple allows a middle ground, where developers (like me) can 
distribute software *outside* the App Store while still giving end users a measure of security for their systems. In particular, 

* Gatekeeper can determine whether the *installer package* was tampered with after the developer created the package
* Gatekeeper can determine whether the application itself, and its supporting libraries, was tampered with after the developer created the package
* The installer package and the executable code are *digitally signed* by the developer, and the code carries that digital 
  signature with it. The signature can be traced back to a developer who is part of Apple's Developer program. In the case of Habit, 
  the signature comes from an account held by the University of California, Davis. 

`Here's a web page <https://mac-how-to.gadgethacks.com/how-to/open-third-party-apps-from-unidentified-developers-macos-0158095/>`_ 
explaining all this, and **how to install the package**.


