#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <GL/glut.h>
using namespace std;

#include "WaveFrontOBJ.h" 

//------------------------------------------------------------------------------
// ��ü�� �����ϸ鼭 filename ���� object �� �д´�
// Construct object and read object from filename
WaveFrontOBJ::WaveFrontOBJ(char * filename){

	isFlat = true;
	mode = GL_POLYGON;

	char * line = new char[200];
	char * line_back = new char[200];
	char wspace[] = " \t\n";
	char separator[] = "/";
	char * token;
	int indices[3];
	float x, y, z;
	float tex_u, tex_v;

	ifstream file(filename);

	if (!file){

		cerr << "Cannot open file: " << filename << " exiting." << endl;
		exit(-1);
	}

	while (!file.eof()){

		file.getline(line, 199);
		// first, strip off comments

		if (line[0] == '#'){

			continue;
		}
		else if (!strcmp(line, "")){

			continue;
		}
		else{

			strcpy(line_back, line); // strtok destroys line.

			token = strtok(line, wspace);

			if (!strcmp(token, "v")){

				x = atof(strtok(NULL, wspace));
				y = atof(strtok(NULL, wspace));
				z = atof(strtok(NULL, wspace));

				verts.push_back(Vertex(x, y, z));
			}
			else if (!strcmp(token, "vn")){

				x = atof(strtok(NULL, wspace));
				y = atof(strtok(NULL, wspace));
				z = atof(strtok(NULL, wspace));

				Vector vn(x, y, z);

				vn.Normalize();
				normals.push_back(vn);
			}
			else if (!strcmp(token, "vt")){

				tex_u = atof(strtok(NULL, wspace));
				tex_v = atof(strtok(NULL, wspace));
				texCoords.push_back(TexCoord(tex_u, tex_v));
			}
			else if (!strcmp(token, "f")){

				int vi = (int)vIndex.size();

				faces.push_back(Face(vi));
				Face& curFace = faces.back();

				for (char *p = strtok(NULL, wspace); p; p = strtok(NULL, wspace)){

					indices[0] = 0;
					indices[1] = 0;
					indices[2] = 0;

					char* pos = p;
					int len = (int)strlen(p);

					for (int j = 0, i = 0; j <= len && i < 3; j++){

						if (p[j] == '/' || p[j] == 0){

							p[j] = 0;
							indices[i++] = atoi(pos);
							pos = p + j + 1;
						}
					}

					vIndex.push_back(indices[0] - 1);
					tIndex.push_back(indices[1] - 1);
					nIndex.push_back(indices[2] - 1);
					curFace.vCount++;

					if (indices[2] != 0){

						isFlat = false;
					}
				}
				if (curFace.vCount > 2){

					curFace.normal = faceNormal(verts[vIndex[vi]], verts[vIndex[vi + 1]], verts[vIndex[vi + 2]]);
					curFace.normal.Normalize();
					faceNormals.push_back(curFace.normal);
				}
			}

			else if (!strcmp(token, "g")){      // group
			}
			else if (!strcmp(token, "s")){      // smoothing group
			}
			else if (!strcmp(token, "u")){      // material line
			}
			else if (!strcmp(token, "")){       // blank line
			}
			else{

				cout << line_back << endl;
			}
		}
	}

	vertexNormal();

	computeBoundingBox();
}

//------------------------------------------------------------------------------
Vector WaveFrontOBJ::faceNormal(Vertex & v0, Vertex & v1, Vertex & v2){

	// PA #4 �� ������ �κ� #7
	// face normal �� ����Ѵ�.
	// ���޹��� vertex v0, v1, v2 �� �̿��� ���� 2 ���� �����,
	// �� ���͸� �����Ѵ�.
	// ������ ���ʹ� normalize �ؼ� �����Ѵ�.
	Vector fn(v0.pos.x - v1.pos.x, v0.pos.y - v1.pos.y, v0.pos.z - v1.pos.z);
	Vector temp_v2(v1.pos.x - v2.pos.x, v1.pos.y - v2.pos.y, v1.pos.z - v2.pos.z);

	fn = fn.CrossProduct(temp_v2);
	fn.Normalize();

	return fn;
}

void WaveFrontOBJ::vertexNormal(){

	// PA #4 �� ������ �κ� #8
	// vertex normal �� ����Ѵ�.
	// ��� vertex �� ���ؼ� ������ face �� ã�� �װ͵��� average face normal �� ����Ѵ�.
	// ù ��° for ������ vertex �� �ϳ� ����.
	// �� ��° for ���� �� ��° for ������ �� vertex �� ������ face �� ã�� face normal �� �����ش�.
	// ���� face normal �� normalize �ϸ� average face normal == vertex normal �� �ȴ�.
	// (face normal ���� ���� normalize �Ǿ��� ������ ��հ��� ��ġ�Ѵ�.)
	for (int i = 0; i < (int)verts.size(); i++){

		Vector temp(0, 0, 0);

		for (int j = 0; j < (int)faces.size(); j++){
			for (int k = faces[j].vIndexStart; k < faces[j].vIndexStart + faces[j].vCount; k++){

				if (vIndex[k] == i){

					temp = temp + faces[j].normal;
				}
			}
		}

		temp.Normalize();
		verts[i].normal = temp;
	}
}

//------------------------------------------------------------------------------
// OpenGL API �� ����ؼ� ���Ͽ��� �о�� object �� �׸��� �Լ�.
// Draw object which is read from file
void WaveFrontOBJ::Draw(){

	int i;

	for (int f = 0; f < (int)faces.size(); f++){

		Face& curFace = faces[f];
		glBegin(mode);

		for (int v = 0; v < curFace.vCount; v++){

			int vi = curFace.vIndexStart + v;

			if (isFlat){

				if (v == 0){

					glNormal3f(curFace.normal.x, curFace.normal.y, curFace.normal.z);
				}
			}
			else if ((i = vIndex[vi]) >= 0){

				glNormal3f(verts[i].normal.x, verts[i].normal.y, verts[i].normal.z);
			}
			if ((i = tIndex[vi]) >= 0){

				glTexCoord2f(texCoords[i].u, texCoords[i].v);
			}
			if ((i = vIndex[vi]) >= 0){

				glVertex3f(verts[i].pos.x, verts[i].pos.y, verts[i].pos.z);
			}
		}
		glEnd();
	}
}

void WaveFrontOBJ::Draw_FN(){

	// PA #4 �� ������ �κ� #9
	// face normal �� ǥ���Ѵ�.
	// face normal �� face �� �߽ɿ��� face normal �������� ������ ������ �׸��� �ȴ�.
	// facePos �� face �� �߽��� ������ �ִ� ������.
	// face �� �̷�� vertex 3 ���� ã�� facePos �� ����ϰ�, ũ�⸦ 3���� �����ش�.
	// �� ������ ��� face �� ���� �ݺ��Ѵ�.
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (int i = 0; i < (int)faces.size(); i++){

		Vector facePos(0, 0, 0);
		Vector v1 = (verts[vIndex[faces[i].vIndexStart]].pos);
		Vector v2 = (verts[vIndex[faces[i].vIndexStart + 1]].pos);
		Vector v3 = (verts[vIndex[faces[i].vIndexStart + 2]].pos);

		facePos = v1 + v2 + v3;
		facePos.x = facePos.x / 3.0;
		facePos.y = facePos.y / 3.0;
		facePos.z = facePos.z / 3.0;

		glColor3d(0, 1, 0);
		glVertex3d(facePos.x, facePos.y, facePos.z);
		glVertex3d(facePos.x + faces[i].normal.x, facePos.y + faces[i].normal.y, facePos.z + faces[i].normal.z);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void WaveFrontOBJ::Draw_VN(){

	// PA #4 �� ������ �κ� #9
	// vertex normal �� ǥ���Ѵ�.
	// vertex �� ��ġ���� vertex normal �������� ������ ������ �׸��� �ȴ�.
	// ��� vertex �� ���� �ݺ��Ѵ�.
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (int i = 0; i < (int)verts.size(); i++){

		glColor3d(0, 0, 0);
		glVertex3d(verts[i].pos.x, verts[i].pos.y, verts[i].pos.z);
		glVertex3d(verts[i].pos.x + verts[i].normal.x, verts[i].pos.y + verts[i].normal.y, verts[i].pos.z + verts[i].normal.z);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------
void WaveFrontOBJ::computeBoundingBox(){

	if (verts.size() > 0){

		bbmin.pos.x = verts[0].pos.x;
		bbmin.pos.y = verts[0].pos.y;
		bbmin.pos.z = verts[0].pos.z;
		bbmax.pos.x = verts[0].pos.x;
		bbmax.pos.y = verts[0].pos.y;
		bbmax.pos.z = verts[0].pos.z;

		for (int i = 1; i < (int)verts.size(); i++){

			if (verts[i].pos.x < bbmin.pos.x){

				bbmin.pos.x = verts[i].pos.x;
			}
			if (verts[i].pos.y < bbmin.pos.y){

				bbmin.pos.y = verts[i].pos.y;
			}
			if (verts[i].pos.z < bbmin.pos.z){

				bbmin.pos.z = verts[i].pos.z;
			}
			if (verts[i].pos.x > bbmax.pos.x){

				bbmax.pos.x = verts[i].pos.x;
			}
			if (verts[i].pos.y > bbmax.pos.y){

				bbmax.pos.y = verts[i].pos.y;
			}
			if (verts[i].pos.z > bbmax.pos.z){

				bbmax.pos.z = verts[i].pos.z;
			}
		}
	}
}