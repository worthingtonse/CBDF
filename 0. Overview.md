# Overview of CBDF

CBDF is meant to reduce the size of any document and databeing transfered from A to B. However, in this repo, we show the specifications of how the CBDC is used 
for QMail files. Qmail files end with a .qmail such as "myfile.qmail". The specification used for qmail is nearly identical to the file format that will be used for the
Qweb page format. CBDF is also used to pass data from a RAIDA Client to a RAIDA Server. 

QMail's CBDF radically reduces message size by using the Compact Binary Document Format (CBDF). As a novel alternative to HTML, a 
CBDF-formatted message can be as little as 1% of the size of an equivalent modern email. For example, instead of creating an email's 
layout using HTML and CSS, qmails have 255 unique layouts to choose from making it possible to create the qmail layout with one byte. 
Qmail layouts are made up of panels that can themselves be divided into sub panels using one byte. Each panel is described by a 16-byte code. 
Colors are reduced from 16,777,216 to 65,535 choices. The number of fonts possible are limited to 256. 
Since this is all encoded in binary, it is extreemly effficient.

This efficiency nearly eliminates the bandwidth and storage required to handle spam. Furthermore, messages sent to groups are stored securely 
in shared distributed storage, removing the need for servers to hold multiple copies. This massive reduction in required system resources 
(bandwidth, storage, RAM, electricity) makes it feasible for individuals to host QMail servers on consumer-grade hardware.

## QMail File struture: 
The qmail files are devided into four major parts. These parts are simular to the HEAD and BODY tags that HTML uses. 

1. Meta Data
2. Styles
3. Text with style commands from the Style Section
4. Logic 

Each of these sections can have sub parts except the meta section. 

### Styles has:
Note: This has not been implemented yet and can change. 
1. Background
2. Container Styles including Modals (popup boxes)
3. Text Styles including the Qmail subject and sub subject
4. Container objects (images, nav bars, tables, forms, controls)

### Text
1. Background Layer
2. Container Layers (There can be up to 256 layers that can sit on top of each other and be turned on and off)
3. Content Layer (Items inside containers like images, tables, frames, navigation, forms and widgets)

### Logic
Note that this has not been finalized yet. This language maybe based on using an Erlang Virtual Maching (BEAM) or Elixir. 
1. Supervisor
2. Actors
3. Initialized Data
4. Uninitialized Data
5. Preload
6. Main/Start
7. Container Logic links (Links Actors to container events)
8. Page Close
