#ifndef __HEADER_FILE_GUI_h__
#define __HEADER_FILE_GUI_h__


u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonWithState(u16 x1,u16 y1,u16 x2,u16 y2, unsigned char state);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK16_Spec(u16 x, u16 y, u16 fc, u16 bc, u8* s);
//void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num) ;
//void Gui_DrawFont_ASC24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_ASC16Ex(u16 x, u16 y, u16 fc, u16 bc, u8 *s, char opposite);
void Gui_Fill(u16 x, u16 y, u16 w, u16 h, u16 color);


#define BUTTON_STATE_DOWN (1)
#define BUTTON_STATE_UP (0)


// RGB565 Begin
#ifdef __cplusplus
/*inline unsigned short RGB6toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned short color = 0;
    color |= (red >> 3) << 11;      //?red??5???color??5?
    color |= (green >> 2) << 5;     //?green??6???color???6?
    color |= blue >> 3;             //?blue??5???color??5?
    return color;
}*/
#define RGB6toRGB565(r, g, b) (((r >> 3) << 11) | ((g >> 2) <<5) | ((b >>3)))
#endif
// RGB565 End





#endif
