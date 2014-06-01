press right arrow to begin
	Welcome to 'hierachical notebook'
	
	Navigate using your arrow keys, right takes you a level further down in the tree, left brings you up a level, up and down navigates amongst the siblings.
	An entry followed by two dots have subentries.
	
	usage documentation
		In the bottom of your terminal window you see a list of allowed commands, a letter with a caret (^) in front means that you should press control and the letter.
		pressing F1 cycles two different help screens
		
		navigation
			you navigate the tree by using the arrow keys.
			pageup and pagedown skips up and down by 10 items.
			by starting to type the start of an sibling entry, the focus will skip to a node beginning with those characters, pressing tab will select that node, tabbing again will go to the sublevel, just like filename completion in a unix shell.
			to find a node press ctrl+f, enter a search term, and follow the instructions at the bottom of the screen.
		data entry/modification
			to insert a node at a location, press insert, and a new node will appear below the current node.
			pressing enter will edit the current node.
			the entry completion feature explained in navigation will also insert a new node if you press enter after typing something that is not already exist as a node, the new node will be inserted as the bottommost sibling at the current level.
		todo lists
			pressing ctrl+t will toggle a checkmark to the left of the current entry
			new nodes created below this node will also have a checkmark.
			pressing ctrl+d will check the checkbox, 
			the upperlevel todo items will be checked if all the sublevel items are checked off, if just some sublevels are checked off a percentage will be shown insted of the checkmark.
		removing nodes
			pressing delete or backspace while navigating will remove
			a node, if the node has children you must confirm the
			deletion
		exporting to other formats
			pressing ctrl+e will bring up the export menu
			it will write the siblings and sublevels of the current node to a file.
			the most useful is perhaps the html export mode, hnb itself doesn't sport a nice printing interface, but the html can be printed nicely from an webbrowser.
		saving/quitting
			pressing [esc] or ctrl+x will give you the choices to,
			save
			quit
			save and quit,..
			amongst other options
		command line parameters
			usage: hnb [options] [file]
			 
			if file is omitted hnb will edit the default database
			you can find out which file is the default file
			by using the help option
			 
			OPTIONS:
			  -h, --help    - display usage and options
			  -v, --version - display the version of hnb
	sample data:
		books in my bookshelf
			Aldous Huxley, Brave New World
			Conan Doyle, The Adventures of Sherlock Holmes
			Edgar Allan Poe, Selected works
			Geogre Orwell, Animal Farm
			Harper Lee, To Kill a Mockingbird
			J R R Tolkien, The Lord Of The Rings
				Three Rings for the Eleven-kings under the sky
				  Seven for the Dwarf-lords in their halls of stone,
				Nine for Mortal Men doomed to die,
				  One for the Dark Lord on his dark throne
				In the Land of Mordor where shadows lie.
				  One Ring to rule them all, One Ring to find them,
				  One Ring to bring them all and in the darkness bind them
				In the Land of Mordor where shadows lie.
			Jack London, Livsgnisten og andre noveller
			Knut Hamsun, Pan
			Language Myths
			Milton, Paradise Lost
				Rivers in hell
					Abhorred Styx, the flood of deadly hate
					Sad Acheron of sorrow, black and deep;
					Cocytus , named of lamentation loud
					Heard on the rueful stream; fierce Phlegethon,
					Whose waves of torrent fire inflame with rage.
					Far off from these, a slow and silent stream,
					Lethe, the river of oblivion, rolls
					Her watery labyrinth, whereof who drinks
					Forthwith his former state and being forgets,
					Forgets both joy and grief, pleasure and pain.
				'famous quote' (in sandman among others),..
					The mind is it's own place, and in itself
					Can make a Heaven of Hell, a Hell of Heaven.
					What matter where, if I be still the same,
					And what I should be, all but less than he
					Whom thunder hath made greater? Here at least
					We shall be free; the Almighty hath not built
					Here for his envy, will not drive us hence;
					Here we may reign secure, and, in my choice,
					To reign is worth ambition, though in Hell:
					Better to reign in Hell than serve in Heaven.
			Oscar Wilde, The Picture of Dorian Gray
			Philip K. Dick, Do Androids Dream of Electronic Sheep (Bladerunner)
			Pokus 1
			Pokus 2
			Tarjei Vesaas, Fuglane
		cd's in my collection
			none, it was stolen,
			I surive on mp3's at the moment
		[ ]packing for the journey there and back again
			[ ]clothes
				[X]4x underwear
				[ ]extra trousers
				[X]3 shirts
				[ ]winter clothes
				[ ]extra pair of whoes
			[ ]stuff
				[ ]music
				[ ]reading material
				[X]money
	
	This data was loaded, because you didn't specify any database to load, and no default database was wound in your home directory.
