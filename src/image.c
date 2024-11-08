/*
 * Copyright (c) 2024 ogasawara futo <pik6cs@gmail.com>
 * This software is licensed under the MIT License.
 */
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <id3tag.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    void* image_data;
    size_t image_size;
} ThreadData;

static void* displayCoverImageThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    SDL_Window *window = SDL_CreateWindow("Cover Image", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        300, 300, 
        SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        free(data);
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        free(data);
        return NULL;
    }

    // Create RWops from memory
    SDL_RWops *rw = SDL_RWFromConstMem(data->image_data, data->image_size);
    if (!rw) {
        fprintf(stderr, "SDL_RWFromConstMem Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        free(data);
        return NULL;
    }

    // Load image from RWops
    SDL_Surface *image_surface = IMG_Load_RW(rw, 1);
    if (!image_surface) {
        fprintf(stderr, "IMG_Load_RW Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        free(data);
        return NULL;
    }

    // Create texture from surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        SDL_FreeSurface(image_surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        free(data);
        return NULL;
    }

    // Get image dimensions
    int img_w = image_surface->w;
    int img_h = image_surface->h;
    SDL_FreeSurface(image_surface);

    // Main loop
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
               // quit = 1;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                 //   quit = 1;
                }
            }
        }

        // Clear renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Calculate aspect ratio preserving rectangle
        int win_w, win_h;
        SDL_GetWindowSize(window, &win_w, &win_h);
        double win_aspect = (double)win_w / win_h;
        double img_aspect = (double)img_w / img_h;

        SDL_Rect dst_rect;
        if (win_aspect > img_aspect) {
            dst_rect.h = win_h;
            dst_rect.w = (int)(win_h * img_aspect);
            dst_rect.x = (win_w - dst_rect.w) / 2;
            dst_rect.y = 0;
        } else {
            dst_rect.w = win_w;
            dst_rect.h = (int)(win_w / img_aspect);
            dst_rect.x = 0;
            dst_rect.y = (win_h - dst_rect.h) / 2;
        }

        // Render the image
        SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap at ~60 FPS
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    free(data->image_data);
    free(data);
    return NULL;
}

void displayCoverImage(void* image_data, size_t image_size) {
    ThreadData* data = malloc(sizeof(ThreadData));
    if (!data) {
        fprintf(stderr, "Failed to allocate thread data\n");
        return;
    }
    data->image_data = image_data;
    data->image_size = image_size;

    pthread_t thread_id;
    int result = pthread_create(&thread_id, NULL, displayCoverImageThread, data);
    if (result != 0) {
        fprintf(stderr, "Failed to create thread: %d\n", result);
        free(data);
        return;
    }

    pthread_detach(thread_id);
}



void getCoverImage(const char *filename) {
    struct id3_file *file = id3_file_open(filename, ID3_FILE_MODE_READONLY);
    
    if (file == NULL) {
        fprintf(stderr, "Failed to open MP3 file: %s\n", filename);
        return;
    }

    struct id3_tag *tag = id3_file_tag(file);
    if (tag == NULL) {
        fprintf(stderr, "Failed to read ID3 tag from file: %s\n", filename);
        id3_file_close(file);
        return;
    }

    struct id3_frame *frame = id3_tag_findframe(tag, "APIC", 0);
    
    if (frame) {
        
        id3_length_t image_size;
        const id3_byte_t *image_data = id3_field_getbinarydata(&frame->fields[4], &image_size);
        
        if (image_data && image_size > 0) {
            
            void *image_copy = malloc(image_size);
            if (image_copy) {
                memcpy(image_copy, image_data, image_size);
                displayCoverImage(image_copy, image_size);
            } else {
                fprintf(stderr, "Failed to allocate memory for image data\n");
            }
        } else {
            fprintf(stderr, "No image data found in APIC frame\n");
        }
    } else {
    }
    
    id3_file_close(file);
}



