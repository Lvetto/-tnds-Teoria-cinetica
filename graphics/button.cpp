#include "button.h"

Button::Button(int pos_x, int pos_y, int size_x, int size_y, const char *text, int font_size) {
    this->pos[0] = pos_x;
    this->pos[1] = pos_y;

    this->size[0] = size_x;
    this->size[1] = size_y;

    this->font_size = font_size;
    this->text = text;

    createSurface();

    this->onClick_func = nullptr;
    this->selected = false;
}

void Button::createSurface(){
    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", font_size);     // load default font
    if (!font) {
        SDL_Log("Impossibile caricare il font: %s", SDL_GetError());
        return;
    }

    // render text
    SDL_Color color = {255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);

    // make sure button is big enough for the text
    int text_height = textSurface->h;
    int text_width = textSurface->w;
    if (text_width > size[0])   size[0] = text_width + 10;
    if (text_height > size[1])   size[1] = text_height + 10;
    
    // compute coordinates to make text centered
    int tx = static_cast<int>((size[0] - text_width) / 2);
    int ty = static_cast<int>((size[1] - text_height) / 2);
    SDL_Rect text_rect = {tx, ty, text_width, text_height};

    // actually draw the button
    this->button_surf = SDL_CreateRGBSurface(0, size[0], size[1], 32, 0, 0, 0, 0);
    SDL_Rect rect1 = {0, 0, size[0], size[1]};
    SDL_Rect rect2 = {5, 5, size[0]-10, size[1]-10};
    SDL_FillRect(this->button_surf, &rect1, SDL_MapRGB(this->button_surf->format, 120, 120, 120));
    SDL_FillRect(this->button_surf, &rect2, SDL_MapRGB(this->button_surf->format, 150, 150, 150));

    SDL_BlitSurface(textSurface, NULL, this->button_surf, &text_rect);

    // clean up
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

}

void Button::draw(SDL_Renderer* renderer) {

    // if the button is selected, draw a small outline around it
    if (selected) {
        SDL_Rect rect = {pos[0]-1, pos[1]-1, size[0]+2, size[1]+2};
        SDL_SetRenderDrawColor(renderer, 52, 207, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // draw button surface on window in the right location
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, button_surf);   // maybe move to CreateSurface?
    SDL_Rect dest = {pos[0], pos[1], size[0], size[1]};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &dest);

    // clean up
    SDL_DestroyTexture(buttonTexture);

}

void Button::checkClick(int mouse_x, int mouse_y) {
    // if button has a function assigned, check if button is clicked
    if (onClick_func) {
        // check if mouse x and y is in the right interval
        if ((mouse_x >= pos[0]) and (mouse_x <= pos[0] + size[0])) {
            if ((mouse_y >= pos[1]) and (mouse_y <= pos[1] + size[1])) {
                onClick_func();
            }
        }
    }
}

void Button::checkSelect(int mouse_x, int mouse_y) {
    // check if mouse x and y is in the right interval
    if ((mouse_x >= pos[0]) and (mouse_x <= pos[0] + size[0])) {
            if ((mouse_y >= pos[1]) and (mouse_y <= pos[1] + size[1])) {
                this->selected = true;
                return;
            }
        }
    this->selected = false;     // unreacheable if button is selected
}
