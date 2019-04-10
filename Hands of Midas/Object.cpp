#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <algorithm>
#include <GL/glut.h>
using namespace std;

#include "Object.h" 

void drawAxis(float len){

	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(len, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, len, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, len);
	glEnd();
	glEnable(GL_LIGHTING);
}

GLuint loadTexture (char *texfile) {
	
	GLuint texture = NULL;
	glGenTextures(1, &texture);

	FILE *fp = fopen(texfile, "rb");
	
	if (!fp) {
		
		printf("ERROR : No %s.\n fail to bind %d\n", texfile, texture);
		
		return false;
	}

	int width, height, channel;

	unsigned char * image = stbi_load_from_file(fp, &width, &height, &channel, 4);
	
	fclose(fp);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	return texture;
}

Objects::Objects(){

	object_num = 0;
}

void Objects::addObject (char * filename) {
	
	Vector center;
	Matrix obj2wld;
	WaveFrontOBJ * obj = new WaveFrontOBJ(filename);

		object_num++;
		center.x = (obj->bbmax.pos.x + obj->bbmin.pos.x) / 2.0;
		center.y = (obj->bbmax.pos.y + obj->bbmin.pos.y) / 2.0;
		center.z = (obj->bbmax.pos.z + obj->bbmin.pos.z) / 2.0;

		object_list.push_back(obj);
		obj2wld_list.push_back(obj2wld);
		center_list.push_back(center);
}

void Objects::addTexture (char * filename, int idx) {

	GLuint texture;

		texture = loadTexture(filename);
		textures.push_back(texture);
		texture_idx.push_back(idx);
}

void Objects::initObjects () {

		for (unsigned int i = 0; i < object_num; i++){

			glPushMatrix();
			glLoadIdentity();
			glTranslatef(4 * i, -(object_list[i]->bbmin.pos.y), 4 * i);
			//glTranslatef(0, -(object_list[i]->bbmin.pos.y), 0);
			glRotated(-90, 0, 1, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[i].matrix());
			glPopMatrix();
		}
}

void Objects::drawObjects () {

		for (unsigned int i = 0; i < object_num; i++){

			glPushMatrix();
			glMultMatrixf(obj2wld_list[i].matrix());

			for (int j = 0; j < textures.size(); j++){

				if (texture_idx[j] == i){
					
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textures[j]);
				}
			}

			// bounding box 표시
			glDisable(GL_LIGHTING);
			glBegin(GL_LINES);
			glColor3d(1, 0, 1);
			glVertex3d(object_list[i]->bbmax.pos.x, object_list[i]->bbmax.pos.y, object_list[i]->bbmax.pos.z);
			glColor3d(1, 1, 0);
			glVertex3d(object_list[i]->bbmin.pos.x, object_list[i]->bbmin.pos.y, object_list[i]->bbmin.pos.z);
			glEnd();
			glEnable(GL_LIGHTING);
			// bounding box 표시

			drawAxis(5);
			glEnable(GL_LIGHTING);
			object_list[i]->Draw();

			glDisable(GL_TEXTURE_2D);

			glPopMatrix();
		}
}

void Objects::moveObject (float x, float y, float z, int idx){

		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(obj2wld_list[idx].matrix());

		glTranslatef(x, y, z);
		object_list[idx]->Draw();

		glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[idx].matrix());
		glPopMatrix();
}

void Objects::rotateObject (float angle, float x, float y, float z, int idx) {

		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(obj2wld_list[idx].matrix());

		//glTranslatef(center_list[idx].x, center_list[idx].y, center_list[idx].z);
		glRotatef(angle, x, y, z);
		//glTranslatef(-center_list[idx].x, -center_list[idx].y, -center_list[idx].z);
		object_list[idx]->Draw();

		glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[idx].matrix());
		glPopMatrix();
}

void Objects::scaleObject (float ratio, int idx) {

	GLfloat mat[16];

		glPushMatrix();
		glMultMatrixf(obj2wld_list[idx].matrix());
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);

		glScalef(ratio, ratio, ratio);

		glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[idx].matrix());
		glPopMatrix();
}

void Objects::set3dPosObject (float x, float y, float z, int idx) {

		glPushMatrix();
		glLoadIdentity();

		glTranslatef(x, y, z);

		glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[idx].matrix());
		glPopMatrix();
}

void Objects::set2dPosObject (float x, float z, int idx) {

		glPushMatrix();
		glLoadIdentity();

		glTranslatef(x, -(object_list[idx]->bbmin.pos.y), z);

		glGetFloatv(GL_MODELVIEW_MATRIX, obj2wld_list[idx].matrix());
		glPopMatrix();
}

int Objects::isColideNaive (int idx) {

	GLfloat temp_mat[16];
	GLfloat target_mat[16];
	Vertex target_bbmax = object_list[idx]->bbmax;
	Vertex target_bbmin = object_list[idx]->bbmin;

		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(obj2wld_list[idx].matrix());
		glGetFloatv(GL_MODELVIEW_MATRIX, target_mat);
		glPopMatrix();
		/*
		for (unsigned int i = 0; i < object_num; i++){

			if (i != idx) {
			
				Vertex temp_bbmax = object_list[i]->bbmax;
				Vertex temp_bbmin = object_list[i]->bbmin;

				glPushMatrix();
				glLoadIdentity();
				glMultMatrixf(obj2wld_list[i].matrix());
				glGetFloatv(GL_MODELVIEW_MATRIX, temp_mat);
				glPopMatrix();

				int condXL = temp_bbmin.pos.x + temp_mat[12] < target_bbmax.pos.x + target_mat[12] && target_bbmax.pos.x + target_mat[12] < temp_bbmax.pos.x + temp_mat[12];
				int condYL = temp_bbmin.pos.y + temp_mat[13] < target_bbmax.pos.y + target_mat[13] && target_bbmax.pos.y + target_mat[13] < temp_bbmax.pos.y + temp_mat[13];
				int condZL = temp_bbmin.pos.z + temp_mat[14] < target_bbmax.pos.z + target_mat[14] && target_bbmax.pos.z + target_mat[14] < temp_bbmax.pos.z + temp_mat[14];
				int condXR = temp_bbmin.pos.x + temp_mat[12] < target_bbmin.pos.x + target_mat[12] && target_bbmin.pos.x + target_mat[12] < temp_bbmax.pos.x + temp_mat[12];
				int condYR = temp_bbmin.pos.y + temp_mat[13] < target_bbmin.pos.y + target_mat[13] && target_bbmin.pos.y + target_mat[13] < temp_bbmax.pos.y + temp_mat[13];
				int condZR = temp_bbmin.pos.z + temp_mat[14] < target_bbmin.pos.z + target_mat[14] && target_bbmin.pos.z + target_mat[14] < temp_bbmax.pos.z + temp_mat[14];

				if (condXL && condZL && (condYL || condYR)) {

					// 
					printf("object : %d, coliison : %d\n", i, 1);

					return 1;
				}
				else if (condXL && condZR && (condYL || condYR)) {

					// z 축 좌측 충돌
					printf("object : %d, coliison : %d\n", i, 2);

					return 2;
				}
				else if (condXR && condZL && (condYL || condYR)) {

					// x 축 우측 충돌
					printf("object : %d, coliison : %d\n", i, 3);

					return 3;
				}
				else if (condXR && condZR && (condYL || condYR)){

					// z 축 우측 충돌
					printf("object : %d, coliison : %d\n", i, 4);

					return 4;
				}
			}
		}
		*/
	return 0;
}

int Objects::isColideStrict(int idx) {



	return 0;
}