


MUICacheView v1.00
Copyright (c) 2008 Nir Sofer
Web site: http://www.nirsoft.net



Description
===========

Each time that you start using a new application, Windows operating
system automatically extract the application name from the version
resource of the exe file, and stores it for using it later, in Registry
key known as the 'MuiCache'.

This utility allows you to easily view and edit the list of all MuiCache
items on your system. You can edit the name of the application, or
alternatively, you can delete unwanted MUICache items.
Be aware that even if your delete MUICache items, they'll reappear in the
next time that you run the application.



The Location Of The MUICache Data in the Registry
=================================================

In Windows 2000, Windows XP, Windows Server 2003, the MUICahce data is
stored under
HKEY_CURRENT_USER\Software\Microsoft\Windows\ShellNoRoam\MUICache.
Starting from Windows Vista, the MUICahce data is stored under
HKEY_CURRENT_USER\Software\Classes\Local
Settings\Software\Microsoft\Windows\Shell\MuiCache



Using MUICacheView
==================

MUICacheView doesn't require any installation process or additional DLLs.
In order to start using it, simply run the executable file -
MUICacheView.exe
The main window of MUICacheView displays the list of all MUICache items.
You can select one or more items and use the 'Delete Selected Items'
option to delete them. You can also use the 'Properties' window to edit
the application name of single MUICache item.



System Requirements
===================

This utility works under Windows 2000, Windows XP, Windows Server 2003,
and Windows Vista.



Translating MUICacheView to other languages
===========================================

In order to translate MUICacheView to other language, follow the
instructions below:
1. Run MUICacheView with /savelangfile parameter:
   MUICacheView.exe /savelangfile
   A file named MUICacheView_lng.ini will be created in the folder of
   MUICacheView utility.
2. Open the created language file in Notepad or in any other text
   editor.
3. Translate all string entries to the desired language. Optionally,
   you can also add your name and/or a link to your Web site.
   (TranslatorName and TranslatorURL values) If you add this information,
   it'll be used in the 'About' window.
4. After you finish the translation, Run MUICacheView, and all
   translated strings will be loaded from the language file.
   If you want to run MUICacheView without the translation, simply rename
   the language file, or move it to another folder.



License
=======

This utility is released as freeware. You are allowed to freely
distribute this utility via floppy disk, CD-ROM, Internet, or in any
other way, as long as you don't charge anything for this. If you
distribute this utility, you must include all files in the distribution
package, without any modification !



Disclaimer
==========

The software is provided "AS IS" without any warranty, either expressed
or implied, including, but not limited to, the implied warranties of
merchantability and fitness for a particular purpose. The author will not
be liable for any special, incidental, consequential or indirect damages
due to loss of data or any other reason.



Feedback
========

If you have any problem, suggestion, comment, or you found a bug in my
utility, you can send a message to nirsofer@yahoo.com
