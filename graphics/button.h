#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Button {
    public:

    Button(int posx, int posy, int size_x, int size_y, const char *text, int font_size=15);     // initialize button. Position is top left corner

    void createSurface();   // generate the surface used to draw the button

    void draw(SDL_Renderer* renderer);  // draw button to screen

    void checkClick(int mouse_x, int mouse_y);  // check if button is clicked

    void checkSelect(int mouse_x, int mouse_y); // check if button is selected
    
    int pos[2]; int size[2];
    const char *text; int font_size;
    TTF_Font *font;
    SDL_Surface *button_surf;
    bool selected;
    void (*onClick_func)();
};
