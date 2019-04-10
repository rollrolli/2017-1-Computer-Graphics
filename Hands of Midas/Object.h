#ifndef OBJECT_H
#define OBJECT_H


#include <vector>

#include "Matrix.h"
#include "Vector.h"
#include "WaveFrontOBJ.h"
#include "stb_image.h"

class Objects{

public:

	unsigned int object_num;
	std::vector <WaveFrontOBJ *> object_list;
	std::vector <Matrix> obj2wld_list;
	std::vector <GLuint> textures;
	std::vector <int> texture_idx;
	std::vector <Vector> center_list;

	Objects();

	void addObject(char * filename);
	void addTexture(char * filename, int idx);
	void initObjects();
	void drawObjects();
	void moveObject(float x, float y, float z, int idx);
	void rotateObject(float angle, float x, float y, float z, int idx);
	void scaleObject(float ratio, int idx);
	void set3dPosObject(float x, float y, float z, int idx);
	void set2dPosObject(float x, float z, int idx);
	int isColideNaive(int idx);
	int isColideStrict(int idx);
};

#endif