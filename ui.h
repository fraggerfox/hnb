#define UI_UP		401
#define UI_DOWN		402
#define UI_LEFT		403
#define UI_RIGHT	404
#define UI_TOP		405
#define UI_BOTTOM	406
#define UI_REMOVE	408
#define UI_QUIT		409
#define UI_ENTER		412
#define UI_ESCAPE		413
#define UI_BACKSPACE	414
#define UI_MARK			415
#define UI_MENU			416
#define UI_MODE_NAVIGATE 0
#define UI_MODE_EDIT	 1
#define UI_MODE_MARKED	 2
#define UI_MODE_QUIT	 3
#define UI_MODE_MENU	 4


void ui_init();
void ui_draw(Node *node,char *input, int mode);
void ui_end();
int ui_input();
