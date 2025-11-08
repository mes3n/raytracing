#ifndef SCENES_H
#define SCENES_H

#include "render/hittables.h"

/**
 * Create a specefic scene and add it to world
 *
 * @param scene The id of the scene to be created
 * @param world Pointer to world object which will be added to
 */
int create_scenes(const int scene, Hittables **world);

#endif // SCENES_H
