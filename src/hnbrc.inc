"##########\n\
# configuration file for hnb,\n\
#\n\
# use a tabspacing of 8 when editing this file\n\
#\n\
# (please be aware that this is the configuration file for a development version,\n\
#  things might change drastically soon,.. or perhaps later)\n\
\n\
########## \n\
# what kind of bullets are displayed before each item\n\
#\n\
bullet_parent   +\n\
#bullet_parent   -\n\
#bullet_parent   *\n\
#bullet_parent   ·\n\
bullet_leaf   -\n\
#bullet_leaf   *\n\
#bullet_leaf   ·\n\
\n\
\n\
########\n\
# default format to use available parameters:\n\
# 0 - xml using hnb DTD\n\
# 1 - tab indented ascii\n\
# 2 - xml using no DTD (xml editor)\n\
# 3 - xml using OPML DTD\n\
\n\
default_format 0\n\
\n\
#########\n\
#\n\
# defining how each node is displayed, the display string syntax is\n\
# interpreted as follows:\n\
#\n\
#   spaces turn into real spaces, i means indentation, - means bullet,\n\
#   d means the real data of the node, x is a temporary placeholder for\n\
#   upcoming columntypes,. (for debugging only)\n\
#\n\
#   i and x can take an argument specifying how many characters wide\n\
#   the field should be\n\
#\n\
#   until further features are added, only the multiplier after\n\
#   indentation actually does something\n\
#\n\
# \n\
display_format i4- d\n\
\n\
#########\n\
# what command is executed when a user wants a action performed on a\n\
# entry containing an url\n\
# \n\
# * is subsituted for the url\n\
#\n\
#web_command opera -remote 'openURL(*)'\n\
#web_command opera -remote 'openURL(*,new-window)'\n\
#web_command netscape -remote 'openURL(*)'\n\
#web_command netscape -remote 'openURL(*,new-window)'\n\
#web_command mozilla -remote 'openURL(*)'\n\
#web_command mozilla -remote 'openURL(*,new-window)'\n\
#web_command konqueror *\n\
\n\
web_command galeon -n *\n\
\n\
\n\
#########\n\
# what command is executed when a user wants a action performed on a\n\
# entry containing a mailaddress\n\
# \n\
# * is subsituted for the mailaddress\n\
#\n\
#mail_command xterm -rv pine *\n\
\n\
mail_command xterm -rv +sb mutt *\n\
\n\
\n\
##########\n\
# amount of tree displayed\n\
#\n\
# 0 all anchesteral levels expanded\n\
# 1 same as 0, but expand first level of children of current item (buggy)\n\
# 2 expand whole tree\n\
# 3 hide everything but the \"anchestral path\" (buggy)\n\
#\n\
collapse_mode	0\n\
\n\
##########\n\
# lock the position of the selected node to upper half of the screen \n\
#\n\
fixedfocus	0\n\
\n\
##########\n\
# allow to move beyond topmost and bottommost siblings\n\
#\n\
forced_down	0\n\
forced_up	0\n\
\n\
##########\n\
# how long the program should wait before accepting that ESC(27d) is the\n\
# ESC key and not the start of an escape sequence\n\
# increase this number if you are using hnb over a slow network link\n\
# value given is in ms (1/1000seconds)\n\
#\n\
escdelay 100\n\
\n\
\n\
#########\n\
# keep whitespace as is when loading file from xml\n\
#\n\
keepwhitespace	0\n\
\n\
##########\n\
# save current position when saving a database\n\
#\n\
savepos		0\n\
\n\
xml_cuddle 1\n\
\n\
##########\n\
# color scheme configuration, syntax is:\n\
# \"style <item>  <foreground/background> <attribute,[attribute]>\"\n\
# legal items are shown below\n\
# colors are: blue, cyan, green, red, magenta, yellow, black and white\n\
# attributes are: normal, reverse, bold, underline\n\
\n\
style menuitem		default/default	reverse\n\
style menutext		cyan/default normal\n\
style parentnode	default/default bold\n\
style node		default/default	normal\n\
style bullet		cyan/default	normal\n\
style selected		yellow/red 	bold\n\
style parentselected	yellow/red	bold\n\
style background	default/default	normal\n\
\n\
##########\n\
# keybindings\n\
#\n\
# syntax: \n\
#   \"bindkey <scope[,scope[...]]>  <key>   <action>\"\n\
#\n\
# multiple actions can be chained together by using the virtual key \"..\"\n\
# as the following keybinding\n\
#\n\
##################################################\n\
\n\
context main\n\
	helptext <ESC>| exit || |F2| save || |F5| commandline || |return| edit || | / | find || |^S| ort\n\
\n\
	bind	esc		context quitsave\n\
\n\
	bind	up		up\n\
	bind	down		down\n\
	bind	left		left\n\
	bind	right		right\n\
	bind 	npage		pagedown\n\
	bind  	ppage		pageup\n\
\n\
	bind  	return  	edit\n\
	bind  	del		remove\n\
	bind  	ins		insert_below\n\
	bind	..		edit\n\
	bind  	tab		complete\n\
\n\
	bind  	^T		toggle_todo\n\
	bind  	^D		toggle_done\n\
\n\
	bind  	>		indent\n\
	bind  	<		outdent\n\
\n\
	bind  ^F			context search\n\
	bind	..		save_state\n\
	bind	..		getquery\n\
	bind	..		next_match\n\
\n\
	bind  	f2		save\n\
\n\
	bind	^X		cut\n\
	bind	^C		copy\n\
	bind	^V		paste\n\
	bind	..		down\n\
\n\
	bind	^S		save_state\n\
	bind	..		sort\n\
\n\
	bind	backspace	backspace\n\
	bind	backspace2	backspace\n\
\n\
	bind	f5		commandline\n\
	bind	..		status_clear\n\
\n\
	bind	sleft		movenode left\n\
	bind	sright		movenode right\n\
	bind	sprevious	movenode up\n\
	bind	snext		movenode down\n\
\n\
	bind	^G		context grab\n\
	bind	..		save_state\n\
\n\
	bind	^Z		restore_state\n\
	bind	^A		action\n\
\n\
	bind	f1		status Take a look at ~/.hnbrc to check out the configuration\n\
	bind	..		status there you will also find details about the available keybindings.\n\
\n\
context quitsave\n\
	helptext Save on exit? |    | Y |es | J |a || | N |o || |ESC| cancel\n\
\n\
	bind	Y	save\n\
	bind	..	quit\n\
	bind	y	save\n\
	bind	..	quit\n\
\n\
	bind	j	save\n\
	bind	..	quit\n\
	bind	J	save\n\
	bind	..	quit\n\
\n\
	bind	n	quit\n\
	bind	N	quit\n\
\n\
	bind	esc	context main\n\
	bind	any unbound\n\
\n\
\n\
context grab\n\
	helptext Node grabbed |  |return| |space| drop here || |ESC| cancel\n\
	bind	esc		restore_state\n\
	bind	..		context main\n\
	bind	return		context main\n\
	bind	space		context main\n\
\n\
	bind	left		movenode left\n\
	bind	right		movenode right\n\
	bind	up		movenode up\n\
	bind	down		movenode down\n\
\n\
	bind	any		unbound\n\
\n\
\n\
context search\n\
	helptext Searching |  |ESC| cancel || | n |ext || | p |revious || |space|/|return| go\n\
	bind	esc		context main\n\
	bind	..		restore_state\n\
	bind	n		next_match\n\
	bind	N		next_match\n\
	bind	P		prev_match\n\
	bind	p		prev_match\n\
	bind	up		prev_match\n\
	bind	down		next_match	\n\
	bind	cancel		poppd\n\
	bind	return		context main\n\
	bind	any		unbound\n\
\n\
context confirm\n\
	helptext  y | | j | |return| confirm || |any key| cancel\n\
	bind	y		confirm\n\
	bind	j		confirm\n\
	bind	return		confirm\n\
	bind	any		cancel\n\
\n\
context lineedit\n\
	helptext lineedit\n\
	bind	up		up\n\
	bind	down		down\n\
	bind	left		left\n\
	bind	right		right\n\
	bind	return		confirm\n\
	bind	^S		split\n\
	bind	^J		join\n\
	bind	backspace	backspace\n\
	bind	backspace2	backspace\n\
	bind	^A		bol\n\
	bind	^E		eol\n\
	bind	home		bol\n\
	bind	end		eol\n\
	bind	del		delete\n\
	bind	tab		complete\n\
	bind	esc		cancel\n\
\n\
context nodeedit\n\
	helptext editing node |  |ESC| revert || |return| confirm\n\
	bind	up		up\n\
	bind	down		down\n\
	bind	left		left\n\
	bind	right		right\n\
	bind	return		confirm\n\
	bind	^S		split\n\
	bind	^J		join\n\
	bind	backspace	backspace\n\
	bind	backspace2	backspace\n\
	bind	^A		bol\n\
	bind	^E		eol\n\
	bind	home		bol\n\
	bind	end		eol\n\
	bind	del		delete\n\
	bind	tab		complete\n\
	bind	esc		cancel\n\
\n\
context main\n\
\n\
status hnb - hierarchical notebook   http://hnb.sourceforge.net/\n\
status (c) Øyvind Kolås 2001-2003    pippin@users.sourceforge.net\n\
"
