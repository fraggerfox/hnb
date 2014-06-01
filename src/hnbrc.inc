"# configuration file for hnb,\n\
#\n\
# use a tab-spacing of 4 when editing this file, or edit it thorugh\n\
# hnb with \"hnb -a ~/.hnbrc\" to take advantage of the indentation in it\n\
#\n\
# (please be aware that this is the configuration file for a development version,\n\
#  things might change drastically soon,.. or perhaps later)\n\
\n\
##### bullets\n\
	# what kind of bullets are displayed before each item\n\
	##### samples\n\
		#bullet_parent   -\n\
		#bullet_parent   *\n\
		#bullet_parent   ·\n\
		#bullet_parent_expanded   +\n\
		#bullet_parent_expanded   *\n\
		#bullet_parent_expanded   ·\n\
		#bullet_leaf   *\n\
		#bullet_leaf   -\n\
	bullet_leaf   ·\n\
	bullet_parent   +\n\
	bullet_parent_expanded   -\n\
	\n\
##### user interface\n\
	##### display format\n\
		# defining how each node is displayed, the display string syntax is\n\
		# interpreted as follows:\n\
		#\n\
		#   spaces turn into real spaces, i means indentation, - means bullet,\n\
		#   d means the real data of the node, x is a temporary placeholder for\n\
		#   upcoming column-types,. (for debugging only)\n\
		#\n\
		#   i and x can take an argument specifying how many characters wide\n\
		#   the field should be\n\
		#\n\
		#   until further features are added, only the multiplier after\n\
		#   indentation actually does something\n\
		#   # indicates node number (actually line number when used as a folding editor)\n\
		##### samples\n\
			#display_format # i4- d\n\
		display_format i4- d\n\
		\n\
	##### fixed focus\n\
		# lock the selection bar to a position 1/3 down the screen\n\
		fixedfocus	0\n\
		\n\
	\n\
##### external commands\n\
	##### http:// handler\n\
		# what command is executed when a user wants a action performed on a\n\
		# entry containing an URL\n\
		# \n\
		# * is substituted for the URL\n\
		#\n\
		# the X ones have their error and standard output redirected to dev/null\n\
		# and are put into the background to keep hnb running afterwards\n\
		##### samples\n\
			#web_command opera -remote 'openURL(*)'			> /dev/null 2>&1 &\n\
			#web_command netscape -remote 'openURL(*)'		> /dev/null 2>&1 &\n\
			#web_command netscape -remote 'openURL(*,new-window)'	> /dev/null 2>&1 &\n\
			#web_command mozilla -remote 'openURL(*)'		> /dev/null 2>&1 &\n\
			#web_command mozilla -remote 'openURL(*,new-window)'	> /dev/null 2>&1 &\n\
			#web_command konqueror *				> /dev/null 2>&1 &\n\
			#web_command galeon -n					> /dev/null 2>&1 &\n\
			#web_command w3m *\n\
			#web_command lynx *\n\
			#web_commend rxvt -fn fixed -rv +sb -e w3m *		> /dev/null 2>&1 &\n\
		web_command opera -remote 'openURL(*,new-window)'	> /dev/null 2>&1 &\n\
		\n\
	##### mail@handler\n\
		# what command is executed when a user wants a action performed on a\n\
		# entry containing a mail-address\n\
		# \n\
		# * is substituted for the mail-address\n\
		#\n\
		## samples\n\
			#mail_command xterm -rv pine *				> /dev/null 2>&1 &\n\
			#mail_command xterm -rv +sb -e mutt *			> /dev/null 2>&1 &\n\
			#mail_command pine *\n\
		mail_command mutt *\n\
		\n\
	##### spell checking\n\
		# the spell-checker used\n\
		#\n\
		## samples\n\
			#spell_command aspell -c\n\
			#spell_command ispell -x\n\
			#spell_command ispell -x -damerican\n\
			#spell_command ispell -x -dbritish\n\
			#spell_command ispell -x -dbokmål\n\
			#spell_command ispell -x -dnynorsk\n\
		spell_command ispell -x\n\
		\n\
	\n\
##### file formats\n\
	##### Default Format\n\
		# 0 - XML using hnb DTD\n\
		# 1 - tab indented ASCII\n\
		# 2 - XML using no DTD (working as XML editor)\n\
		# 3 - XML using OPML DTD\n\
		format 0\n\
		\n\
	##### Remember current position in tree\n\
		# wheter hnb remembers the current position in a file, (currently\n\
		# supported by the hnb DTD only, but might easily be extended since\n\
		# it fakes it with a XML comment) 1=true 0=false\n\
		savepos	1\n\
		\n\
	##### XML format\n\
		##### XML Cuddling\n\
			# When loading xml files, should data be allowed on \n\
			# the same line as tags?\n\
			xml_cuddle 1\n\
			\n\
		\n\
	\n\
##### Color/style\n\
	# color scheme configuration, syntax is:\n\
	# \"style <item>  <foreground/background> <attribute,[attribute]>\"\n\
	# legal items are shown below\n\
	# colors are: blue, cyan, green, red, magenta, yellow, black and white\n\
	# attributes are: normal, reverse, bold, underline\n\
	\n\
	style menuitem       default/default reverse\n\
	style menutext       cyan/default    normal\n\
	style parentnode     default/default bold\n\
	style node           default/default normal\n\
	style bullet         cyan/default    normal\n\
	style selected       yellow/red      bold\n\
	style parentselected yellow/red      bold\n\
	style background     default/default normal\n\
	\n\
##### key-bindings\n\
	#\n\
	# syntax: \n\
	#   \"bind <scope[,scope[...]]>  <key>   <action>\"\n\
	#\n\
	# multiple actions can be chained together by using the virtual key \"..\"\n\
	# as the following key-binding\n\
	#\n\
	##################################################\n\
	\n\
	context main\n\
		helptext <ESC>| exit || |F2| save || |F5| commandline || |return| edit || | / | find || |^S| ort\n\
		\n\
		bind f1 status Take a look at ~/.hnbrc to check out the configuration\n\
		bind .. status there you will also find details about the available keybindings.\n\
		bind .. status please feel free to contribute more useful keybindings and helptexts...\n\
		\n\
		bind esc context quitsave\n\
		\n\
		bind f5 commandline\n\
		bind .. status_clear\n\
		\n\
		bind f6 context spellcheck\n\
		bind .. save_state\n\
		bind .. spell\n\
		\n\
		bind ^E context export\n\
		bind ^F context search\n\
		bind .. save_state\n\
		bind .. getquery enter keyword to search for\n\
		bind .. next_match\n\
		\n\
		bind f10 context user\n\
		\n\
		bind ^G context grab\n\
		bind .. save_state\n\
		\n\
		bind ^A action\n\
		\n\
		bind up    up\n\
		bind down  down\n\
		bind left  left\n\
		bind right right\n\
		bind npage pagedown\n\
		bind ppage pageup\n\
		bind home  go_root\n\
		\n\
		bind return edit\n\
		bind del    remove\n\
		\n\
		bind ins    insert_below\n\
		bind .. edit\n\
		\n\
		bind tab complete\n\
		bind ^T  toggle_todo\n\
		bind ^D  toggle_done\n\
		bind >   indent\n\
		bind <   outdent\n\
		bind f2  save\n\
		\n\
		bind ^X save_state\n\
		bind .. cut\n\
		\n\
		bind ^C copy\n\
		\n\
		bind ^V paste\n\
		bind .. down\n\
		\n\
		bind ^S save_state\n\
		bind .. sort\n\
		\n\
		bind backspace	backspace\n\
		bind backspace2	backspace\n\
		\n\
		bind sleft movenode left\n\
		bind sright	 movenode right\n\
		bind sprevious movenode up\n\
		bind snext movenode down\n\
		\n\
		bind + expand\n\
		bind - collapse\n\
		\n\
		bind ^Z restore_state\n\
		\n\
	context nodeedit\n\
		helptext editing node |  |ESC| revert || |return| confirm\n\
		bind up    up\n\
		bind down  down\n\
		bind left  left\n\
		bind right right\n\
		\n\
		bind return  confirm\n\
		bind esc     cancel\n\
		\n\
		bind ^S  split\n\
		bind ^J  join\n\
		\n\
		bind backspace  backspace\n\
		bind backspace2 backspace\n\
		\n\
		bind ^A   bol\n\
		bind home bol\n\
		bind ^E   eol\n\
		bind end  eol\n\
		\n\
		bind del  delete\n\
		\n\
	context lineedit\n\
		helptext lineedit\n\
		\n\
		bind up      up\n\
		bind down    down\n\
		bind left    left\n\
		bind right   right\n\
		\n\
		bind return  confirm\n\
		bind esc     cancel\n\
		bind tab     complete\n\
		\n\
		bind backspace backspace\n\
		bind backspace2 backspace\n\
		bind del        delete\n\
		\n\
		bind ^A   bol\n\
		bind home bol\n\
		bind ^E   eol\n\
		bind end  eol\n\
		\n\
	context confirm\n\
		helptext  y | | j | |return| confirm || |any key| cancel\n\
		\n\
		bind y      confirm\n\
		bind j      confirm\n\
		bind return confirm\n\
		\n\
		bind any    cancel\n\
		\n\
	context quitsave\n\
		helptext Save on exit? |    | Y |es | J |a || | N |o || |ESC| cancel\n\
		bind Y  save\n\
		bind .. quit\n\
		\n\
		bind y  save\n\
		bind .. quit\n\
		\n\
		bind J  save\n\
		bind .. quit\n\
		\n\
		bind j  save\n\
		bind .. quit\n\
		\n\
		bind N  quit\n\
		bind n  quit\n\
		\n\
		bind esc context main\n\
		bind any unbound\n\
		\n\
	context spellcheck\n\
		helptext Spellchecking |   |Space| next node |ESC| cancel |F6,Return| confirm changes\n\
		\n\
		bind space go_recurse\n\
		bind ..    spell\n\
		\n\
		bind down go_recurse\n\
		bind ..   spell\n\
		\n\
		bind up   go_backrecurse\n\
		bind ..   spell\n\
		\n\
		bind return context main\n\
		bind f6 context main\n\
		\n\
		bind esc restore_state\n\
		bind .. context main\n\
		\n\
		bind any unbound\n\
	context grab\n\
		helptext Node grabbed |  |return| |space| drop here || |ESC| cancel\n\
		bind esc    restore_state\n\
		bind ..     context main\n\
		bind return context main\n\
		bind space  context main\n\
		bind left   movenode left\n\
		bind right  movenode right\n\
		bind up     movenode up\n\
		bind down   movenode down\n\
		bind any    unbound\n\
	context export\n\
		helptext Export |  | A |scii || | H |tml  html| C |ss  | P |ostscript  |ESC| cancel\n\
		\n\
		bind esc\n\
			bind .. context main\n\
		bind A \n\
			bind .. getquery what file should I save the ASCII output in?\n\
			bind .. export_ascii *\n\
			bind .. context main\n\
		bind a\n\
			bind .. getquery what file should I save the ASCII output in?\n\
			bind .. export_ascii *\n\
			bind .. context main\n\
		bind H\n\
			bind .. getquery what file should I save the HTML output in?\n\
			bind .. export_html *\n\
			bind .. context main\n\
		bind h\n\
			bind .. getquery what file should I save the HTML output in?\n\
			bind .. export_html *\n\
			bind .. context main\n\
		bind C\n\
			bind .. getquery what file should I save the HTML/CSS output in?\n\
			bind .. export_htmlcss *\n\
			bind .. context main\n\
		bind c\n\
			bind .. getquery what file should I save the HTML/CSS output in?\n\
			bind .. export_htmlcss *\n\
			bind .. context main\n\
		bind P\n\
			bind .. getquery what file should I save the Postscript output in?\n\
			bind .. export_ps *\n\
			bind .. context main\n\
		bind p\n\
			bind .. getquery what file should I save the Postscript output in?\n\
			bind .. export_ps *\n\
			bind .. context main\n\
		\n\
		bind any unbound\n\
	context search\n\
		helptext Searching |  |ESC| cancel || | n |ext || | p |revious || |space|/|return| go\n\
		bind down   next_match	\n\
		bind up     prev_match		\n\
		bind P      prev_match\n\
		bind p      prev_match\n\
		bind n      next_match\n\
		bind N      next_match\n\
		\n\
		bind return context main\n\
		\n\
		bind esc    context main\n\
		bind ..     restore_state\n\
		\n\
		bind any    unbound\n\
	context user\n\
		helptext Usermenu | |ESC| cancel |F1| ispell language |F2| browser command |F3| expand all  |F4|  collapse all\n\
		bind esc context main\n\
		\n\
		bind f1  context user_ispell\n\
		bind f2  context user_browser\n\
		bind f3  expand_all\n\
		bind ..  context main\n\
		\n\
		bind f4  collapse_all\n\
		bind ..  context main\n\
		\n\
		bind any unbound\n\
		\n\
	context user_ispell\n\
		helptext Language for ispell: | |F1| Br.english  |F2| Am.english  |F3| Norwegian bokmål  |F4| Norwegian nynorsk\n\
		bind f1  spell_command ispell -x -damerican\n\
		bind ..  context main\n\
		\n\
		bind f2  spell_command ispell -x -dbritish\n\
		bind ..  context main\n\
		\n\
		bind f3  spell_command ispell -x -dbokmål\n\
		bind ..  context main\n\
		\n\
		bind f4  spell_command ispell -x -dnynorsk\n\
		bind ..  context main\n\
		\n\
		bind esc context main\n\
		bind any unbound\n\
		\n\
	context user_browser\n\
		helptext Browser action: | |F1| w3m replace hnb  |F2| w3m in rxvt  |F3| Opera, new tab  |F4| Mozilla new window\n\
		bind f1 browser_command w3m *\n\
		bind .. context main\n\
		\n\
		bind f2 web_commend rxvt -T \"w3m spawned from hnb\" -fn fixed -rv +sb -e w3m * > /dev/null 2>&1 &\n\
		bind .. context main\n\
		\n\
		bind f3 web_command opera -remote 'openURL(*,new-window)' > /dev/null 2>&1 &\n\
		bind .. context main\n\
		\n\
		bind f4 web_command mozilla -remote 'openURL(*,new-window)'	> /dev/null 2>&1 &\n\
		bind .. context main\n\
		\n\
		bind esc context main\n\
		bind any unbound\n\
	\n\
		\n\
			\n\
			\n\
	##### keyrelated options\n\
		##### escdelay\n\
			# how long the program should wait before accepting that ESC(27d) is the\n\
			# ESC key and not the start of an escape sequence\n\
			# increase this number if you are using hnb over a slow network link\n\
			# value given is in ms (1/1000seconds)\n\
			#\n\
			escdelay 100\n\
#####\n\
	context main\n\
	status hnb - hierarchical notebook   http://hnb.sourceforge.net/\n\
	status (c) Øyvind Kolås 2001-2003    pippin@users.sourceforge.net\n\
"
