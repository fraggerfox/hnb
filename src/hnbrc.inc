"##########\n\
# configuration file for hnb,\n\
#\n\
# use a tabspacing of 8 when editing this file\n\
#\n\
# (please be aware that this is the configuration file for a development version,\n\
#  things might change drastically soon,.. or perhaps later)\n\
\n\
##########\n\
# number of spaces to indent each level relative to the parent level\n\
#\n\
indentspaces	4\n\
\n\
########## \n\
# what kind of bullets are displayed before each item\n\
#\n\
# 0 none\n\
# 1 +/- \n\
# 2 * \n\
# 3 - \n\
#\n\
bulletmode	3\n\
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
##########\n\
# keep whitespace as is when loading file from xml\n\
#\n\
keepwhitespace	0\n\
\n\
##########\n\
# save current position when saving a database\n\
#\n\
savepos		1\n\
\n\
\n\
##########\n\
# color scheme configuration, syntax is:\n\
# \"style <item>  <foreground/background> <attribute,[attribute]>\"\n\
# legal items are shown below\n\
# colors are: blue, cyan, green, red, magenta, yellow, black and white\n\
# attributes are: normal, reverse, bold, underline\n\
\n\
style menuitem		default/default	reverse\n\
style menutext		default/default normal\n\
style node		default/default	normal\n\
style parentnode	default/default bold\n\
style bullet		default/default	normal\n\
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
##################################################\n\
#    available actions:\n\
#####\n\
#           add <string>\n\
#          addc <parent> <string>\n\
#          bind <key> <action> [parameter]\n\
#            cd <path>\n\
#   commandline \n\
#       context <context>\n\
#          copy \n\
#           cut \n\
#          edit \n\
#  export_ascii <filename>\n\
#   export_help <filename>\n\
#    export_hnb <filename>\n\
#   export_html <filename>\n\
#    export_xml <filename>\n\
#      getquery \n\
#      helptext <help for context>\n\
#  import_ascii <filename>\n\
#   import_help <filename>\n\
#    import_hnb <filename>\n\
#    import_xml <filename>\n\
#        indent \n\
#  insert_below \n\
#            ls [-Rst] [path]\n\
#      movenode <up|left|right|down>\n\
#    next_match \n\
#       outdent \n\
#         paste \n\
#          popd \n\
#    prev_match \n\
#         pushd \n\
#           pwd returns the current path\n\
#             q see the entry for 'quit'\n\
#          quit quits the application\n\
#        remove \n\
#            rm [-f] path\n\
#          save \n\
#     show_vars show all variables\n\
#          sort \n\
#        status <message>\n\
#  status_clear \n\
#         style <item> <foreground/background> <attributes>\n\
#   toggle_done \n\
#   toggle_todo \n\
\n\
\n\
context main\n\
	helptext ^Q(uit) ^S(ave) return edit ^F(ind) ^O(rder) F5 commandline\n\
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
	bind  	^N		indent\n\
	bind  	^B		outdent\n\
\n\
	bind  	^F		context search\n\
	bind	..		getquery\n\
\n\
	bind  	^Q		context quit\n\
\n\
	bind  	^S		save\n\
\n\
	bind	^X		cut\n\
	bind	^C		copy\n\
	bind	^V		paste\n\
\n\
	bind	^O		sort\n\
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
	bind	^space		context grab\n\
	bind	..		status grabbed node, move it, and drop it\n\
\n\
context grab\n\
	bind	esc		context main\n\
	bind	return		context main\n\
	bind	space		context main\n\
	bind	left		status movenode left\n\
	bind	right		status movenode right\n\
	bind	up		movenode up\n\
	bind	down		movenode down\n\
	bind	any		unbound\n\
\n\
\n\
context search\n\
	helptext \"ESC-cancel up/down-previous/next - space/return-stop\"\n\
	bind	esc		context main\n\
	bind	down		next_match\n\
	bind	up		prev_match\n\
	bind	cancel		poppd\n\
	bind	return		context main\n\
	bind	any		unbound\n\
\n\
\n\
context quit\n\
	helptext ^S(ave)  ^Q(uit)  ^X(quit and save) ESC(ancel) \n\
\n\
	bind	esc		context main\n\
	bind	^S		save\n\
	bind	..		context main\n\
	bind	s		save\n\
	bind	..		context main\n\
	bind	S		save\n\
	bind	..		context main\n\
	bind	^Q		quit\n\
	bind	q		quit\n\
	bind	Q		quit\n\
	bind	^X		save\n\
	bind	X		save\n\
	bind	x		save\n\
	bind	..		quit\n\
	bind	any		unbound\n\
\n\
context confirm\n\
	bind	y		confirm\n\
	bind	return		confirm\n\
	bind	any		cancel\n\
\n\
context lineedit\n\
	bind	up		up\n\
	bind	down		down\n\
	bind	left		left\n\
	bind	right		right\n\
	bind	return		confirm\n\
	bind	^S		split\n\
	bind	^J		join\n\
	bind	backspace	backspace\n\
	bind	^A		bol\n\
	bind	^E		eol\n\
	bind	home		bol\n\
	bind	end		eol\n\
	bind	del		delete\n\
	bind	tab		complete\n\
	bind	esc		cancel\n\
\n\
context nodeedit\n\
	bind	up		up\n\
	bind	down		down\n\
	bind	left		left\n\
	bind	right		right\n\
	bind	return		confirm\n\
	bind	^S		split\n\
	bind	^J		join\n\
	bind	backspace	backspace\n\
	bind	^A		bol\n\
	bind	^E		eol\n\
	bind	home		bol\n\
	bind	end		eol\n\
	bind	del		delete\n\
	bind	tab		complete\n\
	bind	esc		cancel\n\
\n\
context main\n\
"
