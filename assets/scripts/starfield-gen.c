/*
 * Compile
 * =======
 *
 * Make sure raylib is installed on you system.
 *
 * gcc -o starfield-gen starfield-gen.c -lraylib -lm -lpthread -ldl -lrt -lX11
 *
 * How to use
 * ==========
 * ./starfield-gen -i "tileset.png" -s 16 -w 9 -h 1 -o "starfield.png" -u 4096
 * -v 4096
 */

#include <argp.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_TILESET_PATH "tileset.png"
#define DEFAULT_TILE_SIZE 16
#define DEFAULT_TILESET_WIDTH 4
#define DEFAULT_TILESET_HEIGHT 1

#define DEFAULT_DENSITY 2

#define DEFAULT_IMAGE_PATH "starfield.png"
#define DEFAULT_IMAGE_WIDTH 256
#define DEFAULT_IMAGE_HEIGHT 256

struct sg_arguments_s {
  char *tileset_path;
  int tile_size;
  int tileset_width;
  int tileset_height;

  char *image_path;
  int star_density;
  int image_width;
  int image_height;
};

const char *sg_version = "starfield-gen 1.0";
static char sg_purpose[] = "\nGenerate a starfield image using a "
                           "tileset.\n\nNOTE: the first tile of the tileset "
                           "should be blank (no star).";
static struct argp_option sg_options[] = {
    {"tileset", 'i', "FILE", 0,
     "Path to the tileset image (default: tileset.png)"},
    {"size", 's', "SIZE", 0, "Size of a tile (default: 16)"},
    {"tileset_width", 'w', "WIDTH", 0,
     "Number of tiles horizontally in the tileset (default: 4)"},
    {"tileset_height", 'h', "HEIGHT", 0,
     "Number of tiles vertically in the tileset (default: 1)"},

    {"image", 'o', "FILE", 0,
     "Path to the generated image image (default: starfield.png)"},
    {"star_density", 'd', "DENSITY", 0, "the star density (default: 2)"},
    {"image_width", 'u', "WIDTH", 0,
     "Width in pixel of the image (default: 256)"},
    {"image_height", 'v', "HEIGHT", 0,
     "Height in pixel of the image (default: 256)"},
    {0}};

static error_t sg_parse_opt(int key, char *arg, struct argp_state *state) {
  struct sg_arguments_s *arguments = (struct sg_arguments_s *)state->input;

  int star_density = 0;

  switch (key) {
  case 'i':
    arguments->tileset_path = arg;
    break;
  case 's':
    arguments->tile_size = atoi(arg);
    break;
  case 'w':
    arguments->tileset_width = atoi(arg);
    break;
  case 'h':
    arguments->tileset_height = atoi(arg);
    break;

  case 'o':
    arguments->image_path = arg;
    break;
  case 'd':
    star_density = atoi(arg);

    if (star_density < 1)
      star_density = 1;

    arguments->star_density = star_density;
    break;
  case 'u':
    arguments->image_width = atoi(arg);
    break;
  case 'v':
    arguments->image_height = atoi(arg);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp sg_argp = {sg_options, sg_parse_opt, NULL, sg_purpose};

/*
 * Generate an 2D array of tiles position and choose one.
 * Return the tile source rectangle of a random tile within a tileset.
 */
Rectangle sg_get_rand_tile(int tile_size, int tileset_width,
                           int tileset_height) {
  int star_tiles[tileset_width * tileset_height][2];
  int index = 0;
  for (int y = 0; y < tileset_height; ++y) {
    for (int x = 0; x < tileset_width; ++x) {
      star_tiles[index][0] = x;
      star_tiles[index][1] = y;
      index++;
    }
  }

  index = rand() % (tileset_width * tileset_height);
  return (Rectangle){star_tiles[index][0] * tile_size,
                     star_tiles[index][1] * tile_size, tile_size, tile_size};
}

int main(int argc, char **argv) {
  struct sg_arguments_s arguments;

  arguments.tileset_path = DEFAULT_TILESET_PATH;
  arguments.tile_size = DEFAULT_TILE_SIZE;
  arguments.tileset_width = DEFAULT_TILESET_WIDTH;
  arguments.tileset_height = DEFAULT_TILESET_HEIGHT;

  arguments.image_path = DEFAULT_IMAGE_PATH;
  arguments.star_density = DEFAULT_DENSITY;
  arguments.image_width = DEFAULT_IMAGE_WIDTH;
  arguments.image_height = DEFAULT_IMAGE_HEIGHT;

  argp_parse(&sg_argp, argc, argv, 0, 0, &arguments);

  InitWindow(1, 1, "");

  Texture2D tileset = LoadTexture(arguments.tileset_path);

  srand(time(NULL));

  RenderTexture2D target =
      LoadRenderTexture(arguments.image_width, arguments.image_height);

  BeginTextureMode(target);

  ClearBackground(BLACK);

  for (int y = 0; y < arguments.image_height; y += arguments.tile_size) {
    for (int x = 0; x < arguments.image_width; x += arguments.tile_size) {
      if (rand() % arguments.star_density == 0) {
        Rectangle src =
            sg_get_rand_tile(arguments.tile_size, arguments.tileset_width,
                             arguments.tileset_height);
        Rectangle dest = {x, y, arguments.tile_size, arguments.tile_size};
        DrawTexturePro(tileset, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
      } else {
        Rectangle src = {0, 0, arguments.tile_size, arguments.tile_size};
        Rectangle dest = {x, y, arguments.tile_size, arguments.tile_size};
        DrawTexturePro(tileset, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
      }
    }
  }

  EndTextureMode();

  Image starfield = LoadImageFromTexture(target.texture);

  ExportImage(starfield, arguments.image_path);

  UnloadRenderTexture(target);
  UnloadTexture(tileset);

  CloseWindow();

  return 0;
}
