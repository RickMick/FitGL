#include "Loader.h"

#include <iostream>

#ifdef USE_FREEIMAGE
#include <FreeImagePlus.h>
#endif

#ifdef USE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // USE_STB_IMAGE


#include <geGL/StaticCalls.h>
using namespace ge::gl;
using namespace fgl;

bool Loader::generateOnImageNotFound = true;

std::shared_ptr<ge::gl::Texture> Loader::texture(std::string const & fileName, bool generateMipmap,
  GLenum filterMin, GLenum filterMag, GLenum wrapR, GLenum wrapS) {
  auto img = image(fileName);
  auto tex = std::make_shared<ge::gl::Texture>();
  tex->create(GL_TEXTURE_2D, GL_RGBA, 0, img->width, img->height, 0);
  tex->setData2D(img->data, img->format, GL_UNSIGNED_BYTE);
  if(generateMipmap)tex->generateMipmap();
  tex->texParameteri(GL_TEXTURE_MIN_FILTER, filterMin);
  tex->texParameteri(GL_TEXTURE_MAG_FILTER, filterMag);
  tex->texParameteri(GL_TEXTURE_WRAP_R, wrapR);
  tex->texParameteri(GL_TEXTURE_WRAP_S, wrapS);
  return tex;
}

std::shared_ptr<ge::gl::Texture> Loader::cubemap(std::string const(&fileNames)[6], bool generateMipmap, GLenum filterMin, GLenum filterMag, GLenum wrapR, GLenum wrapS) {
  auto tex = std::make_shared<ge::gl::Texture>();


  ImageS img[6];
  for (int i = 0; i < 6; i++) {
    img[i] = image(fileNames[i]);
  }
  
  tex->create(GL_TEXTURE_CUBE_MAP, GL_RGBA, 0, img[0]->width, img[0]->height);

  for (int i = 0; i < 6; i++) {
    tex->setData2D(img[i]->data, GL_BGRA, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,0,img[i]->width,img[i]->height);
  }

  if (generateMipmap) tex->generateMipmap();
  tex->texParameteri(GL_TEXTURE_MIN_FILTER, filterMin);
  tex->texParameteri(GL_TEXTURE_MAG_FILTER, filterMag);
  tex->texParameteri(GL_TEXTURE_WRAP_R, wrapR);
  tex->texParameteri(GL_TEXTURE_WRAP_S, wrapS);
  return tex;
}


ImageS Loader::image(std::string const & fileName) {
#ifdef USE_FREEIMAGE
  fipImage fimg;
  if (!fimg.load(fileName.c_str())) {
    std::string er = "File not found or unknown format: " + fileName;
    if (generateOnImageNotFound) {
      std::cerr << er << "\n";
      return generateImage();
    }
    else {
      throw std::runtime_error(er.c_str());
      return nullptr;
    }
  }
  fimg.convertTo32Bits();
  auto img = std::make_shared<Image>();
  img->height = fimg.getHeight();
  img->width = fimg.getWidth();
  img->data = new unsigned int[img->height*img->width];
  img->format = GL_BGRA;
  memcpy(img->data, fimg.accessPixels(), sizeof(int)*img->width*img->height);
  return img;
#elif USE_STB_IMAGE
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 4);
  if (!data)
  {
      std::string er = "File not found or unknown format: " + fileName;
      std::cerr << er << "\n";
      if (generateOnImageNotFound) {
          std::cerr << er << "\n";
          return generateImage();
      }
      else {
          throw std::runtime_error(er.c_str());
          return nullptr;
      }
  }
  auto img = std::make_shared<Image>();
  img->height = height;
  img->width = width;
  img->data = new unsigned int[img->height * img->width];
  img->format = GL_RGBA;
  memcpy(img->data, data, sizeof(int)* img->width* img->height);
  stbi_image_free(data);
  return img;
#else
  // generate random color checkboard
  return generateImage();
#endif
}


ImageS Loader::generateImage(int width, int height) {
  auto img = std::make_shared<Image>();
  img->width = width;
  img->height = height;
  img->data = new unsigned int[width * height];
  unsigned int colors[] = {
    0xFF000000,
    0xFFFF0000,
    0xFF00FF00,
    0xFF0000FF,
    0xFFFFFF00,
    0xFFFF00FF,
    0xFF00FFFF,
    0xFFFFFFFF
  };
  int c1 = rand() % 8;
  int c2 = rand() % 8;
  if (c2 == c1)c2 = (c2 + 1) % 8;
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++) {
      if (((x >> 4) & 1) == ((y >> 4) & 1))
        img->data[y * width + x] = colors[c1];
      else
        img->data[y * width + x] = colors[c2];
    }
  return img;
}
