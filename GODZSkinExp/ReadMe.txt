========================================================================
    DYNAMIC LINK LIBRARY : GODZSkinExp Project Overview
========================================================================

www.kreationsedge.info - Kreation's Edge
vajuras@yahoo.com

GODZSkelExp.dle programmed by Richard 'vajuras' Osborne (inception 
April 3, 2004). This is a plugin for 3D Studio MAX 7. Exports skeletal 
animations to the GODZ Animation format. Requires the IGame Exporter
which can be downloaded from Sparks Discreet. IGame.dll is now included
as of 3ds max 7.

How To Use:

First Compile the GODZSkelExp.dle file to your plugins folder. To export a model that
has the skin modifier applied (Physique)- select the model and then select
File->Export Selected (then select .GSA file type). This will create a .GSA file 
containing the current model + animations (if any are present). When compiling 
the exporter, make sure you compile it using the 'Hybrid' target.

Exporting Collision Models:

The exporter has the capability to export collision models. This way importers can
associate collision volumes with bones. AFTER the physique modifier has been applied,
simply 'select and link' a sphere to the desired bone. Give the sphere the desired
radius. Now when the mesh is exported, the collision data will be exported as well.

Log:

The exporter always dumps a log file to the same directory as the GSA that was output.

Updated Dec '04 -
Updated to 3dsmax 7 from 3dsmax 5.

Updated Nov 9, 04 -
- Fix for exporting animations for non-biped child bones (dummy boxes)
- Added progress bar

Sources cited:

[1] http://www.gamasutra.com/features/19980320/baumann_01.htm 
"From 3D Studio Max to Direct 3D" by Loic Baumann

[2] http://www.gamasutra.com/features/20000505/sidhu_04.htm
"Creating Procedural Objects in 3D Studio Max" by Gurjeet Sidhu
