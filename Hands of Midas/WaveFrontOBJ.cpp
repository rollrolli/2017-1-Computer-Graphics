#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <GL/glut.h>
using namespace std;

#include "WaveFrontOBJ.h" 

//------------------------------------------------------------------------------
// 객체를 생성하면서 filename 에서 object 를 읽는다
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

	// PA #4 의 수정된 부분 #7
	// face normal 을 계산한다.
	// 전달받은 vertex v0, v1, v2 를 이용해 벡터 2 개를 만들고,
	// 두 벡터를 외적한다.
	// 외적한 벡터는 normalize 해서 리턴한다.
	Vector fn(v0.pos.x - v1.pos.x, v0.pos.y - v1.pos.y, v0.pos.z - v1.pos.z);
	Vector temp_v2(v1.pos.x - v2.pos.x, v1.pos.y - v2.pos.y, v1.pos.z - v2.pos.z);

	fn = fn.CrossProduct(temp_v2);
	fn.Normalize();

	return fn;
}

void WaveFrontOBJ::vertexNormal(){

	// PA #4 의 수정된 부분 #8
	// vertex normal 을 계산한다.
	// 모든 vertex 에 대해서 인접한 face 를 찾아 그것들의 average face normal 을 계산한다.
	// 첫 번째 for 문에서 vertex 를 하나 고른다.
	// 두 번째 for 문과 세 번째 for 문에서 그 vertex 와 인접한 face 를 찾아 face normal 을 더해준다.
	// 더한 face normal 을 normalize 하면 average face normal == vertex normal 이 된다.
	// (face normal 들이 원래 normalize 되었기 때문에 평균값과 일치한다.)
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
// OpenGL API 를 사용해서 파일에서 읽어둔 object 를 그리는 함수.
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

	// PA #4 의 수정된 부분 #9
	// face normal 을 표시한다.
	// face normal 은 face 의 중심에서 face normal 방향으로 나가는 직선을 그리면 된다.
	// facePos 는 face 의 중심을 가지고 있는 변수다.
	// face 를 이루는 vertex 3 개를 찾아 facePos 를 계산하고, 크기를 3으로 나눠준다.
	// 이 과정을 모든 face 에 대해 반복한다.
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

	// PA #4 의 수정된 부분 #9
	// vertex normal 을 표시한다.
	// vertex 의 위치에서 vertex normal 방향으로 나가는 직선을 그리면 된다.
	// 모든 vertex 에 대해 반복한다.
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