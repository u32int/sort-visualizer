cmake_minimum_required(VERSION 3.18)
project(sdl2-sort)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2GFX REQUIRED SDL2_gfx)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -pedantic -g -lm")

set(SRCS
  main.c
  game.c
  render.c
  algorithm.c
  util.c
)

set(HEADERS
  game.h
  render.h
  algorithm.h
  util.h
)

add_executable(sdlsort ${SRCS} ${HEADERS})
target_link_libraries(sdlsort ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})
