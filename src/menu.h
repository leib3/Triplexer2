
typedef const struct MenuStruct
{  const char * text;
   unsigned char num_menupoints;
   unsigned char up;
   unsigned char down;
   unsigned char enter;
   void ( *fp ) ( void );
} MenuEntry;
