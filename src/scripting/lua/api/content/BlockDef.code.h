struct Diggler_Content_BlockDef {
  struct {
    int variabilty;
    int texturesCount;
    struct Diggler_Content_BlockDef_appearance_texture {
      const char *name, *path;
      uint8_t repeatXdiv, repeatYdiv;
    } *textures;
    struct {
      enum {
        LookType_Hidden = 0,
        LookType_Cube = 1
      } type;
      union {
        struct {
          struct {
            int texture;
          } sides[6];
        } cube;
      } data;
    } look;
  } appearance;
};
