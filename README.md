baconwm
=======

Automatically populating full-screen wm.

In brainstorming phase currently.

design philosophy
=======
Everything should open full-screen, and new programs spawn a new workspace.  
As a client is closed/killed, the array of workspaces should index and collapse existing workspaces.  
Ctrl+Tab should allow cycling through workspaces. 

todo
======
Everything. :D  

1. Create function and implement main loop to hold XDisplay open.
2. Spawn a single program (probably a terminal) via keycommands.
3. Create a clean kill of wm.
4. At this point, implement virtual workspaces to conform to design concept.
5. Bloat it up. 
