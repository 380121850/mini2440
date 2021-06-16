SUBDIRS = big

if XMLUI
smallicons = 2rightarrow.png bookmark_add.png bookmark.png configure.png \
	fileclose.png filenew.png find.png

if LARGEICONS
icons =
else
icons = back.png fileprint.png forward.png gear.png gohome.png \
	images_display.png lock.png reload.png stop.png viewmag.png \
	window_list.png \
	fileopen.png filesave.png filesaveas.png \
	editcut.png editcopy.png editpaste.png undo.png redo.png
endif

rcfile = konqe.rc
else
smallicons =
icons =
rcfile =
endif

action_DATA = $(smallicons) $(icons)
actiondir   = $(kde_datadir)/konq-embed/actions

shellrc_DATA = $(rcfile)
shellrcdir   = $(kde_datadir)/konq-embed
