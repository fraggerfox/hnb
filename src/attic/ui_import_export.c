#include "tree.h"
#include "ui.h"
#include "file.h"
#include "prefs.h"
#include "ui_cli.h"
#include "ui_import_export.h"

#define BUFFERLENGTH 4096

Node *ui_export (Node *pos)
{
	int c, stop = 0;
	int format = format_hnb;
	char filename[255];

	while (!stop) {
		ui_draw (pos, pos, "", ui_mode_export);
		c = parsekey (ui_input (), ui_scope_export);
		switch (c) {
			case 'h':
				format = format_hnb;
				stop = 1;
				break;
			case '?':
				format = format_help;
				stop = 1;
				break;
			case 'H':
				format = format_html;
				stop = 1;
				break;
			case 'a':
				format = format_ascii;
				stop = 1;
				break;
			case 'x':
				format = format_xml;
				stop = 1;
				break;
			case 's':
#if 0
				format = format_sxml;
				stop = 1;
#endif
				break;
/*			case k_export_pipe:
				strcpy ((char *) filename, "command line (%s for file):");
				ui_draw (pos,pos, (char *) filename, ui_mode_getstr);
				if (!strlen (filename))
					return pos;
				ui_end();
				ascii_export(node_top(pos), "hnb.tmp.2.txt");
				{
					char cmd_buf[400];
					sprintf(cmd_buf,filename,"hnb.tmp.2.txt");
					system(cmd_buf);
				}
				ui_init();
				stop=1;
				break;*/
			case ui_action_cancel:
				stop = 2;
				break;
			default:
				undefined_key ("export", c);
		}
	}

	if (stop == 1) {
		sprintf (filename, "%s file to export:", format_name[format]);
		ui_draw (pos, pos, (char *) filename, ui_mode_getstr);
		if (strlen (filename)) {
			char buf[4096];

			sprintf (buf, "export_%s %s", format_name[format], filename);
			pos = docmd (pos, buf);
		}
	}

	ui_draw (pos, pos, " ", ui_mode_help0 + prefs.help_level);
	if (file_error[0])
		info (file_error);

	return pos;
}

Node *ui_import (Node *pos)
{
	int c, stop = 0;
	char filename[100];
	int format = format_hnb;

	while (!stop) {
		ui_draw (pos, pos, "", ui_mode_import);

		c = parsekey (ui_input (), ui_scope_import);
		switch (c) {
			case 'h':
				format = format_hnb;
				stop = 1;
				break;
			case 'a':
				format = format_ascii;
				stop = 1;
				break;
			case 'x':
				format = format_xml;
				stop = 1;
				break;
			case 's':
#if 0
				format = format_sxml;
				stop = 1;
#endif
				break;
			case ui_action_cancel:
				stop = 2;
				break;
			default:
				undefined_key ("import", c);
		}
	}

	if (stop == 1) {
		sprintf (filename, "%s file to import:", format_name[format]);
		ui_draw (pos, pos, (char *) filename, ui_mode_getstr);
		if (strlen (filename)) {
			char buf[4096];

			sprintf (buf, "import_%s %s", format_name[format], filename);
			pos = docmd (pos, buf);
		}
	}
	return pos;
}
